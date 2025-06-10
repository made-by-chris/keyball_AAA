#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerState.h"

AKeyballPlayerController::AKeyballPlayerController()
{
    // Initialize the key label to natural glyph map
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("UnknownCharCode_246"), TEXT("ö"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("One"), TEXT("1"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Two"), TEXT("2"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Three"), TEXT("3"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Four"), TEXT("4"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Five"), TEXT("5"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Six"), TEXT("6"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Seven"), TEXT("7"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Eight"), TEXT("8"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Nine"), TEXT("9"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Zero"), TEXT("0"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Period"), TEXT("."));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Semicolon"), TEXT(";"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Slash"), TEXT("/"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Comma"), TEXT(","));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Hyphen"), TEXT("-"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Ampersand"), TEXT("&"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Apostrophe"), TEXT("'"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Asterisk"), TEXT("*"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Backslash"), TEXT("\\"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Caps Lock"), TEXT("Caps Lock"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Backspace"), TEXT("Backspace"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Caret"), TEXT("^"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Colon"), TEXT(":"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Dollar"), TEXT("$"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Down"), TEXT("Down"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("End"), TEXT("End"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Enter"), TEXT("Enter"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Equals"), TEXT("="));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Escape"), TEXT("Escape"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Exclamation"), TEXT("!"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Home"), TEXT("Home"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Insert"), TEXT("Insert"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left"), TEXT("Left"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Alt"), TEXT("Left Alt"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Bracket"), TEXT("["));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Cmd"), TEXT("Left Cmd"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Ctrl"), TEXT("Left Ctrl"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Parantheses"), TEXT("("));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Shift"), TEXT("Left Shift"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num -"), TEXT("Num -"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num *"), TEXT("Num *"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num ."), TEXT("Num ."));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num /"), TEXT("Num /"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num +"), TEXT("Num +"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 0"), TEXT("Num 0"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 1"), TEXT("Num 1"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 2"), TEXT("Num 2"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 3"), TEXT("Num 3"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 4"), TEXT("Num 4"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 5"), TEXT("Num 5"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 6"), TEXT("Num 6"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 7"), TEXT("Num 7"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 8"), TEXT("Num 8"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 9"), TEXT("Num 9"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num Lock"), TEXT("Num Lock"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Page Down"), TEXT("Page Down"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Page Up"), TEXT("Page Up"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Pause"), TEXT("Pause"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Quote"), TEXT("Quote"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right"), TEXT("Right"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Alt"), TEXT("Right Alt"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Bracket"), TEXT("]"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Cmd"), TEXT("Right Cmd"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Ctrl"), TEXT("Right Ctrl"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Parantheses"), TEXT(")"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Shift"), TEXT("Right Shift"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Scroll Lock"), TEXT("Scroll Lock"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Tab"), TEXT("Tab"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Underscore"), TEXT("_"));
    UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Up"), TEXT("Up"));
}

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

bool AKeyballPlayerController::ServerHandleKeyPress_Validate(const FKey& PressedKey)
{
    return true;
}

