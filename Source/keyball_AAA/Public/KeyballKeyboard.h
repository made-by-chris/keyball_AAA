// KeyballKeyboard.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballComboDetector.h"
#include "KeyballKeyboard.generated.h"

class ABP_Key; // forward declaration

UCLASS()
class AKeyballKeyboard : public AActor
{
    GENERATED_BODY()

public:
    AKeyballKeyboard();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnComboTriggered(const struct FKeyballCombo& Combo); // called by controller/multicast

    // Optional: register keys from BP_Key instances
    void RegisterKey(const FString& Symbol, ABP_Key* Key);

protected:
    // Stores each key by its symbol (e.g. "Q", "W", "E", etc.)
    UPROPERTY()
    TMap<FString, ABP_Key*> KeyMap;

    // Per-key animation/transformation state
    TMap<FString, FKeyTransformState> TransformStates;

    // Handles all combo effects and animation logic
    void ApplyComboEffect(const struct FKeyballCombo& Combo);
    void UpdateKeyTransforms(float DeltaTime);
};
