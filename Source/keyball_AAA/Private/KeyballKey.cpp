#include "KeyballKey.h"

AKeyballKey::AKeyballKey()
{
    PrimaryActorTick.bCanEverTick = true;
    AnimationSpeed = 25.0f;
    TargetZOffset = 0.0f;
    CurrentZOffset = 0.0f;
    MaxZOffset = 20.0f;

    StaticMeshX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshX"));
    RootComponent = StaticMeshX;

    StaticMeshForOutlineX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForOutlineX"));
    StaticMeshForOutlineX->SetupAttachment(RootComponent);

    // scale outline mesh to 1.1
    StaticMeshForOutlineX->SetWorldScale3D(FVector(1.1f, 1.1f, 1.1f));
}

void AKeyballKey::BeginPlay()
{
    Super::BeginPlay();

    TransformState.BaseTransform = GetActorTransform();
    TransformState.LocalTransform = FTransform::Identity;
    TransformState.GlobalTransform = FTransform::Identity;

    if (StaticMeshForOutlineX && OutlineMaterial)
    {
        OutlineMID = StaticMeshForOutlineX->CreateDynamicMaterialInstance(0, OutlineMaterial);
        UpdateOutline(CurrentState); // Set initial color
    }
}

void AKeyballKey::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateKeyAnimation(DeltaTime);
}

void AKeyballKey::StartPressAnimation()
{
    TargetZOffset = MaxZOffset;
}

void AKeyballKey::StartReleaseAnimation()
{
    TargetZOffset = 0.f;
}

void AKeyballKey::UpdateKeyAnimation(float DeltaTime)
{
    CurrentZOffset = FMath::FInterpTo(CurrentZOffset, TargetZOffset, DeltaTime, AnimationSpeed);

    FVector LocalOffset(0, 0, CurrentZOffset);
    TransformState.LocalTransform.SetTranslation(LocalOffset);

    SetActorTransform(TransformState.GetWorldTransform());
}

void AKeyballKey::UpdateOutline(EKeyLEDState NewState)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Updating outline to %s"), *UEnum::GetValueAsString(NewState)));
    if (StaticMeshForOutlineX)
    {
        UMaterialInterface* CurrentMat = StaticMeshForOutlineX->GetMaterial(0);
        FString MatName = CurrentMat ? CurrentMat->GetName() : TEXT("nullptr");
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Outline mesh material: %s"), *MatName));
    }
    CurrentState = NewState;
    if (!OutlineMID) return;

    FLinearColor ColorToSet = InactiveColor;
    float Opacity = 1.0f;

    switch (NewState)
    {
        case EKeyLEDState::Active:
            ColorToSet = ActiveColor;
            break;
        case EKeyLEDState::FastActive:
            ColorToSet = FastActiveColor;
            break;
        case EKeyLEDState::Inactive:
            ColorToSet = InactiveColor;
            break;
        case EKeyLEDState::ShiftActive:
            ColorToSet = ShiftActiveColor;
            break;
        case EKeyLEDState::Cooldown:
            ColorToSet = CooldownColor;
            break;
    }

    // If you want to set opacity separately, you can do so here
    // For example, if (NewState == EKeyLEDState::Cooldown) Opacity = 0.0f;

    OutlineMID->SetVectorParameterValue("Color", ColorToSet);
    OutlineMID->SetScalarParameterValue("Opacity", Opacity);
}

void AKeyballKey::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (StaticMeshForOutlineX && OutlineMaterial)
    {
        StaticMeshForOutlineX->SetMaterial(0, OutlineMaterial);
    }
}
