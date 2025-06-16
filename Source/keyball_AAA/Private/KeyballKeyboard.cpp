#include "KeyballKeyboard.h"
#include "Keyball_Function_Library.h"
#include "KeyballKey.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AKeyballKeyboard::AKeyballKeyboard()
{
    PrimaryActorTick.bCanEverTick = true;
    KeyboardIDs.Init(0, 40);
    bReplicates = true;
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

    TArray<int32> AffectedKeys;
    if (bIsHorizontal)
    {
        int RowStart = StartIndex / 10 * 10;
        for (int32 i = 0; i < 10; ++i)
        {
            int32 Index = RowStart + i;
            if (KeyMap.Contains(Index))
            {
                AffectedKeys.Add(Index);
            }
        }
    }
    else // vertical
    {
        int Col = StartIndex % 10;
        for (int32 r = 0; r < 4; ++r)
        {
            int32 Index = r * 10 + Col;
            if (KeyMap.Contains(Index))
            {
                AffectedKeys.Add(Index);
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