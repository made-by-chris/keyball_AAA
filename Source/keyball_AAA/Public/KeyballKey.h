#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballKey.generated.h"

UCLASS()
class KEYBALL_AAA_API AKeyballKey : public AActor
{
    GENERATED_BODY()

public:
    AKeyballKey();

    // === Public interface ===
    UFUNCTION(BlueprintCallable)
    void StartPressAnimation();

    UFUNCTION(BlueprintCallable)
    void StartReleaseAnimation();

    UFUNCTION(BlueprintCallable)
    void UpdateKeyAnimation(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void ResetKeyTransform();

    UFUNCTION(BlueprintCallable)
    FString GetSymbol() const;

    // === Configurable ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
    FString Symbol;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
    float MaxZOffset = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
    float AnimationSpeed = 5.0f;

protected:
    // === Internal animation state ===
    FTransform BaseTransform;
    FTransform GlobalTransform;
    FTransform LocalTransform;

    float TargetZOffset;
    float CurrentZOffset;
};