void AKeyballPlayerController::HandleKeyPress(const FKey& PressedKey)
{
    // here gotta add the new pressed key to the pressed keys array, and check and set the keyball combo
    FString KeyString = PressedKey.ToString();
    // Get natural glyph if it exists in the map
    FString* NaturalGlyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString);
    if (NaturalGlyph)
    {
        KeyString = *NaturalGlyph;
    }

    //on-screen log
    // GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Key pressed: %s"), *KeyString));

    // if (!CanPlayerPressMoreKeys())
    // {
    //     return;
    // }

    // add the new pressed key to the pressed keys array
    CurrentlyPressedKeys.AddUnique(KeyString);
    // if (!CheckKeyOwner(PressedKey)) {
    //     return;
    // }
    bool bIsValid = false;
    bool bIsSpecial = false;
    CheckIsValidKey(PressedKey, bIsValid, bIsSpecial);
    if (bIsSpecial)
    {
    }
    // Handle special keys ("left shift", "right shift", "delete")
    // set leftShiftActive (tab is also left), or rightShiftActive (delete is also "rightShift")
    // if (PressedKey == EKeys::LeftShift || PressedKey == EKeys::Tab)
    // {
    //     leftShiftActive = true;
    //     return;
    // }
    // else if (PressedKey == EKeys::RightShift || PressedKey == EKeys::Delete)
    // {
    //     rightShiftActive = true;
    //     return;
    // }

    // check and set the keyball combo
    FKeyballComboResult ComboResult = UKeyballComboDetector::DetectKeyballCombo(selectedLayout, CurrentlyPressedKeys);
    KeyballCombo = ComboResult;
    // if (KeyballCombo.MoveType != EKeyballMoveType::None) {
        // Print the move type on screen
        FString MoveTypeStr = UEnum::GetValueAsString(KeyballCombo.MoveType);
        FString PressedKeysStr = FString::Join(CurrentlyPressedKeys, TEXT(", "));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Keyball combo detected: %s | Pressed keys: [%s]"), *MoveTypeStr, *PressedKeysStr));
        // and in the regular logs
        UE_LOG(LogTemp, Log, TEXT("Keyball combo detected: %s | Pressed keys: [%s]"), *MoveTypeStr, *PressedKeysStr);
    // }
    // Iterate over all BP_Key actors in the world
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        
        // Ensure it's a BP_Key actor
        if (Actor->GetClass()->GetName().Contains("BP_Key"))
        {
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
                    // Call "Set Key Active(true)" on the actor
                    static FName FuncName(TEXT("Set Key Active"));
                    UFunction* Func = Actor->FindFunction(FuncName);

                    if (Func)
                    {
                        struct FSetKeyActiveParams
                        {
                            bool Active;
                            bool shiftIsActive;
                        };

                        FSetKeyActiveParams Params;
                        Params.Active = true;
                        Params.shiftIsActive = false;
                        Actor->ProcessEvent(Func, &Params);
                    }
                    else
                    {
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
    // FString KeyString = Key.ToString();
    // int32 KeyIndex = selectedLayout.IndexOfByKey(KeyString);
    // if (KeyIndex == INDEX_NONE) return false;

    // const int32 Team = GetTeamNumber();
    // if (Team == 0) // Left side
    //     return (KeyIndex % 10 <= 4);

    // if (Team == 1) // Right side
    //     return (KeyIndex % 10 >= 5);

    // return false;
}

bool AKeyballPlayerController::CanPlayerPressMoreKeys()
{
    return true;
    // return CurrentlyPressedKeys.Num() < MaxKeysAllowed;
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

    return -1;
}

void AKeyballPlayerController::ClientKeyReleased(FKey Key)
{
    if (HasAuthority())
    {
        ServerHandleKeyRelease(Key);
    }
    else
    {
        ServerHandleKeyRelease(Key); // trigger RPC
    }
}

void AKeyballPlayerController::ServerHandleKeyRelease_Implementation(const FKey& ReleasedKey)
{
    HandleKeyRelease(ReleasedKey);
}

bool AKeyballPlayerController::ServerHandleKeyRelease_Validate(const FKey& ReleasedKey)
{
    return true;
}

void AKeyballPlayerController::HandleKeyRelease(const FKey& ReleasedKey)
{
    FString KeyString = ReleasedKey.ToString();

    // Get natural glyph if it exists in the map
    FString* NaturalGlyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString);
    if (NaturalGlyph)
    {
        KeyString = *NaturalGlyph;
    }

    // Remove from pressed keys
    CurrentlyPressedKeys.Remove(KeyString);

    // Update keyball combo
    FKeyballComboResult ComboResult = UKeyballComboDetector::DetectKeyballCombo(selectedLayout, CurrentlyPressedKeys);
    KeyballCombo = ComboResult;


    // Shift toggles
    if (ReleasedKey == EKeys::LeftShift || ReleasedKey == EKeys::Tab)
    {
        leftShiftActive = false;
    }
    else if (ReleasedKey == EKeys::RightShift || ReleasedKey == EKeys::Delete)
    {
        rightShiftActive = false;
    }

    // Find BP_Key actor and deactivate it
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor->GetClass()->GetName().Contains("BP_Key"))
        {
            FName SymbolName("Symbol");
            FProperty* SymbolProp = Actor->GetClass()->FindPropertyByName(SymbolName);

            if (FStrProperty* StrProp = CastField<FStrProperty>(SymbolProp))
            {
                FString Symbol = StrProp->GetPropertyValue_InContainer(Actor);

                if (Symbol.ToLower() == KeyString.ToLower())
                {
                    static FName FuncName("Set Key Active");
                    UFunction* Func = Actor->FindFunction(FuncName);

                    if (Func)
                    {
                        struct FSetKeyActiveParams
                        {
                            bool Active;
                            bool shiftIsActive;
                        };

                        FSetKeyActiveParams Params;
                        Params.Active = false;
                        Params.shiftIsActive = false;
                        Actor->ProcessEvent(Func, &Params);
                    }
                    else
                    {
                    }
                }
            }
        }
    }
}
