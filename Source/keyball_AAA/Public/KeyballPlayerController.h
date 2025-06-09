#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerController.generated.h"

UCLASS()
class KEYBALL_AAA_API AKeyballPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // Called from Blueprint when a key is pressed
    UFUNCTION(BlueprintCallable, Category="KeyPress")
    void ClientKeyPressed(FKey Key);

	UFUNCTION(BlueprintCallable, Category="KeyPress")
	bool CheckIsValidKey(const FKey& Key, bool& bIsValid, bool& bIsSpecial) const;

	UFUNCTION(BlueprintCallable, Category="KeyPress")
	bool CheckKeyOwner(const FKey& Key);

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> specialMoveKeys = { "Left Shift", "Tab", "Right Shift", "Delete" };

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> selectedLayout;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> unrealKeyLabels;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	int32 team1ActiveKeyCount;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	int32 team2ActiveKeyCount;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	bool leftShiftActive;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	bool rightShiftActive;

    // Called on the server — receives input from client
    UFUNCTION(Server, Reliable)
    void ServerHandleKeyPress(const FKey& PressedKey);


    UFUNCTION(BlueprintCallable, Category="KeyPress")
    void ClientKeyReleased(FKey Key);

    UFUNCTION(Server, Reliable)
    void ServerHandleKeyRelease(const FKey& ReleasedKey);

    void HandleKeyRelease(const FKey& ReleasedKey);


protected:
    // The actual keypress logic (runs only on server)
    void HandleKeyPress(const FKey& PressedKey);

    // These are assumed to still be Blueprint-defined
    UPROPERTY(BlueprintReadWrite, Category="KeyPress")
    TArray<FString> CurrentlyPressedKeys;

    UPROPERTY(BlueprintReadWrite, Category="KeyPress")
    int32 MaxKeysAllowed = 3;

private:
    bool CanPlayerPressMoreKeys();
    int32 GetTeamNumber() const;
    TArray<AActor*> KeyStringToActors(const FKey& Key);

    UPROPERTY()
    FKeyballComboResult KeyballCombo;
};
