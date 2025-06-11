#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerState.h"
#include "KeyballKeyboard.h"
#include "Engine/Engine.h"

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
    if (HasAuthority())
        ServerHandleKeyPress(Key);
    else
        ServerHandleKeyPress(Key);
}

void AKeyballPlayerController::ClientKeyReleased(FKey Key)
{
    if (HasAuthority())
        ServerHandleKeyRelease(Key);
    else
        ServerHandleKeyRelease(Key);
}

bool AKeyballPlayerController::ServerHandleKeyPress_Validate(const FKey& PressedKey) { return true; }
bool AKeyballPlayerController::ServerHandleKeyRelease_Validate(const FKey& ReleasedKey) { return true; }

void AKeyballPlayerController::ServerHandleKeyPress_Implementation(const FKey& PressedKey)
{
    HandleKeyPress(PressedKey);
}

void AKeyballPlayerController::ServerHandleKeyRelease_Implementation(const FKey& ReleasedKey)
{
    HandleKeyRelease(ReleasedKey);
}

void AKeyballPlayerController::HandleKeyPress(const FKey& PressedKey)
{
    FString KeyString = PressedKey.ToString();
    if (FString* Glyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString))
        KeyString = *Glyph;

    CurrentlyPressedKeys.AddUnique(KeyString);

    bool bIsValid = false, bIsSpecial = false;
    CheckIsValidKey(PressedKey, bIsValid, bIsSpecial);

    if (!bIsValid) return;

    // Detect combo
    FKeyballComboResult ComboResult = UKeyballComboDetector::DetectKeyballCombo(selectedLayout, CurrentlyPressedKeys);
    KeyballCombo = ComboResult;

    // Debug log
    if (KeyballCombo.MoveType != EKeyballMoveType::None)
    {
        FString MoveTypeStr = UEnum::GetValueAsString(KeyballCombo.MoveType);
        FString PressedKeysStr = FString::Join(CurrentlyPressedKeys, TEXT(", "));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green,
            FString::Printf(TEXT("Combo detected: %s | Keys: [%s]"), *MoveTypeStr, *PressedKeysStr));
    }

    // === NEW: Pass combo to keyboard ===
   if (Keyboard)
    {
        Keyboard->OnKeyPressed(KeyString, CurrentlyPressedKeys, KeyballCombo);
    }

}

void AKeyballPlayerController::HandleKeyRelease(const FKey& ReleasedKey)
{
    FString KeyString = ReleasedKey.ToString();
    if (FString* Glyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString))
        KeyString = *Glyph;

    CurrentlyPressedKeys.Remove(KeyString);

    // Re-run combo check
    FKeyballComboResult ComboResult = UKeyballComboDetector::DetectKeyballCombo(selectedLayout, CurrentlyPressedKeys);
    KeyballCombo = ComboResult;

    if (ReleasedKey == EKeys::LeftShift || ReleasedKey == EKeys::Tab)
        leftShiftActive = false;
    else if (ReleasedKey == EKeys::RightShift || ReleasedKey == EKeys::Delete)
        rightShiftActive = false;

    // ✅ Notify keyboard to deactivate key and update state
    if (Keyboard)
    {
        Keyboard->OnKeyReleased(KeyString, CurrentlyPressedKeys);
    }
}


bool AKeyballPlayerController::CheckIsValidKey(const FKey& Key, bool& bIsValid, bool& bIsSpecial) const
{
    FString KeyString = Key.ToString();
    bIsSpecial = specialMoveKeys.Contains(KeyString);
    bIsValid = bIsSpecial || selectedLayout.Contains(KeyString) || unrealKeyLabels.Contains(KeyString);
    return bIsValid;
}

bool AKeyballPlayerController::CheckKeyOwner(const FKey& Key) { return true; }

bool AKeyballPlayerController::CanPlayerPressMoreKeys()
{
    return CurrentlyPressedKeys.Num() < MaxKeysAllowed;
}

int32 AKeyballPlayerController::GetTeamNumber() const
{
    if (const auto* PS = Cast<AKeyballPlayerState>(PlayerState))
        return PS->GetTeamNumber();
    return -1;
}
