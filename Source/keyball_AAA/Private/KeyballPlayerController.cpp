#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerState.h"
#include "KeyballKeyboard.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

// AKeyballPlayerController::AKeyballPlayerController()
// {
//     // Initialize the key label to natural glyph map
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("UnknownCharCode_246"), TEXT("ö"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("One"), TEXT("1"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Two"), TEXT("2"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Three"), TEXT("3"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Four"), TEXT("4"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Five"), TEXT("5"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Six"), TEXT("6"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Seven"), TEXT("7"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Eight"), TEXT("8"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Nine"), TEXT("9"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Zero"), TEXT("0"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Period"), TEXT("."));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Semicolon"), TEXT(";"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Slash"), TEXT("/"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Comma"), TEXT(","));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Hyphen"), TEXT("-"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Ampersand"), TEXT("&"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Apostrophe"), TEXT("'"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Asterisk"), TEXT("*"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Backslash"), TEXT("\\"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Caps Lock"), TEXT("Caps Lock"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Backspace"), TEXT("Backspace"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Caret"), TEXT("^"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Colon"), TEXT(":"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Dollar"), TEXT("$"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Down"), TEXT("Down"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("End"), TEXT("End"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Enter"), TEXT("Enter"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Equals"), TEXT("="));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Escape"), TEXT("Escape"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Exclamation"), TEXT("!"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Home"), TEXT("Home"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Insert"), TEXT("Insert"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left"), TEXT("Left"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Alt"), TEXT("Left Alt"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Bracket"), TEXT("["));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Cmd"), TEXT("Left Cmd"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Ctrl"), TEXT("Left Ctrl"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Parantheses"), TEXT("("));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Left Shift"), TEXT("Left Shift"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num -"), TEXT("Num -"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num *"), TEXT("Num *"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num ."), TEXT("Num ."));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num /"), TEXT("Num /"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num +"), TEXT("Num +"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 0"), TEXT("Num 0"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 1"), TEXT("Num 1"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 2"), TEXT("Num 2"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 3"), TEXT("Num 3"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 4"), TEXT("Num 4"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 5"), TEXT("Num 5"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 6"), TEXT("Num 6"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 7"), TEXT("Num 7"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 8"), TEXT("Num 8"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num 9"), TEXT("Num 9"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Num Lock"), TEXT("Num Lock"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Page Down"), TEXT("Page Down"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Page Up"), TEXT("Page Up"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Pause"), TEXT("Pause"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Quote"), TEXT("Quote"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right"), TEXT("Right"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Alt"), TEXT("Right Alt"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Bracket"), TEXT("]"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Cmd"), TEXT("Right Cmd"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Ctrl"), TEXT("Right Ctrl"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Parantheses"), TEXT(")"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Right Shift"), TEXT("Right Shift"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Scroll Lock"), TEXT("Scroll Lock"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Tab"), TEXT("Tab"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Underscore"), TEXT("_"));
//     UnrealKeyLabelToNaturalGlyphMap.Add(TEXT("Up"), TEXT("Up"));
// }




AKeyballPlayerController::AKeyballPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    Keyboard = nullptr;
    ComboDetector = nullptr;
}

void AKeyballPlayerController::BeginPlay()
{
    Super::BeginPlay();

    for (TActorIterator<AKeyballKeyboard> It(GetWorld()); It; ++It)
    {
        Keyboard = *It;
        break;
    }

    if (Keyboard)
    {
        ComboDetector = NewObject<UKeyballComboDetector>(this);
    }
}

void AKeyballPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AKeyballPlayerController::OnAnyKeyPressed(FKey PressedKey)
{
    int32 Index = GetIndexFromFKey(PressedKey);
    if (Index < 0 || CurrentlyPressedIndices.Contains(Index)) return;

    CurrentlyPressedIndices.Add(Index);

    FKeyballComboResult KeyballCombo;
    if (ComboDetector)
    {
        KeyballCombo = ComboDetector->DetectKeyballCombo(CurrentlyPressedIndices);
    }

    if (Keyboard)
    {
        Keyboard->OnKeyPressed(Index, CurrentlyPressedIndices, KeyballCombo);
    }
}

void AKeyballPlayerController::OnAnyKeyReleased(FKey ReleasedKey)
{
    int32 Index = GetIndexFromFKey(ReleasedKey);
    if (Index < 0 || !CurrentlyPressedIndices.Contains(Index)) return;

    CurrentlyPressedIndices.Remove(Index);

    if (Keyboard)
    {
        Keyboard->OnKeyReleased(Index, CurrentlyPressedIndices);
    }
}

int32 AKeyballPlayerController::GetIndexFromFKey(const FKey& InKey) const
{
    // This is a placeholder. You need to define a proper mapping between FKey and your 0-39 index.
    static TMap<FKey, int32> KeyToIndexMap = {
        { EKeys::One, 0 }, { EKeys::Two, 1 }, { EKeys::Three, 2 }, { EKeys::Four, 3 }, { EKeys::Five, 4 },
        { EKeys::Six, 5 }, { EKeys::Seven, 6 }, { EKeys::Eight, 7 }, { EKeys::Nine, 8 }, { EKeys::Zero, 9 },
        { EKeys::Q, 10 }, { EKeys::W, 11 }, { EKeys::E, 12 }, { EKeys::R, 13 }, { EKeys::T, 14 },
        { EKeys::Y, 15 }, { EKeys::U, 16 }, { EKeys::I, 17 }, { EKeys::O, 18 }, { EKeys::P, 19 },
        { EKeys::A, 20 }, { EKeys::S, 21 }, { EKeys::D, 22 }, { EKeys::F, 23 }, { EKeys::G, 24 },
        { EKeys::H, 25 }, { EKeys::J, 26 }, { EKeys::K, 27 }, { EKeys::L, 28 }, { EKeys::Semicolon, 29 },
        { EKeys::Z, 30 }, { EKeys::X, 31 }, { EKeys::C, 32 }, { EKeys::V, 33 }, { EKeys::B, 34 },
        { EKeys::N, 35 }, { EKeys::M, 36 }, { EKeys::Comma, 37 }, { EKeys::Period, 38 }, { EKeys::Slash, 39 }
    };

    const int32* FoundIndex = KeyToIndexMap.Find(InKey);
    return FoundIndex ? *FoundIndex : -1;
}
