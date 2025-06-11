#include "KeyballKey.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

AKeyballKey::AKeyballKey()
{
    PrimaryActorTick.bCanEverTick = false; // animation handled by keyboard tick
    BaseTransform = GetActorTransform();
    LocalTransform = FTransform::Identity;
    GlobalTransform = FTransform::Identity;
    AnimationSpeed = 5.0f; // adjust for smoothness
    TargetZOffset = 0.f;
    CurrentZOffset = 0.f;
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
    // Lerp toward target Z position
    CurrentZOffset = FMath::FInterpTo(CurrentZOffset, TargetZOffset, DeltaTime, AnimationSpeed);

    FVector LocalOffset(0, 0, CurrentZOffset);
    LocalTransform.SetTranslation(LocalOffset);

    // Apply combined transform
    SetActorTransform(BaseTransform * GlobalTransform * LocalTransform);
}

FString AKeyballKey::GetSymbol() const
{
    return Symbol;
}

void AKeyballKey::ResetKeyTransform()
{
    TargetZOffset = 0.f;
    CurrentZOffset = 0.f;
    LocalTransform = FTransform::Identity;
    SetActorTransform(BaseTransform);
}
