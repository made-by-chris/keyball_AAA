// KeyballKey.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyballComboDetector.h"
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

    void UpdateOutline(EKeyLEDState NewState);
    void OnConstruction(const FTransform& Transform);

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    USceneComponent* SharedTransformComponent;

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

    void TriggerWhack(const FVector& InAxis, EKeyballDirection Direction);
    void SetCustomZOffset(float NewZ);


    UFUNCTION(BlueprintCallable, Category = "Keyball|Animation")
    void UpdateKeyAnimation(float DeltaTime);
    void SetLocalZOffset(float Z);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Symbol; // purely for UI/debug

    // Animation
    float AnimationSpeed = 5.0f;
        
    float TargetLocalZOffset = 0.f;
    float CurrentLocalZOffset = 0.f;
    float MaxLocalZOffset = 20.f;


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

private:
    // Whack state
    bool bWhackActive = false;
    FTransform WhackTransform;
    FVector WhackAxis = FVector::UpVector;
    float WhackElapsedTime = 0.f;
    float WhackDuration = 0.6f;
    float WhackMaxAngle = 90.f;
    float WhackReturnAngle = 45.f;

    FVector WhackPivot = FVector::ZeroVector;

    TMap<EKeyballDirection, FVector> TopFacePivots;

    void CacheTopFacePivots();


};
