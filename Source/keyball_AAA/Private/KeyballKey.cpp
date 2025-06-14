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

    if (OutlineMaterial)
    {
        StaticMeshForOutlineX->SetMaterial(0, OutlineMaterial);
    }

}

void AKeyballKey::BeginPlay()
{
    Super::BeginPlay();

    TransformState.BaseTransform = GetActorTransform();
    TransformState.LocalTransform = FTransform::Identity;
    TransformState.GlobalTransform = FTransform::Identity;
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
