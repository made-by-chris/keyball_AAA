#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerController.generated.h"

class AKeyballKeyboard;

UCLASS()
class KEYBALL_AAA_API AKeyballPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AKeyballPlayerController();

    UFUNCTION(BlueprintCallable, Category = "KeyPress")
    void ClientKeyPressed(FKey Key);

    UFUNCTION(BlueprintCallable, Category = "KeyPress")
    void ClientKeyReleased(FKey Key);

    UFUNCTION(Server, Reliable)
    void ServerHandleKeyPress(const FKey& PressedKey);
    bool ServerHandleKeyPress_Validate(const FKey& PressedKey);

    UFUNCTION(Server, Reliable)
    void ServerHandleKeyRelease(const FKey& ReleasedKey);
    bool ServerHandleKeyRelease_Validate(const FKey& ReleasedKey);

    // Keyboard reference (set from elsewhere or auto-detected)
    UPROPERTY(BlueprintReadWrite)
    AKeyballKeyboard* Keyboard;

protected:
    void HandleKeyPress(const FKey& PressedKey);
    void HandleKeyRelease(const FKey& ReleasedKey);

    bool CheckIsValidKey(const FKey& Key, bool& bIsValid, bool& bIsSpecial) const;
    bool CheckKeyOwner(const FKey& Key);
    bool CanPlayerPressMoreKeys();

    int32 GetTeamNumber() const;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> CurrentlyPressedKeys;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> selectedLayout;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> unrealKeyLabels;

    UPROPERTY(BlueprintReadWrite)
    TMap<FString, FString> UnrealKeyLabelToNaturalGlyphMap;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> specialMoveKeys = { "Left Shift", "Tab", "Right Shift", "Delete" };

    UPROPERTY(BlueprintReadWrite)
    int32 MaxKeysAllowed = 3;

    UPROPERTY(BlueprintReadWrite)
    bool leftShiftActive;

    UPROPERTY(BlueprintReadWrite)
    bool rightShiftActive;

private:
    FKeyballComboResult KeyballCombo;
};
