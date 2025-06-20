// awesome! I wonder if we can work the same magic for "whack" which quick lerps to the end point then gently eases to half-way. that's all fine so far.. but you know what im gonna say. if its released, it should lerp back to its neutral rotation. not snap. (whack is on the staticMeshX level btw, a diff transform level to wave and tilt), and if reinvoked, it should not reset itself to neutral first - just start from where it is
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

    void StartPressAnimation(bool isDoubleTap = false, bool magicActive = false);
    void StartReleaseAnimation();

    void UpdateOutline(EKeyLEDState NewState, FKeyballComboResult Combo = FKeyballComboResult());
    void OnConstruction(const FTransform& Transform);

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    USceneComponent* SharedTransformComponent;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshX;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshForOutlineX;

    // material for main mesh

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    UMaterialInterface* MainMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    UMaterialInstanceDynamic* MainMID;

    // material for outline

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor WhackColor = FLinearColor(0,1,0,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor StairsColor = FLinearColor(0,1,0,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor WaveColor = FLinearColor(0,1,0,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor TiltColor = FLinearColor(0,1,0,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    FLinearColor TerrainColor = FLinearColor(0,1,0,1);


    EKeyLEDState CurrentState = EKeyLEDState::Inactive;

    void TriggerWhack(const FVector& InAxis, EKeyballDirection Direction);
    void SetCustomZOffset(float NewZ);

    void StartWave(float InPhaseOffset, bool bReverseDirection = false);
    void StopWave();

    bool bSharedZActive = false;
    float SharedZTimeElapsed = 0.f;
    float SharedZDuration = 0.f;
    float SharedZStart = 0.f;
    float SharedZTarget = 0.f;

    void StartSharedOffsetZ(float TargetZ, float Duration);
    void StartTilt(const FVector& InPivot, const FVector& InAxis, float Duration = 1.5f, float PhaseOffset = 0.0f);
    void StopTilt();




    // Tilt animation
    bool bTiltActive = false;
    float TiltTimeElapsed = 0.f;
    float TiltDuration = 0.5f;
    float TiltMaxAngle = 23.f; // degrees
    float TiltPhaseOffset = 0.f; // Phase offset for staggering
    float TiltStartAngle = 0.f; // Current angle when starting new tilt
    float TiltTargetAngle = 0.f; // Target angle for the tilt
    FVector TiltCurrentAxis = FVector::RightVector; // Current tilt axis to detect direction changes

    FVector TiltPivot = FVector::ZeroVector;
    FVector TiltAxis = FVector::RightVector; // defaults to horizontal row





    // void StartTerrain();
    // void StopTerrain();

    UFUNCTION(BlueprintCallable, Category = "Keyball|Animation")
    void UpdateKeyAnimation(float DeltaTime);
    void SetLocalZOffset(float Z);
    static const float GenericKeyPressZOffset;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Symbol; // purely for UI/debug

    // Animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Animation")
    float AnimationSpeed = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Animation")
    float DoubleTapAnimationSpeed = 10.0f;
    float TargetLocalZOffset = 0.f;
    float CurrentLocalZOffset = 0.f;
    float MaxLocalZOffset = 200.f;
    bool bMagicActive = false;

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

    // Wave animation state
    bool bWaveActive = false;
    float WaveTimeElapsed = 0.f;
    float WaveDuration = 1.5f; // Match tilt duration exactly
    float WavePhaseOffset = 0.f;
    float WaveAmplitude = 23.f; // Keep the higher amplitude for visibility
    float WaveFrequency = 2.f;
    float WaveStartZ = 0.f; // Current Z position when starting new wave
    float WaveTargetZ = 0.f; // Target Z position for the wave
    bool bWaveDirectionReversed = false; // Track wave direction for smooth transitions

    bool bIsDoubleTapActive = false;
};
