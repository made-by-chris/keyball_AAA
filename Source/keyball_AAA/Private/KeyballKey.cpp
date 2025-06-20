#include "KeyballKey.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "KeyballComboDetector.h"

// Define the static member at file scope
const float AKeyballKey::GenericKeyPressZOffset = 8.f;

AKeyballKey::AKeyballKey()
{
    PrimaryActorTick.bCanEverTick = true;
    AnimationSpeed = 2.5f;
    DoubleTapAnimationSpeed = 20.0f;
    TargetLocalZOffset =  0.0f;
    CurrentLocalZOffset = 0.0f;
    MaxLocalZOffset = 200.0f;

    bSharedZActive = false;
    SharedZTimeElapsed = 0.f;
    SharedZDuration = 0.f;
    SharedZStart = 0.f;
    SharedZTarget = 0.f;
    

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    SharedTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SharedTransform"));
    SharedTransformComponent->SetupAttachment(RootComponent);

    StaticMeshX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshX"));
    StaticMeshX->SetupAttachment(SharedTransformComponent);
    StaticMeshX->SetCollisionProfileName(TEXT("BlockAll")); // Enable collision for the main mesh
    StaticMeshX->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    StaticMeshX->SetCollisionObjectType(ECC_WorldStatic);

    StaticMeshForOutlineX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForOutlineX"));
    StaticMeshForOutlineX->SetupAttachment(StaticMeshX);
    StaticMeshForOutlineX->SetWorldScale3D(FVector(1.1f));
    StaticMeshForOutlineX->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AKeyballKey::BeginPlay()
{
    Super::BeginPlay();

    // TransformState.BaseTransform = GetActorTransform();
    TransformState.LocalTransform = FTransform::Identity;

    MainMID = StaticMeshX->CreateDynamicMaterialInstance(0, MainMaterial);
    StaticMeshX->SetMaterial(0, MainMID);
    UpdateOutline(CurrentState);


    if (StaticMeshForOutlineX && OutlineMaterial)
    {
        OutlineMID = StaticMeshForOutlineX->CreateDynamicMaterialInstance(0, OutlineMaterial);
        StaticMeshForOutlineX->SetMaterial(0, OutlineMID);
        UpdateOutline(CurrentState);
    }

    CacheTopFacePivots();
}

void AKeyballKey::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateKeyAnimation(DeltaTime);
    if (bSharedZActive)
    {
        SharedZTimeElapsed += DeltaTime;
        float t = FMath::Clamp(SharedZTimeElapsed / SharedZDuration, 0.f, 1.f);
        float Z = FMath::Lerp(SharedZStart, SharedZTarget, t);
        SharedTransformComponent->SetRelativeLocation(FVector(0, 0, Z));

        if (SharedZTimeElapsed >= SharedZDuration)
        {
            bSharedZActive = false;
            SharedTransformComponent->SetRelativeLocation(FVector::ZeroVector);
        }
    }

}

void AKeyballKey::StartPressAnimation(bool isDoubleTap, bool magicActive)
{
    TargetLocalZOffset = GenericKeyPressZOffset;
    bMagicActive = magicActive;
    bIsDoubleTapActive = isDoubleTap;
}

void AKeyballKey::StartReleaseAnimation()
{
    TargetLocalZOffset =  0.f;
    bWhackActive = false;
    WhackElapsedTime = 0.f;
    StaticMeshX->SetRelativeTransform(TransformState.LocalTransform); // reset
}

