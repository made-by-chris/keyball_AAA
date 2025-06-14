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

// #if WITH_EDITOR
//     bRunConstructionScriptOnDrag = true;
// #endif
}

void AKeyballKeyboard::GenerateFromBlueprintData()
{
    ClearKeyboard();

    if (KeyboardIDs.Num() == 0)
    {
        KeyboardIDs.Init(2, 40);  // fallback layout
    }

    TArray<FKeycapSpawnData> SpawnData;
    UKeyball_Function_Library::GetKeysForKeyboard(KeyboardIDs, SpawnData);

    // Compute center offset to center the layout
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
            // Attach to keyboard actor
            SpawnedKey->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

            // Slight scale up
            // SpawnedKey->SetActorScale3D(FVector(2.0f));

            // Assign mesh
            if (Data.Mesh)
            {
                if (SpawnedKey->StaticMeshX)
                    SpawnedKey->StaticMeshX->SetStaticMesh(Data.Mesh);
                if (SpawnedKey->StaticMeshForOutlineX)
                {
                    SpawnedKey->StaticMeshForOutlineX->SetStaticMesh(Data.Mesh);
                    if (SpawnedKey->OutlineMaterial)
                    {
                        SpawnedKey->StaticMeshForOutlineX->SetMaterial(0, SpawnedKey->OutlineMaterial);
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Set outline material for key %d: %s"), Data.Index, *SpawnedKey->OutlineMaterial->GetName()));
                        }
                    }
                    else
                    {
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("No outline material set for key %d"), Data.Index));
                        }
                    }
                }
            }

            RegisterKey(Data.Index, SpawnedKey);
        }
        else
        {
        }
    }
}

// #if WITH_EDITOR
// void AKeyballKeyboard::OnConstruction(const FTransform& Transform)
// {
//     Super::OnConstruction(Transform);
//     GenerateFromBlueprintData();
// }
// #endif

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
    // TODO: Handle visual/audio effects for combos
}

void AKeyballKeyboard::OnComboTriggered(const FKeyballComboResult& Combo)
{
    ApplyComboEffect(Combo);
}
