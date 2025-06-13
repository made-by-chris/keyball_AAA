#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "KeyballPlayerController.generated.h"

class AKeyballKeyboard;
class UKeyballComboDetector;
struct FKeyballComboResult;

UCLASS()
class KEYBALL_AAA_API AKeyballPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AKeyballPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    void OnAnyKeyPressed(FKey PressedKey);
    void OnAnyKeyReleased(FKey ReleasedKey);

    int32 GetIndexFromFKey(const FKey& InKey) const;

private:
    AKeyballKeyboard* Keyboard;
    UKeyballComboDetector* ComboDetector;

    UPROPERTY()
    TArray<int32> CurrentlyPressedIndices;
};
