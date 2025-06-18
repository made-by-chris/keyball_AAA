#include "KeyballKeyboard.h"
#include "Keyball_Function_Library.h"
#include "KeyballKey.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

AKeyballKeyboard::AKeyballKeyboard()
{
    PrimaryActorTick.bCanEverTick = true;
    KeyboardIDs.Init(0, 40);
    bReplicates = true;
    Sections = 2;
}

void AKeyballKeyboard::GenerateFromBlueprintData()
{
    ClearKeyboard();

    if (KeyboardIDs.Num() == 0)
    {
        KeyboardIDs.Init(0, 40);  // fallback layout
    }

    TArray<FKeycapSpawnData> SpawnData;
    UKeyball_Function_Library::GetKeysForKeyboard(KeyboardIDs, SpawnData);

    FVector CenterOffset = FVector::ZeroVector;
    for (const FKeycapSpawnData& Data : SpawnData)
    {
        CenterOffset += Data.Transform.GetLocation();
    }
    CenterOffset /= SpawnData.Num();

    for (FKeycapSpawnData& Data : SpawnData)
    {
        FVector AdjustedLocation = Data.Transform.GetLocation() - CenterOffset;
        Data.Transform.SetLocation(AdjustedLocation);
    }

    for (const FKeycapSpawnData& Data : SpawnData)
    {
        if (!KeyActorClass) continue;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AKeyballKey* SpawnedKey = GetWorld()->SpawnActor<AKeyballKey>(KeyActorClass, Data.Transform, SpawnParams);

        if (SpawnedKey)
        {
            SpawnedKey->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

            if (Data.Mesh)
            {
                if (SpawnedKey->StaticMeshX)
                    SpawnedKey->StaticMeshX->SetStaticMesh(Data.Mesh);
                if (SpawnedKey->StaticMeshForOutlineX)
                {
                    SpawnedKey->StaticMeshForOutlineX->SetStaticMesh(Data.Mesh);
                }
            }

            RegisterKey(Data.Index, SpawnedKey);
        }
    }
}

void AKeyballKeyboard::BeginPlay()
{
    Super::BeginPlay();
    GenerateFromBlueprintData();
}

void AKeyballKeyboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bDiagonalActive)
    {
        DiagonalLerpTime += DeltaTime;
        float Alpha = FMath::Clamp(DiagonalLerpTime / 1.0f, 0.f, 1.f); // 1s lerp duration
        for (auto& Elem : DiagonalTargetZ)
        {
            int32 Index = Elem.Key;
            float TargetZ = Elem.Value;
            float StartZ = DiagonalOriginalZ.Contains(Index) ? DiagonalOriginalZ[Index] : 0.f;
            if (!KeyMap.Contains(Index)) continue;
            AKeyballKey* Key = KeyMap[Index];
            if (!Key) continue;
            FTransform T = Key->GetActorTransform();
            FVector Loc = T.GetLocation();
            Loc.Z = FMath::Lerp(StartZ, TargetZ, Alpha);
            T.SetLocation(Loc);
            Key->SetActorTransform(T);
        }
    }

    for (auto& Pair : ActiveKeys)
    {
        if (AKeyballKey* Key = Pair.Value)
        {
            Key->UpdateKeyAnimation(DeltaTime);
        }
    }
}

void AKeyballKeyboard::ClearKeyboard()
{
    for (auto& Pair : KeyMap)
    {
        if (Pair.Value && !Pair.Value->IsPendingKill())
        {
            Pair.Value->Destroy();
        }
    }

    KeyMap.Empty();
    ActiveKeys.Empty();
}

void AKeyballKeyboard::RegisterKey(int32 Index, AKeyballKey* Key)
{
    KeyMap.Add(Index, Key);
}