void AKeyballKey::UpdateKeyAnimation(float DeltaTime)
{
    float Speed = bIsDoubleTapActive ? DoubleTapAnimationSpeed : AnimationSpeed;
    CurrentLocalZOffset = FMath::FInterpTo(CurrentLocalZOffset, TargetLocalZOffset, DeltaTime, Speed);

    FVector LocalOffset(0, 0, CurrentLocalZOffset);
    TransformState.LocalTransform.SetTranslation(LocalOffset);

    // Calculate wave effect (translation)
    FVector WaveOffset = FVector::ZeroVector;
    if (bWaveActive)
    {
        WaveTimeElapsed += DeltaTime;
        float t = (WaveTimeElapsed - WavePhaseOffset) / WaveDuration;
        t = FMath::Clamp(t, 0.f, 1.f);

        // Single cycle: go from start to target, then back to neutral
        float OffsetZ;
        if (t < 0.5f)
        {
            // First half: go from start to target
            float halfT = t * 2.f;
            OffsetZ = FMath::Lerp(WaveStartZ, WaveTargetZ, halfT);
        }
        else
        {
            // Second half: go from target back to neutral
            float halfT = (t - 0.5f) * 2.f;
            OffsetZ = FMath::Lerp(WaveTargetZ, 0.f, halfT);
        }

        WaveOffset = FVector(0, 0, OffsetZ);

        if (WaveTimeElapsed >= WaveDuration + WavePhaseOffset)
        {
            StopWave();
        }
    }

    // Calculate tilt effect (rotation)
    FTransform TiltTransform = FTransform::Identity;
    if (bTiltActive)
    {
        TiltTimeElapsed += DeltaTime;
        float t = (TiltTimeElapsed - TiltPhaseOffset) / TiltDuration;
        t = FMath::Clamp(t, 0.f, 1.f);

        // Lerp from start angle to target angle, then back to neutral
        float Angle;
        if (t < 0.5f)
        {
            // First half: go from start to target
            float halfT = t * 2.f;
            Angle = FMath::Lerp(TiltStartAngle, TiltTargetAngle, halfT);
        }
        else
        {
            // Second half: go from target back to neutral
            float halfT = (t - 0.5f) * 2.f;
            Angle = FMath::Lerp(TiltTargetAngle, 0.f, halfT);
        }

        // Rotation: around TiltPivot and TiltAxis
        FVector WorldLocation = GetActorLocation(); // base position of key
        FVector LocalPivotOffset = TiltPivot - WorldLocation;

        FTransform T1 = FTransform(FVector(-LocalPivotOffset));
        FTransform R = FTransform(FQuat(TiltAxis, FMath::DegreesToRadians(Angle)));
        FTransform T2 = FTransform(FVector(LocalPivotOffset));

        TiltTransform = T2 * R * T1;

        if (TiltTimeElapsed >= TiltDuration + TiltPhaseOffset)
        {
            StopTilt();
        }
    }

    // Combine wave and tilt effects
    FTransform CombinedTransform = TiltTransform;
    CombinedTransform.SetLocation(CombinedTransform.GetLocation() + WaveOffset);
    SharedTransformComponent->SetRelativeTransform(CombinedTransform);

    // Handle whack effect (applied to the mesh, not shared transform)
    if (bWhackActive)
    {
        WhackElapsedTime += DeltaTime;
        float t = WhackElapsedTime / WhackDuration;

        float Angle;
        if (t < 0.5f)
        {
            Angle = FMath::Lerp(0.f, WhackMaxAngle, t * 2.f);
        }
        else if (t < 1.f)
        {
            Angle = FMath::Lerp(WhackMaxAngle, WhackReturnAngle, (t - 0.5f) * 2.f);
        }
        else
        {
            Angle = WhackReturnAngle;
        }

        // Rotate around pivot (relative to key mesh center!)
        FTransform T1 = FTransform(FVector(-WhackPivot));
        FTransform R = FTransform(FQuat(WhackAxis.GetSafeNormal(), FMath::DegreesToRadians(Angle)));
        FTransform T2 = FTransform(WhackPivot);
        FTransform Final = T2 * R * T1;

        // Apply final whack * local press movement
        StaticMeshX->SetRelativeTransform(Final * TransformState.LocalTransform);
    }
    else
    {
        StaticMeshX->SetRelativeTransform(TransformState.LocalTransform);
    }
}


