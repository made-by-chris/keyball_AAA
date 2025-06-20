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
    
    // Initialize per-side target Z maps with neutral positions
    LeftSideTargetZ.Empty();
    RightSideTargetZ.Empty();
    LeftSideNeutralZ.Empty();
    RightSideNeutralZ.Empty();
    
    for (auto& Pair : KeyMap)
    {
        int32 Index = Pair.Key;
        AKeyballKey* Key = Pair.Value;
        if (Key)
        {
            FTransform T = Key->GetActorTransform();
            FVector Loc = T.GetLocation();
            float NeutralZ = Loc.Z;
            
            // Determine which side this key belongs to
            int32 Side = GetSideFromIndex(Index);
            if (Side == 0) // Left side
            {
                LeftSideTargetZ.Add(Index, NeutralZ);
                LeftSideNeutralZ.Add(Index, NeutralZ);
            }
            else // Right side
            {
                RightSideTargetZ.Add(Index, NeutralZ);
                RightSideNeutralZ.Add(Index, NeutralZ);
            }
        }
    }
}

void AKeyballKeyboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bLerpActive)
    {
        LerpTime += DeltaTime;
        float Alpha = FMath::Clamp(LerpTime / LerpDuration, 0.f, 1.f);
        
        // Lerp all keys to their current targets
        for (auto& Pair : KeyMap)
        {
            int32 Index = Pair.Key;
            AKeyballKey* Key = Pair.Value;
            if (!Key) continue;
            
            // Get current position
            FTransform T = Key->GetActorTransform();
            FVector Loc = T.GetLocation();
            float CurrentZ = Loc.Z;
            
            // Get target Z for this key's side
            float TargetZ = CurrentZ; // Default to current if no target
            int32 Side = GetSideFromIndex(Index);
            if (Side == 0 && LeftSideTargetZ.Contains(Index))
            {
                TargetZ = LeftSideTargetZ[Index];
            }
            else if (Side == 1 && RightSideTargetZ.Contains(Index))
            {
                TargetZ = RightSideTargetZ[Index];
            }
            
            // Lerp to target
            Loc.Z = FMath::Lerp(CurrentZ, TargetZ, Alpha);
            T.SetLocation(Loc);
            Key->SetActorTransform(T);
        }
        
        // Check if lerp is complete
        if (Alpha >= 1.0f)
        {
            bLerpActive = false;
            LerpTime = 0.f;
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
            AxisVector = FVector::LeftVector;
            break;
        case EKeyballDirection::Left:
        case EKeyballDirection::DownLeft:
            AxisVector = FVector::RightVector;
            break;
        case EKeyballDirection::Down:
            AxisVector = FVector::BackwardVector;
            break;
        case EKeyballDirection::Up:
            AxisVector = FVector::ForwardVector;
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

void AKeyballKeyboard::ApplyDiagonalCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() != 2) return;
    int32 Start = Combo.KeysIndex[0];
    int32 End = Combo.KeysIndex[1];

    int32 StartRow = Start / 10, StartCol = Start % 10;
    int32 EndRow = End / 10, EndCol = End % 10;

    // Determine which side this diagonal affects
    int32 Section = (StartCol <= 4) ? 0 : 1;
    int32 ColMin = (Section == 0) ? 0 : 5;
    int32 ColMax = (Section == 0) ? 4 : 9;

    int32 MaxDist = FMath::Max(FMath::Abs(EndRow - StartRow), FMath::Abs(EndCol - StartCol));
    float StepHeight = TerrainZOffset;

    // Create new targets for this side
    TMap<int32, float> NewTargets;
    
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

            NewTargets.Add(Index, Z);
        }
    }

    // Set the new targets for this side and start lerping
    SetSideTargetZ(Section, NewTargets);
    
    // Set a timer to reset this side back to neutral after some time
    FTimerHandle ResetTimerHandle;
    GetWorldTimerManager().SetTimer(
        ResetTimerHandle,
        [this, Section]() { ResetSideToNeutral(Section); },
        6.0f, // 6 second duration
        false
    );
}

int32 AKeyballKeyboard::GetSideFromIndex(int32 Index) const
{
    // Left side: columns 0-4, Right side: columns 5-9
    int32 Col = Index % 10;
    return (Col <= 4) ? 0 : 1;
}

void AKeyballKeyboard::SetSideTargetZ(int32 Side, const TMap<int32, float>& NewTargets)
{
    if (Side == 0)
    {
        LeftSideTargetZ = NewTargets;
    }
    else
    {
        RightSideTargetZ = NewTargets;
    }
    StartLerpToTargets();
}

void AKeyballKeyboard::ResetSideToNeutral(int32 Side)
{
    // Reset the specified side back to neutral positions
    TMap<int32, float> NeutralTargets;
    
    if (Side == 0)
    {
        NeutralTargets = LeftSideNeutralZ;
    }
    else
    {
        NeutralTargets = RightSideNeutralZ;
    }
    
    SetSideTargetZ(Side, NeutralTargets);
}

void AKeyballKeyboard::StartLerpToTargets()
{
    bLerpActive = true;
    LerpTime = 0.f;
}

