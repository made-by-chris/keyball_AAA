#include "KeyballKeyboard.h"
#include "KeyballKey.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AKeyballKeyboard::AKeyballKeyboard()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
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


void AKeyballKeyboard::GenerateFromBlueprintData()
{
    TArray<FKeycapSpawnData> SpawnData;

    // Default to "nubs" layout if not set
    if (KeyboardIDs.Num() == 0)
    {
        KeyboardIDs.Init(0, 40);
    }

    // Call the Blueprint function
    UKeyball_Function_Library::GetKeysForKeyboard(KeyboardIDs, SpawnData);

    for (const FKeycapSpawnData& Data : SpawnData)
    {
        if (!KeyActorClass) continue;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AKeyballKey* SpawnedKey = GetWorld()->SpawnActor<AKeyballKey>(KeyActorClass, Data.Transform, SpawnParams);

        if (SpawnedKey)
        {
            RegisterKey(Data.Index, SpawnedKey);

            // Attach to the keyboard actor for hierarchy
            SpawnedKey->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

            // Optionally set the mesh (add a method to AKeyballKey to do this if needed)
            UStaticMeshComponent* MeshComp = SpawnedKey->FindComponentByClass<UStaticMeshComponent>();
            if (MeshComp && Data.Mesh)
            {
                MeshComp->SetStaticMesh(Data.Mesh);
            }
        }
    }
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
        ActiveKeys.Remove(ReleasedIndex);
    }
}

void AKeyballKeyboard::ApplyComboEffect(const FKeyballComboResult& Combo)
{
    // TODO: Implement combo-driven effects like whack, ripple, etc.
}

void AKeyballKeyboard::OnComboTriggered(const FKeyballComboResult& Combo)
{
    ApplyComboEffect(Combo);
}
