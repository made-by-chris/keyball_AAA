#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerState.h"

void AKeyballPlayerController::ClientKeyPressed(FKey Key)
{
    // Forward to server
    if (HasAuthority())
    {
        ServerHandleKeyPress(Key);
    }
    else
    {
        ServerHandleKeyPress(Key); // will trigger RPC to server
    }
}

void AKeyballPlayerController::ServerHandleKeyPress_Implementation(const FKey& PressedKey)
{
    HandleKeyPress(PressedKey);
}

void AKeyballPlayerController::HandleKeyPress(const FKey& PressedKey)
{
    // here gotta add the new pressed key to the pressed keys array, and check and set the keyball combo

    if (!CanPlayerPressMoreKeys())
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Can press more keys"));

    // add the new pressed key to the pressed keys array
    CurrentlyPressedKeys.AddUnique(PressedKey.ToString());
    UE_LOG(LogTemp, Log, TEXT("Currently pressed keys: %s"), *FString::Join(CurrentlyPressedKeys, TEXT(", ")));
    UE_LOG(LogTemp, Log, TEXT("Checking key owner"));
    if (!CheckKeyOwner(PressedKey)) {
        UE_LOG(LogTemp, Log, TEXT("Key pressed is not owned by the player"));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Key pressed is owned by the player"));
    bool bIsValid = false;
    bool bIsSpecial = false;
    CheckIsValidKey(PressedKey, bIsValid, bIsSpecial);
    if (bIsSpecial)
    {
        UE_LOG(LogTemp, Log, TEXT("Special key pressed: %s"), *PressedKey.ToString());
    }
    // Handle special keys ("left shift", "right shift", "delete")
    // set leftShiftActive (tab is also left), or rightShiftActive (delete is also "rightShift")
    if (PressedKey == EKeys::LeftShift || PressedKey == EKeys::Tab)
    {
        leftShiftActive = true;
        return;
    }
    else if (PressedKey == EKeys::RightShift || PressedKey == EKeys::Delete)
    {
        rightShiftActive = true;
        return;
    }

    // check and set the keyball combo
    FKeyballComboResult ComboResult = UKeyballComboDetector::DetectKeyballCombo(selectedLayout, CurrentlyPressedKeys);
    UE_LOG(LogTemp, Log, TEXT("Keyball combo detected: %s"), *ComboResult.ToString());

    // if the combo is valid, set the keyball combo
    KeyballCombo = ComboResult;

    // Iterate over all BP_Key actors in the world
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        
        // Ensure it's a BP_Key actor
        if (Actor->GetClass()->GetName().Contains("BP_Key"))
        {
            UE_LOG(LogTemp, Log, TEXT("I'm looping over: %s"), *Actor->GetName());

            // Try to get the "Symbol" property
            FString Symbol;
            FName SymbolName(TEXT("Symbol"));
            FProperty* SymbolProp = Actor->GetClass()->FindPropertyByName(SymbolName);
            
            if (FStrProperty* StrProp = CastField<FStrProperty>(SymbolProp))
            {
                Symbol = StrProp->GetPropertyValue_InContainer(Actor);

                // Compare with pressed keys
                if (CurrentlyPressedKeys.Contains(Symbol))
                {
                    // Call "SetKeyActive(true)" on the actor
                    static FName FuncName(TEXT("SetKeyActive"));
                    UFunction* Func = Actor->FindFunction(FuncName);

                    if (Func)
                    {
                        struct FSetKeyActiveParams
                        {
                            bool Active;
                        };

                        FSetKeyActiveParams Params;
                        Params.Active = true;
                        Actor->ProcessEvent(Func, &Params);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Function SetKeyActive not found on actor: %s"), *Actor->GetName());
                    }
                }
            }
        }
    }

    
}

// === Helper Logic ===

bool AKeyballPlayerController::CheckIsValidKey(const FKey& Key, bool& bIsValid, bool& bIsSpecial) const
{
    FString KeyString = Key.ToString();
    bIsSpecial = specialMoveKeys.Contains(KeyString);

    const bool bInLayout = selectedLayout.Contains(KeyString);
    const bool bInLabels = unrealKeyLabels.Contains(KeyString);

    bIsValid = bIsSpecial || bInLayout || bInLabels;
    return bIsValid;
}

bool AKeyballPlayerController::CheckKeyOwner(const FKey& Key)
{
    return true; // fix this later on when we get into sessions logic
    FString KeyString = Key.ToString();
    int32 KeyIndex = selectedLayout.IndexOfByKey(KeyString);
    if (KeyIndex == INDEX_NONE) return false;

    const int32 Team = GetTeamNumber();
    if (Team == 0) // Left side
        return (KeyIndex % 10 <= 4);

    if (Team == 1) // Right side
        return (KeyIndex % 10 >= 5);

    return false;
}

bool AKeyballPlayerController::CanPlayerPressMoreKeys()
{
    return CurrentlyPressedKeys.Num() < MaxKeysAllowed;
}

TArray<AActor*> AKeyballPlayerController::KeyStringToActors(const FKey& Key)
{
    TArray<AActor*> OutActors;
    FString KeyString = Key.ToString();
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        if (It->GetName().Contains(KeyString))
        {
            OutActors.Add(*It);
        }
    }
    return OutActors;
}

int32 AKeyballPlayerController::GetTeamNumber() const
{
    if (const AKeyballPlayerState* PS = Cast<AKeyballPlayerState>(PlayerState))
    {
        return PS->GetTeamNumber();
    }

    UE_LOG(LogTemp, Warning, TEXT("PlayerState is not a KeyballPlayerState"));
    return -1;
}
