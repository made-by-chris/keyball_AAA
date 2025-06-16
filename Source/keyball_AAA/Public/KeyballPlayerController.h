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

    UFUNCTION()
    void HandleAnyKeyPressed();

    UFUNCTION()
    void HandleAnyKeyReleased();

    void OnAnyKeyPressed(FKey PressedKey);     // Your original method
    void OnAnyKeyReleased(FKey ReleasedKey);   // Your original method


public:
    AKeyballPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    int32 GetIndexFromFKey(const FKey& InKey) const;

    virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

private:
    AKeyballKeyboard* Keyboard;
    UKeyballComboDetector* ComboDetector;

    UPROPERTY()
    TArray<int32> CurrentlyPressedIndicesP1;
    UPROPERTY()
    TArray<int32> CurrentlyPressedIndicesP2;

    int32 GetPlayerForIndex(int32 Index) const;
};
