#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballComboDetector.h"
#include "KeyballKeyboard.generated.h"

class AKeyballKey;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FKeycapSpawnData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 Index;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UStaticMesh* Mesh;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FTransform Transform;
};

UCLASS()
class KEYBALL_AAA_API AKeyballKeyboard : public AActor
{
    GENERATED_BODY()

public:
    AKeyballKeyboard();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void RegisterKey(int32 Index, AKeyballKey* Key);

    void OnKeyPressed(int32 PressedIndex, const TArray<int32>& AllPressedIndices, const FKeyballComboResult& Combo);
    void OnKeyReleased(int32 ReleasedIndex, const TArray<int32>& RemainingPressedIndices);

    UFUNCTION(BlueprintCallable)
    void OnComboTriggered(const FKeyballComboResult& Combo);

protected:
    void GenerateFromBlueprintData();

    // Maps index to spawned key actor
    TMap<int32, AKeyballKey*> KeyMap;

    // Tracks which keys are active (e.g., visually pressed)
    TMap<int32, AKeyballKey*> ActiveKeys;

    // Layout input for keyboard generator function
    UPROPERTY(EditAnywhere, Category = "Keyboard")
    TArray<int32> KeyboardIDs;

    // Actor class to spawn for keys
    UPROPERTY(EditAnywhere, Category = "Keyboard")
    TSubclassOf<AKeyballKey> KeyActorClass;

    void ApplyComboEffect(const FKeyballComboResult& Combo);
};