void AKeyballKey::TriggerWhack(const FVector& InAxis, EKeyballDirection Direction)
{
    WhackAxis = InAxis;
    WhackElapsedTime = 0.f;
    bWhackActive = true;

    if (TopFacePivots.Contains(Direction))
    {
        WhackPivot = TopFacePivots[Direction];
    }
    else
    {
        WhackPivot = FVector::ZeroVector;
    }
}

void AKeyballKey::CacheTopFacePivots()
{
    if (!StaticMeshX) return;

    FBox Bounds = StaticMeshX->CalcLocalBounds().GetBox();
    FVector Extents = Bounds.GetExtent();
    float TopZ = Bounds.Max.Z;

    TopFacePivots.Add(EKeyballDirection::Up,        FVector(0, -Extents.Y, TopZ));
    TopFacePivots.Add(EKeyballDirection::Down,      FVector(0,  Extents.Y, TopZ));
    TopFacePivots.Add(EKeyballDirection::Left,      FVector(-Extents.X, 0, TopZ));
    TopFacePivots.Add(EKeyballDirection::Right,     FVector( Extents.X, 0, TopZ));
    TopFacePivots.Add(EKeyballDirection::UpLeft,    FVector(-Extents.X, -Extents.Y, TopZ));
    TopFacePivots.Add(EKeyballDirection::UpRight,   FVector( Extents.X, -Extents.Y, TopZ));
    TopFacePivots.Add(EKeyballDirection::DownLeft,  FVector(-Extents.X,  Extents.Y, TopZ));
    TopFacePivots.Add(EKeyballDirection::DownRight, FVector( Extents.X,  Extents.Y, TopZ));
}

void AKeyballKey::SetLocalZOffset(float Z)
{
    TargetLocalZOffset = FMath::Clamp(Z, 0.f, MaxLocalZOffset * 2.f);
}

void AKeyballKey::UpdateOutline(EKeyLEDState NewState, FKeyballComboResult Combo)
{
    CurrentState = NewState;
    if (!OutlineMID) return;

    FLinearColor ColorToSet = InactiveColor;
    float Opacity = 1.0f;

    switch (NewState)
    {
        case EKeyLEDState::Active:       ColorToSet = ActiveColor; break;
        case EKeyLEDState::FastActive:   ColorToSet = FastActiveColor; break;
        case EKeyLEDState::Inactive:     ColorToSet = InactiveColor; Opacity = 0.0f; break;
        case EKeyLEDState::ShiftActive:  ColorToSet = ShiftActiveColor; break;
        case EKeyLEDState::Cooldown:     ColorToSet = CooldownColor; break;
    }

    // let's add another switch which takes priority, and colours stuff based on the combo type instead 
    switch (Combo.MoveType)
    {
        case EKeyballMoveType::Whack:
            ColorToSet = WhackColor;
            break;
        case EKeyballMoveType::Stairs:
            ColorToSet = StairsColor;
            break;
        case EKeyballMoveType::Wave:
            ColorToSet = WaveColor;
            break;
        case EKeyballMoveType::Tilt:
            ColorToSet = TiltColor;
            break;
        case EKeyballMoveType::Terrain:
            ColorToSet = TerrainColor;
            break;
    }

    OutlineMID->SetVectorParameterValue("Color", ColorToSet);
    OutlineMID->SetScalarParameterValue("Opacity", Opacity);    
}

void AKeyballKey::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}