void AKeyballKeyboard::OnKeyPressed(int32 PressedIndex, const TArray<int32>& AllPressedIndices, const FKeyballComboResult& Combo, bool leftMagic, bool rightMagic, bool isDoubleTap)
{
    if (AKeyballKey** Found = KeyMap.Find(PressedIndex))
    {
        AKeyballKey* Key = *Found;
        if (Combo.MoveType != EKeyballMoveType::Whack)
        {
            // Determine which magic to use based on which half of the board
            bool magicActive = (PressedIndex % 10 <= 4) ? leftMagic : rightMagic;
            Key->StartPressAnimation(isDoubleTap, magicActive);
        }
        Key->UpdateOutline(EKeyLEDState::Active, Combo);
        ActiveKeys.Add(PressedIndex, Key);
        // You can use leftMagic, rightMagic, isDoubleTap here for further effects if needed
    }

    if (Combo.MoveType != EKeyballMoveType::None)
    {
        ApplyComboEffect(Combo);
    }
}

void AKeyballKeyboard::OnKeyReleased(int32 ReleasedIndex, const TArray<int32>& RemainingPressedIndices)
{
    if (AKeyballKey** Found = KeyMap.Find(ReleasedIndex))
    {
        AKeyballKey* Key = *Found;
        Key->StartReleaseAnimation();
        Key->UpdateOutline(EKeyLEDState::Inactive);
        ActiveKeys.Remove(ReleasedIndex);
    }
}

void AKeyballKeyboard::ApplyComboEffect(const FKeyballComboResult& Combo)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("ApplyComboEffect: %s"), 
        *UEnum::GetValueAsString(Combo.MoveType)));
    FString ComboIndicesString = FString::Printf(TEXT("Combo indices: %d, %d, %s"), Combo.KeysIndex[0], Combo.KeysIndex[1], *UEnum::GetValueAsString(Combo.Direction));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, ComboIndicesString);    

    switch (Combo.MoveType)
    {
        case EKeyballMoveType::Whack:
            ApplyWhackCombo(Combo);
            break;
        case EKeyballMoveType::Stairs:
            ApplyStairsCombo(Combo);
            break;
        case EKeyballMoveType::Wave:
            ApplyWaveCombo(Combo);
            break;
        case EKeyballMoveType::Tilt:
            ApplyTiltCombo(Combo);
            break;
        case EKeyballMoveType::Diagonal:
            ApplyDiagonalCombo(Combo);
            break;

        // Future cases:
        // case EKeyballMoveType::Stairs: ApplyStairsCombo(Combo); break;
        // case EKeyballMoveType::Wave:   ApplyWaveCombo(Combo); break;

        default:
            break;
    }
}

void AKeyballKeyboard::ApplyWhackCombo(const FKeyballComboResult& Combo)
{
    int32 IndexA = Combo.KeysIndex[0];
    // Do not activate IndexB
    EKeyballDirection Direction = Combo.Direction;

    if (!KeyMap.Contains(IndexA)) return;

    AKeyballKey* KeyA = KeyMap[IndexA];
    if (!KeyA) return;

    FVector Axis = GetWhackRotationAxis(Direction);
    KeyA->TriggerWhack(Axis, Direction);
}


FVector AKeyballKeyboard::GetWhackRotationAxis(EKeyballDirection Direction) const
{
    static const TMap<EKeyballDirection, FVector> AxisMap = {
        { EKeyballDirection::Up,        FVector(0, 1, 0) },
        { EKeyballDirection::Down,      FVector(0, -1, 0) },
        { EKeyballDirection::Left,      FVector(1, 0, 0) },
        { EKeyballDirection::Right,     FVector(-1, 0, 0) },
        { EKeyballDirection::UpRight,   FVector(-1, 1, 0).GetSafeNormal() },
        { EKeyballDirection::UpLeft,    FVector(1, 1, 0).GetSafeNormal() },
        { EKeyballDirection::DownRight, FVector(-1, -1, 0).GetSafeNormal() },
        { EKeyballDirection::DownLeft,  FVector(1, -1, 0).GetSafeNormal() },
        { EKeyballDirection::None,      FVector::UpVector }
    };

    const FVector* Found = AxisMap.Find(Direction);
    return Found ? *Found : FVector::UpVector;
}

void AKeyballKeyboard::OnComboTriggered(const FKeyballComboResult& Combo)
{
    ApplyComboEffect(Combo);
}

