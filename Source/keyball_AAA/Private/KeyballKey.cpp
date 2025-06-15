#include "KeyballKey.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "KeyballComboDetector.h"

AKeyballKey::AKeyballKey()
{
    PrimaryActorTick.bCanEverTick = true;
    AnimationSpeed = 25.0f;
    TargetLocalZOffset =  0.0f;
    CurrentLocalZOffset = 0.0f;
    MaxLocalZOffset = 20.0f;

    SharedTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SharedTransform"));
    RootComponent = SharedTransformComponent;

    StaticMeshX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshX"));
    StaticMeshX->SetupAttachment(SharedTransformComponent);

    StaticMeshForOutlineX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForOutlineX"));
    StaticMeshForOutlineX->SetupAttachment(StaticMeshX);
    StaticMeshForOutlineX->SetWorldScale3D(FVector(1.1f));

}

void AKeyballKey::BeginPlay()
{
    Super::BeginPlay();

    // TransformState.BaseTransform = GetActorTransform();
    TransformState.LocalTransform = FTransform::Identity;

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
}

void AKeyballKey::StartPressAnimation()
{
    TargetLocalZOffset =  MaxLocalZOffset;
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
    CurrentLocalZOffset = FMath::FInterpTo(CurrentLocalZOffset, TargetLocalZOffset, DeltaTime, AnimationSpeed);

    FVector LocalOffset(0, 0, CurrentLocalZOffset);

    TransformState.LocalTransform.SetTranslation(LocalOffset);

    // Base translation (from actor's world placement)
    FTransform BaseLocal = FTransform::Identity;
    BaseLocal.SetTranslation(FVector::ZeroVector); // We now let the actor handle base location

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

void AKeyballKey::UpdateOutline(EKeyLEDState NewState)
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

    OutlineMID->SetVectorParameterValue("Color", ColorToSet);
    OutlineMID->SetScalarParameterValue("Opacity", Opacity);    
}

void AKeyballKey::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
}