void AKeyballKey::StartWave(float InPhaseOffset, bool bReverseDirection)
{
    // If already waving, capture current position as start point
    if (bWaveActive)
    {
        float currentT = (WaveTimeElapsed - WavePhaseOffset) / WaveDuration;
        currentT = FMath::Clamp(currentT, 0.f, 1.f);

        // Calculate current Z position
        float currentZ;
        if (currentT < 0.5f)
        {
            // First half: going from start to target
            float halfT = currentT * 2.f;
            currentZ = FMath::Lerp(WaveStartZ, WaveTargetZ, halfT);
        }
        else
        {
            // Second half: going from target back to neutral
            float halfT = (currentT - 0.5f) * 2.f;
            currentZ = FMath::Lerp(WaveTargetZ, 0.f, halfT);
        }
        
        // Check if the wave direction has changed
        bool bDirectionChanged = bWaveDirectionReversed != bReverseDirection;
        
        if (bDirectionChanged)
        {
            // If direction changed, flip the sign of the current position
            // This makes +15 in one direction become -15 in the opposite direction
            WaveStartZ = -currentZ;
            WaveTargetZ = WaveAmplitude;
        }
        else
        {
            // Same direction, continue normally
            WaveStartZ = currentZ;
            WaveTargetZ = WaveAmplitude;
        }
    }
    else
    {
        WaveStartZ = 0.f; // Start from neutral if not currently waving
        WaveTargetZ = WaveAmplitude;
    }
    
    WaveTimeElapsed = 0.f;
    WavePhaseOffset = InPhaseOffset;
    bWaveDirectionReversed = bReverseDirection;
    bWaveActive = true;
}

void AKeyballKey::StopWave()
{
    bWaveActive = false;
    WaveTimeElapsed = 0.f;
    // Don't reset SharedTransformComponent - let the animation system handle it
}


void AKeyballKey::StartSharedOffsetZ(float TargetZ, float Duration)
{
    bSharedZActive = true;
    SharedZTimeElapsed = 0.f;
    SharedZDuration = Duration;
    SharedZStart = SharedTransformComponent->GetRelativeLocation().Z;
    SharedZTarget = TargetZ;
}

void AKeyballKey::StartTilt(const FVector& InPivot, const FVector& InAxis, float Duration, float PhaseOffset)
{
    FVector NewAxis = InAxis.GetSafeNormal();
    
    // Check if the tilt direction has changed
    bool bDirectionChanged = bTiltActive && !TiltCurrentAxis.Equals(NewAxis, 0.1f);
    
    // If already tilting, capture current angle as start point
    if (bTiltActive)
    {
        float currentT = (TiltTimeElapsed - TiltPhaseOffset) / TiltDuration;
        currentT = FMath::Clamp(currentT, 0.f, 1.f);

        // Calculate current angle
        float currentAngle;
        if (currentT < 0.5f)
        {
            // First half: going from start to target
            float halfT = currentT * 2.f;
            currentAngle = FMath::Lerp(TiltStartAngle, TiltTargetAngle, halfT);
        }
        else
        {
            // Second half: going from target back to neutral
            float halfT = (currentT - 0.5f) * 2.f;
            currentAngle = FMath::Lerp(TiltTargetAngle, 0.f, halfT);
        }
        
        if (bDirectionChanged)
        {
            // If direction changed, flip the sign of the current angle
            // This makes 23° in one direction become -23° in the opposite direction
            TiltStartAngle = -currentAngle;
            TiltTargetAngle = TiltMaxAngle;
        }
        else
        {
            // Same direction, continue normally
            TiltStartAngle = currentAngle;
            TiltTargetAngle = TiltMaxAngle;
        }
    }
    else
    {
        TiltStartAngle = 0.f; // Start from neutral if not currently tilting
        TiltTargetAngle = TiltMaxAngle;
    }
    
    TiltPivot = InPivot;
    TiltAxis = NewAxis;
    TiltCurrentAxis = NewAxis;
    TiltTimeElapsed = 0.f;
    TiltDuration = Duration;
    TiltPhaseOffset = PhaseOffset;
    bTiltActive = true;
}

void AKeyballKey::StopTilt()
{
    bTiltActive = false;
    TiltTimeElapsed = 0.f;
    // Don't reset SharedTransformComponent - let the animation system handle it
}
