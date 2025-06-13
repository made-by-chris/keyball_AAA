// KeyballKey.h

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

    virtual void Tick(float DeltaTime) override;

    void StartPressAnimation();
    void StartReleaseAnimation();
    void UpdateKeyAnimation(float DeltaTime);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Symbol; // purely for UI/debug

    // Animation
    float AnimationSpeed = 5.0f;
    float TargetZOffset = 0.f;
    float CurrentZOffset = 0.f;
    float MaxZOffset = 20.0f;

    // Transform handling
    struct FKeyTransformState
    {
        FTransform BaseTransform;
        FTransform LocalTransform;
        FTransform GlobalTransform;

        FTransform GetWorldTransform() const
        {
            return BaseTransform * GlobalTransform * LocalTransform;
        }
    };

    FKeyTransformState TransformState;
};
