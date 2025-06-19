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
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    Keyboard = nullptr;
    ComboDetector = nullptr;
    doubleTapT = 0.4f;
    layout = {"1","2","3","4","5","6","7","8","9","0","q","w","e","r","t","z","u","i","o","p","a","s","d","f","g","h","j","k","l","ö","y","x","c","v","b","n","m",",",".","-"};

    UnrealKeyLabelToNaturalGlyphMap.Add("One", "1");
    UnrealKeyLabelToNaturalGlyphMap.Add("Two", "2");
    UnrealKeyLabelToNaturalGlyphMap.Add("Three", "3");
    UnrealKeyLabelToNaturalGlyphMap.Add("Four", "4");
    UnrealKeyLabelToNaturalGlyphMap.Add("Five", "5");
    UnrealKeyLabelToNaturalGlyphMap.Add("Six", "6");
    UnrealKeyLabelToNaturalGlyphMap.Add("Seven", "7");
    UnrealKeyLabelToNaturalGlyphMap.Add("Eight", "8");
    UnrealKeyLabelToNaturalGlyphMap.Add("Nine", "9");
    UnrealKeyLabelToNaturalGlyphMap.Add("Zero", "0");
    UnrealKeyLabelToNaturalGlyphMap.Add("Comma", ",");
    UnrealKeyLabelToNaturalGlyphMap.Add("Period", ".");
    UnrealKeyLabelToNaturalGlyphMap.Add("Minus", "-");
    UnrealKeyLabelToNaturalGlyphMap.Add("Semicolon", ";");
    UnrealKeyLabelToNaturalGlyphMap.Add("Apostrophe", "'");
    UnrealKeyLabelToNaturalGlyphMap.Add("Slash", "/");
    UnrealKeyLabelToNaturalGlyphMap.Add("Hyphen", "-");
    UnrealKeyLabelToNaturalGlyphMap.Add("Backslash", "\\");
    UnrealKeyLabelToNaturalGlyphMap.Add("Equal", "=");
    UnrealKeyLabelToNaturalGlyphMap.Add("Plus", "+");
    UnrealKeyLabelToNaturalGlyphMap.Add("LeftBracket", "[");
    UnrealKeyLabelToNaturalGlyphMap.Add("RightBracket", "]");
    UnrealKeyLabelToNaturalGlyphMap.Add("LeftBrace", "{");
    UnrealKeyLabelToNaturalGlyphMap.Add("RightBrace", "}");
    // UnrealKeyLabelToNaturalGlyphMap.Add("ö", ";");
    // UnrealKeyLabelToNaturalGlyphMap.Add("Ö", ";");
    UnrealKeyLabelToNaturalGlyphMap.Add("ä", "'");
    UnrealKeyLabelToNaturalGlyphMap.Add("ü", "/");
    UnrealKeyLabelToNaturalGlyphMap.Add("ß", "=");
}

void AKeyballPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FString layoutasstring2 = "";
    for (const FString& s : layout)
    {
        layoutasstring2 += s + ", ";
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAABeginPlay: %s"), *layoutasstring2));

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

bool AKeyballPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
    if (EventType == IE_Pressed)
    {
        OnAnyKeyPressed(Key);
    }
    else if (EventType == IE_Released)
    {
        OnAnyKeyReleased(Key);
    }

    return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AKeyballPlayerController::OnAnyKeyPressed(FKey PressedKey)
{
    int32 Index = GetIndexFromLayoutKey(PressedKey);
    if (Index < 0) return;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAOnAnyKeyPressed: %s, %d"), *PressedKey.ToString(), Index));

    // Magic key logic
    if (PressedKey == EKeys::LeftShift)
        leftMagic = true;
    if (PressedKey == EKeys::RightShift || PressedKey == EKeys::Delete)
        rightMagic = true;

    int32 PlayerIndex = GetPlayerForIndex(Index);
    float CurrentTime = GetWorld()->GetTimeSeconds();
    bool isDoubleTap = false;

    if (PlayerIndex == 1)
    {
        if (CurrentlyPressedIndicesP1.Contains(Index)) return;
        float* LastTime = LastPressTimeP1.Find(Index);
        if (LastTime && (CurrentTime - *LastTime) < doubleTapT)
            isDoubleTap = true;
        LastPressTimeP1.Add(Index, CurrentTime);
        CurrentlyPressedIndicesP1.Add(Index);
        FKeyballComboResult KeyballCombo;
        if (ComboDetector)
        {
            KeyballCombo = ComboDetector->DetectKeyballCombo(CurrentlyPressedIndicesP1);
        }
        if (Keyboard)
        {
            Keyboard->OnKeyPressed(Index, CurrentlyPressedIndicesP1, KeyballCombo, leftMagic, rightMagic, isDoubleTap);
        }
    }
    else if (PlayerIndex == 2)
    {
        if (CurrentlyPressedIndicesP2.Contains(Index)) return;
        float* LastTime = LastPressTimeP2.Find(Index);
        if (LastTime && (CurrentTime - *LastTime) < doubleTapT)
            isDoubleTap = true;
        LastPressTimeP2.Add(Index, CurrentTime);
        CurrentlyPressedIndicesP2.Add(Index);
        FKeyballComboResult KeyballCombo;
        if (ComboDetector)
        {
            KeyballCombo = ComboDetector->DetectKeyballCombo(CurrentlyPressedIndicesP2);
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Combo type: %s"), *UEnum::GetValueAsString(KeyballCombo.MoveType)));
        }
        if (Keyboard)
        {
            Keyboard->OnKeyPressed(Index, CurrentlyPressedIndicesP2, KeyballCombo, leftMagic, rightMagic, isDoubleTap);
        }
    }
}

void AKeyballPlayerController::OnAnyKeyReleased(FKey ReleasedKey)
{
    int32 Index = GetIndexFromLayoutKey(ReleasedKey);
    if (Index < 0) return;

    int32 PlayerIndex = GetPlayerForIndex(Index);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAOnAnyKeyReleased: %s, %d"), *ReleasedKey.ToString(), Index));

    // Magic key logic
    if (ReleasedKey == EKeys::LeftShift)
        leftMagic = false;
    if (ReleasedKey == EKeys::RightShift || ReleasedKey == EKeys::Delete)
        rightMagic = false;

    if (PlayerIndex == 1)
    {
        CurrentlyPressedIndicesP1.Remove(Index);
        if (Keyboard)
        {
            Keyboard->OnKeyReleased(Index, CurrentlyPressedIndicesP1);
        }
    }
    else if (PlayerIndex == 2)
    {
        CurrentlyPressedIndicesP2.Remove(Index);
        if (Keyboard)
        {
            Keyboard->OnKeyReleased(Index, CurrentlyPressedIndicesP2);
        }
    }
}

int32 AKeyballPlayerController::GetPlayerForIndex(int32 Index) const
{
    if (Index < 0 || Index > 39) return -1;
    return (Index % 10 <= 4) ? 1 : 2;
}

int32 AKeyballPlayerController::GetIndexFromLayoutKey(const FKey& InKey) const
{
    // log the name (not toString) of the key
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAGetIndexFromLayoutKey: %s"), *InKey.GetFName()));

    FString KeyString = InKey.ToString();
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAGetIndexFromLayoutKey: %s"), *KeyString));
    const FString* MappedGlyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString);
    if (MappedGlyph)
    {
        return layout.Find(*MappedGlyph);
    }
    return layout.Find(KeyString);
}

//unused
void AKeyballPlayerController::updateLayout(const TArray<FString>& NewLayout)
{
    FString layoutasstring = "";
    for (const FString& s : NewLayout)
    {
        layoutasstring += s + ", ";
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAUpdating layout to: %s"), *layoutasstring));
    layout = NewLayout;
}
