// KeyballKey.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballKey.generated.h"

UENUM(BlueprintType)
enum class EKeyLEDState : uint8
{
    Inactive,
    Active,
    FastActive,
    ShiftActive,
    Cooldown
};

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
    void UpdateOutline(EKeyLEDState NewState);
    void OnConstruction(const FTransform& Transform);

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshX;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshForOutlineX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    UMaterialInterface* OutlineMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* OutlineMID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor ActiveColor = FLinearColor(0,1,0,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor FastActiveColor = FLinearColor(0,1,1,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor InactiveColor = FLinearColor(1,1,1,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor ShiftActiveColor = FLinearColor(1,0,1,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor CooldownColor = FLinearColor(1,0,0,1);

    EKeyLEDState CurrentState = EKeyLEDState::Inactive;

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
