#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballComboDetector.h"
#include "KeyballKey.h"
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
    // virtual void OnConstruction(const FTransform& Transform) override;

    void RegisterKey(int32 Index, AKeyballKey* Key);

    void OnKeyPressed(int32 PressedIndex, const TArray<int32>& AllPressedIndices, const FKeyballComboResult& Combo, bool leftMagic, bool rightMagic, bool isDoubleTap);
    void OnKeyReleased(int32 ReleasedIndex, const TArray<int32>& RemainingPressedIndices);

    UFUNCTION(BlueprintCallable)
    void OnComboTriggered(const FKeyballComboResult& Combo);

    int32 Sections = 2;

protected:
    void GenerateFromBlueprintData();

    // Maps index to spawned key actor
    TMap<int32, AKeyballKey*> KeyMap;

    // Tracks which keys are active (e.g., visually pressed)
    TMap<int32, AKeyballKey*> ActiveKeys;

    TArray<int32> KeyboardIDs;

    // Actor class to spawn for keys
    UPROPERTY(EditAnywhere, Category = "Keyboard")
    TSubclassOf<AKeyballKey> KeyActorClass;

    void ApplyComboEffect(const FKeyballComboResult& Combo);

    void ClearKeyboard();


    void ApplyWhackCombo(const FKeyballComboResult& Combo);
    FVector GetWhackRotationAxis(EKeyballDirection Direction) const;
    void ApplyStairsCombo(const FKeyballComboResult& Combo);
    void ApplyWaveCombo(const FKeyballComboResult& Combo);
    void ApplyTiltCombo(const FKeyballComboResult& Combo);
    void ApplyTerrainCombo(const FKeyballComboResult& Combo);
    
    // Per-side target Z management - always lerping
    TMap<int32, float> LeftSideTargetZ;
    TMap<int32, float> RightSideTargetZ;
    TMap<int32, float> LeftSideNeutralZ;  // Original neutral positions
    TMap<int32, float> RightSideNeutralZ; // Original neutral positions
    float LerpTime = 0.f;
    bool bLerpActive = false;
    float LerpDuration = 3.0f;  // 1 second lerp duration

    void StartLerpToTargets();
    void SetSideTargetZ(int32 Side, const TMap<int32, float>& NewTargets);
    void ResetSideToNeutral(int32 Side);
    int32 GetSideFromIndex(int32 Index) const; // 0 = left, 1 = right
    
    // Timer handles for debouncing terrain effects
    FTimerHandle LeftSideTerrainTimerHandle;
    FTimerHandle RightSideTerrainTimerHandle;
    
    bool bSharedZActive = false;
    float SharedZTimeElapsed = 0.f;
    float SharedZDuration = 0.f;
    float SharedZStart = 0.f;
    float SharedZTarget = 0.f;

    void StartSharedOffsetZ(float TargetZ, float Duration);
    // void ApplyTiltCombo(const FKeyballComboResult& Combo);
    // void ApplyTerrainCombo(const FKeyballComboResult& Combo);
    // and later, seesaw, bowl, curl, conveyor force, catapult/"lob", etc.

    float TerrainZOffset = 40.f;
    float TerrainDuration = 6.f;
};


