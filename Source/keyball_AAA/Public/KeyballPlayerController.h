#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
	bool CheckIsValidKey(const FString& Key, bool& bIsValid, bool& bIsSpecial) const;

	UFUNCTION(BlueprintCallable, Category="KeyPress")
	bool CheckIfOwningPlayerCanPressMoreKeys(int32 Team, int32 Limit) const;

	UFUNCTION(BlueprintCallable, Category="KeyPress")
	bool CheckKeyOwner(const FString& Key);

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> specialMoveKeys;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> selectedLayout;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	TArray<FString> unrealKeyLabels;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	int32 team1ActiveKeyCount;

	UPROPERTY(BlueprintReadWrite, Category="KeyPress")
	int32 team2ActiveKeyCount;


    // Called on the server — receives input from client
    UFUNCTION(Server, Reliable)
    void ServerHandleKeyPress(const FString& PressedKey);

protected:
    // The actual keypress logic (runs only on server)
    void HandleKeyPress(const FString& PressedKey);

    // These are assumed to still be Blueprint-defined
    UPROPERTY(BlueprintReadWrite, Category="KeyPress")
    TArray<FString> CurrentlyPressedKeys;

    UPROPERTY(BlueprintReadWrite, Category="KeyPress")
    int32 MaxKeysAllowed;

private:
    bool CanPlayerPressMoreKeys();
    bool CheckForTilt(const FString& Key);
    FString CalculateSymmetricalKey(int32 Index);
    TArray<AActor*> KeyStringToActors(const FString& Key);
};