void AKeyballKeyboard::ApplyStairsCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() != 3) return;

    TArray<float> StepHeights = { 
        AKeyballKey::GenericKeyPressZOffset * 1, 
        AKeyballKey::GenericKeyPressZOffset * 2, 
        AKeyballKey::GenericKeyPressZOffset * 3 
    };
    
    for (int32 i = 0; i < 3; ++i)
    {
        int32 KeyIndex = Combo.KeysIndex[i];
        if (AKeyballKey* Key = KeyMap.FindRef(KeyIndex))
        {
            // Cancel any active whack
            Key->StartReleaseAnimation();

            Key->SetLocalZOffset(StepHeights[i]);
            ActiveKeys.Add(KeyIndex, Key);
        }
    }
}

void AKeyballKeyboard::ApplyWaveCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() < 2) return;

    int32 StartIndex = Combo.KeysIndex[0];
    int32 EndIndex = Combo.KeysIndex.Last();

    // Determine direction: row or column
    bool bIsHorizontal = FMath::Abs(EndIndex - StartIndex) <= 4; // 5-wide row

    int32 Section = (StartIndex % 10 <= 4) ? 0 : 1;

    TArray<int32> AffectedKeys;
    if (bIsHorizontal)
    {
        int RowStart = StartIndex / 10 * 10;
        for (int32 i = 0; i < 10; ++i)
        {
            int32 Index = RowStart + i;
            if ((Section == 0 && i <= 4) || (Section == 1 && i >= 5))
            {
                if (KeyMap.Contains(Index))
                {
                    AffectedKeys.Add(Index);
                }
            }
        }
    }
    else // vertical
    {
        int Col = StartIndex % 10;
        if ((Section == 0 && Col <= 4) || (Section == 1 && Col >= 5))
        {
            for (int32 r = 0; r < 4; ++r)
            {
                int32 Index = r * 10 + Col;
                if (KeyMap.Contains(Index))
                {
                    AffectedKeys.Add(Index);
                }
            }
        }
    }

    for (int32 i = 0; i < AffectedKeys.Num(); ++i)
    {
        int32 Index = AffectedKeys[i];
        AKeyballKey* Key = KeyMap[Index];
        float PhaseOffset = i * PI / 4.f; // customize this for smoother spacing
        Key->StartWave(PhaseOffset);
    }
}

void AKeyballKeyboard::ApplyTiltCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() < 2) return;

    int32 StartIndex = Combo.KeysIndex[0];
    int32 EndIndex = Combo.KeysIndex.Last();

    bool bIsHorizontal = FMath::Abs(EndIndex - StartIndex) <= 4;

    int32 Section = (StartIndex % 10 <= 4) ? 0 : 1;

    TArray<int32> AxisKeys;

    if (bIsHorizontal)
    {
        int32 RowStart = StartIndex / 10 * 10;
        for (int32 i = 0; i < 10; ++i)
        {
            int32 Index = RowStart + i;
            if ((Section == 0 && i <= 4) || (Section == 1 && i >= 5))
            {
                if (KeyMap.Contains(Index)) AxisKeys.Add(Index);
            }
        }
    }
    else
    {
        int32 Col = StartIndex % 10;
        if ((Section == 0 && Col <= 4) || (Section == 1 && Col >= 5))
        {
            for (int32 r = 0; r < 4; ++r)
            {
                int32 Index = r * 10 + Col;
                if (KeyMap.Contains(Index)) AxisKeys.Add(Index);
            }
        }
    }

    if (AxisKeys.Num() < 2) return;

    // Determine anchor (pivot) based on direction
    int32 AnchorIndex = -1;
    switch (Combo.Direction)
    {
        case EKeyballDirection::Right:
        case EKeyballDirection::Down:
        case EKeyballDirection::DownRight:
        case EKeyballDirection::DownLeft:
            AnchorIndex = AxisKeys[0]; // first key in axis
            break;
        case EKeyballDirection::Left:
        case EKeyballDirection::Up:
        case EKeyballDirection::UpLeft:
        case EKeyballDirection::UpRight:
            AnchorIndex = AxisKeys.Last(); // last key in axis
            break;
        default:
            AnchorIndex = AxisKeys[0]; // fallback
            break;
    }

    AKeyballKey* AnchorKey = KeyMap.FindRef(AnchorIndex);
    if (!AnchorKey) return;
    FVector PivotWorld = AnchorKey->GetActorLocation();

    // Use the combo's direction to determine tilt direction
    FVector AxisVector;
    switch (Combo.Direction)
    {
        case EKeyballDirection::Right:
        case EKeyballDirection::DownRight:
            AxisVector = FVector::BackwardVector;
            break;
        case EKeyballDirection::Left:
        case EKeyballDirection::DownLeft:
            AxisVector = FVector::ForwardVector;
            break;
        case EKeyballDirection::Down:
            AxisVector = FVector::LeftVector;
            break;
        case EKeyballDirection::Up:
            AxisVector = FVector::RightVector;
            break;
        default:
            if (Combo.Direction == EKeyballDirection::UpRight)
                AxisVector = FVector::BackwardVector;
            else if (Combo.Direction == EKeyballDirection::UpLeft)
                AxisVector = FVector::ForwardVector;
            else
                AxisVector = FVector::LeftVector;
            break;
    }

    for (int32 i = 0; i < AxisKeys.Num(); ++i)
    {
        AKeyballKey* Key = KeyMap[AxisKeys[i]];
        if (!Key) continue;
        Key->StartTilt(PivotWorld, AxisVector, 1.5f);
    }
}

