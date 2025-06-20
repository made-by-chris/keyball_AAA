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

    void OnAnyKeyPressed(FKey PressedKey);     // Your original method
    void OnAnyKeyReleased(FKey ReleasedKey);   // Your original method

public:
    AKeyballPlayerController();

    UFUNCTION(BlueprintCallable)
    void updateLayout(const TArray<FString>& NewLayout);

    // Server RPCs for key press/release
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerHandleKeyPress(const FKey& PressedKey);
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerHandleKeyRelease(const FKey& ReleasedKey);

    // Replication setup
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
    TArray<FString> layout;

protected:
    virtual void BeginPlay() override;

    int32 GetIndexFromLayoutKey(const FKey& InKey) const;

    virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

    // Add these declarations for server-side input handling
    void HandleKeyPress_Server(const FKey& PressedKey);
    void HandleKeyRelease_Server(const FKey& ReleasedKey);

private:
    AKeyballKeyboard* Keyboard;
    UKeyballComboDetector* ComboDetector;

    UPROPERTY(Replicated)
    TArray<int32> CurrentlyPressedIndicesP1;
    UPROPERTY(Replicated)
    TArray<int32> CurrentlyPressedIndicesP2;

    UPROPERTY(Replicated)
    bool leftMagic = false;
    UPROPERTY(Replicated)
    bool rightMagic = false;
    UPROPERTY(Replicated)
    TMap<int32, float> LastPressTimeP1;
    UPROPERTY(Replicated)
    TMap<int32, float> LastPressTimeP2;
    UPROPERTY(Replicated)
    float doubleTapT = 0.4f;

    int32 GetPlayerForIndex(int32 Index) const;
    
    TMap<FString, FString> UnrealKeyLabelToNaturalGlyphMap;
};
