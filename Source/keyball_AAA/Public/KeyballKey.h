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
    // UPROPERTY declarations first
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    USceneComponent* SharedTransformComponent;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshX;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    UStaticMeshComponent* StaticMeshForOutlineX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    UMaterialInterface* MainMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Visuals")
    UMaterialInstanceDynamic* MainMID;

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

    UFUNCTION(BlueprintCallable, Category = "Keyball|Animation")
    void UpdateKeyAnimation(float DeltaTime);
    void SetLocalZOffset(float Z);
    static const float GenericKeyPressZOffset;

    void StartPressAnimation(bool isDoubleTap = false, bool magicActive = false);
    void StartReleaseAnimation();
    void UpdateOutline(EKeyLEDState NewState, FKeyballComboResult Combo = FKeyballComboResult());
    void OnConstruction(const FTransform& Transform);
    void TriggerWhack(const FVector& InAxis, EKeyballDirection Direction);
    void SetCustomZOffset(float NewZ);
    void StartWave(float InPhaseOffset, bool bReverseDirection = false);
    void StopWave();
    void StartSharedOffsetZ(float TargetZ, float Duration);
    void StartTilt(const FVector& InPivot, const FVector& InAxis, float Duration = 1.5f, float PhaseOffset = 0.0f);
    void StopTilt();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Symbol; // purely for UI/debug

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Animation")
    float AnimationSpeed = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Animation")
    float DoubleTapAnimationSpeed = 10.0f;
    float MaxLocalZOffset = 200.f;
    bool bMagicActive = false;

    // Whack animation state machine
    enum class EWhackAnimPhase : uint8 {
        None,
        ToMax,
        ToReturn,
        HoldReturn,
        ToNeutral
    };
    EWhackAnimPhase WhackAnimPhase = EWhackAnimPhase::None;
    float WhackAnimElapsed = 0.f;
    float WhackAnimDuration = 0.f;
    float WhackStartAngle = 0.f;
    float WhackTargetAngle = 0.f;
    float WhackCurrentAngle = 0.f;
    FVector WhackAxis = FVector::UpVector;
    FVector WhackPivot = FVector::ZeroVector;
    float WhackMaxAngle = 90.f;
    float WhackReturnAngle = 45.f;
    float WhackToMaxDuration = 0.08f;
    float WhackToReturnDuration = 0.18f;
    float WhackToNeutralDuration = 0.18f;

    bool bWaveActive = false;
    float WaveTimeElapsed = 0.f;
    float WaveDuration = 0.5f;
    float WavePhaseOffset = 0.f;
    float WaveAmplitude = 23.f;
    float WaveFrequency = 2.f;
    float WaveStartZ = 0.f;
    float WaveTargetZ = 0.f;
    bool bWaveDirectionReversed = false;

    bool bTiltActive = false;
    float TiltTimeElapsed = 0.f;
    float TiltDuration = 0.5f;
    float TiltMaxAngle = 23.f;
    float TiltPhaseOffset = 0.f;
    float TiltStartAngle = 0.f;
    float TiltTargetAngle = 0.f;
    FVector TiltCurrentAxis = FVector::RightVector;
    FVector TiltPivot = FVector::ZeroVector;
    FVector TiltAxis = FVector::RightVector;

    bool bSharedZActive = false;
    float SharedZTimeElapsed = 0.f;
    float SharedZDuration = 0.f;
    float SharedZStart = 0.f;
    float SharedZTarget = 0.f;

    float CurrentLocalZOffset = 0.f;
    float TargetLocalZOffset = 0.f;

    // Charge mechanic
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Charge")
    float ChargeLevel = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Charge")
    float MaxChargeLevel = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Charge")
    float ChargeTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Charge")
    float MaxChargeTime = 1.f; // Time to reach max charge (tunable)

    UFUNCTION(BlueprintCallable, Category = "Keyball|Charge")
    void UpdateCharge(float DeltaTime, bool bIsKeyHeld);

    UFUNCTION(BlueprintCallable, Category = "Keyball|Charge")
    void ResetCharge();

    // --- Sound cues ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* ChargingSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* ChargedSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* WhackReleaseChargedSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* WhackReleaseNormalSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* WhackReleaseUnderchargedSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyball|Sound")
    USoundBase* RefillSound;

    // Add constructor and Tick declaration
    AKeyballKey();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    // Whack state
    FTransform WhackTransform;
    float WhackElapsedTime = 0.f;
    float WhackDuration = 0.6f;

    TMap<EKeyballDirection, FVector> TopFacePivots;

    void CacheTopFacePivots();

    bool bIsDoubleTapActive = false;

    // For sound logic
    bool bWasCharging = false;
    bool bPlayedChargedSound = false;
    bool bPlayedRefillSound = false;
    float LastChargeLevel = 1.0f;
};