void AKeyballKeyboard::ResetDiagonalEffect()
{
    bDiagonalActive = false;
    // Reset all affected keys to original Z
    for (auto& Elem : DiagonalOriginalZ)
    {
        int32 Index = Elem.Key;
        if (!KeyMap.Contains(Index)) continue;
        AKeyballKey* Key = KeyMap[Index];
        if (!Key) continue;
        FTransform T = Key->GetActorTransform();
        FVector Loc = T.GetLocation();
        Loc.Z = Elem.Value;
        T.SetLocation(Loc);
        Key->SetActorTransform(T);
    }
    DiagonalOriginalZ.Empty();
    DiagonalTargetZ.Empty();
}

void AKeyballKeyboard::ApplyDiagonalCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() != 2) return;
    int32 Start = Combo.KeysIndex[0];
    int32 End = Combo.KeysIndex[1];

    int32 StartRow = Start / 10, StartCol = Start % 10;
    int32 EndRow = End / 10, EndCol = End % 10;

    int32 Section = (StartCol <= 4) ? 0 : 1;
    int32 ColMin = (Section == 0) ? 0 : 5;
    int32 ColMax = (Section == 0) ? 4 : 9;

    int32 MaxDist = FMath::Max(FMath::Abs(EndRow - StartRow), FMath::Abs(EndCol - StartCol));
    float StepHeight = 10.f;

    DiagonalOriginalZ.Empty();
    DiagonalTargetZ.Empty();
    bDiagonalActive = true;
    DiagonalLerpTime = 0.f;

    // Clear any previous timer
    GetWorldTimerManager().ClearTimer(DiagonalResetTimerHandle);

    for (int32 Row = 0; Row < 4; ++Row)
    {
        for (int32 Col = ColMin; Col <= ColMax; ++Col)
        {
            int32 Index = Row * 10 + Col;
            if (!KeyMap.Contains(Index)) continue;
            AKeyballKey* Key = KeyMap[Index];
            if (!Key) continue;

            int32 Dist = FMath::Max(FMath::Abs(EndRow - Row), FMath::Abs(EndCol - Col));
            int32 Height = MaxDist - Dist;
            Height = FMath::Clamp(Height, 0, MaxDist);
            float Z = Height * StepHeight;

            FTransform T = Key->GetActorTransform();
            FVector Loc = T.GetLocation();
            DiagonalOriginalZ.Add(Index, Loc.Z);
            DiagonalTargetZ.Add(Index, Z);
        }
    }

    // Set timer to turn off effect after DiagonalEffectDuration
    GetWorldTimerManager().SetTimer(
        DiagonalResetTimerHandle,
        this,
        &AKeyballKeyboard::ResetDiagonalEffect,
        DiagonalEffectDuration,
        false
    );
}

