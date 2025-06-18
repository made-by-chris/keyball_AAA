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
    void ApplyDiagonalCombo(const FKeyballComboResult& Combo);
    
    bool bSharedZActive = false;
    float SharedZTimeElapsed = 0.f;
    float SharedZDuration = 0.f;
    float SharedZStart = 0.f;
    float SharedZTarget = 0.f;

    void StartSharedOffsetZ(float TargetZ, float Duration);
    // void ApplyRippleCombo(const FKeyballComboResult& Combo);
    // void ApplyTiltCombo(const FKeyballComboResult& Combo);
    // void ApplyDiagonalCombo(const FKeyballComboResult& Combo);
    // and later, seesaw, bowl, curl, conveyor force, catapult/"lob", etc.

    TMap<int32, float> DiagonalOriginalZ;
    TMap<int32, float> DiagonalTargetZ;
    float DiagonalLerpTime = 0.f;
    bool bDiagonalActive = false;
    float DiagonalEffectDuration = 6.f;

    void ResetDiagonalEffect();
    FTimerHandle DiagonalResetTimerHandle;

};


