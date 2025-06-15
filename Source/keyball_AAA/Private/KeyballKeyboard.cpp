#include "KeyballKeyboard.h"
#include "Keyball_Function_Library.h"
#include "KeyballKey.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AKeyballKeyboard::AKeyballKeyboard()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AKeyballKeyboard::GenerateFromBlueprintData()
{
    ClearKeyboard();

    if (KeyboardIDs.Num() == 0)
    {
        KeyboardIDs.Init(5, 40);  // fallback layout
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

void AKeyballKeyboard::OnKeyPressed(int32 PressedIndex, const TArray<int32>& AllPressedIndices, const FKeyballComboResult& Combo)
{
    if (AKeyballKey** Found = KeyMap.Find(PressedIndex))
    {
        AKeyballKey* Key = *Found;
        Key->StartPressAnimation();
        Key->UpdateOutline(EKeyLEDState::Active);
        ActiveKeys.Add(PressedIndex, Key);
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

        // Future cases:
        // case EKeyballMoveType::Stairs: ApplyStairsCombo(Combo); break;
        // case EKeyballMoveType::Wave:   ApplyWaveCombo(Combo); break;

        default:
            break;
    }
}

void AKeyballKeyboard::ApplyWhackCombo(const FKeyballComboResult& Combo)
{
    if (Combo.KeysIndex.Num() < 2) return;

    int32 IndexA = Combo.KeysIndex[0];
    int32 IndexB = Combo.KeysIndex[1];
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
        { EKeyballDirection::Up,        FVector(1, 0, 0) },
        { EKeyballDirection::Down,      FVector(-1, 0, 0) },
        { EKeyballDirection::Left,      FVector(0, 1, 0) },
        { EKeyballDirection::Right,     FVector(0, -1, 0) },
        { EKeyballDirection::UpRight,   FVector(1, -1, 0).GetSafeNormal() },
        { EKeyballDirection::UpLeft,    FVector(1, 1, 0).GetSafeNormal() },
        { EKeyballDirection::DownRight, FVector(-1, -1, 0).GetSafeNormal() },
        { EKeyballDirection::DownLeft,  FVector(-1, 1, 0).GetSafeNormal() },
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

    // Define stair step heights
    TArray<float> StepHeights = { 10.f, 20.f, 30.f };

    for (int32 i = 0; i < 3; ++i)
    {
        int32 KeyIndex = Combo.KeysIndex[i];
        if (AKeyballKey* Key = KeyMap.FindRef(KeyIndex))
        {
            Key->SetLocalZOffset(StepHeights[i]);
            ActiveKeys.Add(KeyIndex, Key); // Ensure it keeps ticking
        }
    }
}