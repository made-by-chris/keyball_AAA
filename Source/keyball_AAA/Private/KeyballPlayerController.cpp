#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KeyballComboDetector.h"
#include "KeyballPlayerState.h"
#include "KeyballKeyboard.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"
#include "Internationalization/Text.h"
#include "Net/UnrealNetwork.h"

AKeyballPlayerController::AKeyballPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    Keyboard = nullptr;
    ComboDetector = nullptr;
    doubleTapT = 0.4f;
    
    UnrealKeyLabelToNaturalGlyphMap.Add("UnknownCharCode_246", "ö");
    UnrealKeyLabelToNaturalGlyphMap.Add("Ö", "ö");
    UnrealKeyLabelToNaturalGlyphMap.Add("ä", "ä");
    UnrealKeyLabelToNaturalGlyphMap.Add("Ä", "ä");
    UnrealKeyLabelToNaturalGlyphMap.Add("ü", "ü");
    UnrealKeyLabelToNaturalGlyphMap.Add("Ü", "ü");
    UnrealKeyLabelToNaturalGlyphMap.Add("ß", "ß");
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
    // UnrealKeyLabelToNaturalGlyphMap.Add("ö", ";");
    UnrealKeyLabelToNaturalGlyphMap.Add("ä", "'");
    UnrealKeyLabelToNaturalGlyphMap.Add("ü", "/");
    UnrealKeyLabelToNaturalGlyphMap.Add("ß", "=");
}

void AKeyballPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // for (int i = 0; i < layout.Num(); i++)
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("layout[29] codepoint: %04x"), layout[29][0]));

    // FString layoutasstring2 = "";
    // for (const FString& s : layout)
    // {
    //     layoutasstring2 += s + ", ";
    // }
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAABeginPlay: %s"), *layoutasstring2));

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

void AKeyballPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AKeyballPlayerController, layout);
    DOREPLIFETIME(AKeyballPlayerController, CurrentlyPressedIndicesP1);
    DOREPLIFETIME(AKeyballPlayerController, CurrentlyPressedIndicesP2);
    DOREPLIFETIME(AKeyballPlayerController, leftMagic);
    DOREPLIFETIME(AKeyballPlayerController, rightMagic);
    DOREPLIFETIME(AKeyballPlayerController, LastPressTimeP1);
    DOREPLIFETIME(AKeyballPlayerController, LastPressTimeP2);
    DOREPLIFETIME(AKeyballPlayerController, doubleTapT);
}

bool AKeyballPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
    if (EventType == IE_Pressed)
    {
        if (IsLocalController())
        {
            ServerHandleKeyPress(Key);
        }
    }
    else if (EventType == IE_Released)
    {
        if (IsLocalController())
        {
            ServerHandleKeyRelease(Key);
        }
    }
    return APlayerController::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

// Server RPCs
void AKeyballPlayerController::ServerHandleKeyPress_Implementation(const FKey& PressedKey)
{
    HandleKeyPress_Server(PressedKey);
}
bool AKeyballPlayerController::ServerHandleKeyPress_Validate(const FKey& PressedKey) { return true; }

void AKeyballPlayerController::ServerHandleKeyRelease_Implementation(const FKey& ReleasedKey)
{
    HandleKeyRelease_Server(ReleasedKey);
}
bool AKeyballPlayerController::ServerHandleKeyRelease_Validate(const FKey& ReleasedKey) { return true; }

// Move the logic from OnAnyKeyPressed to this server-side function
void AKeyballPlayerController::HandleKeyPress_Server(const FKey& PressedKey)
{
    int32 Index = GetIndexFromLayoutKey(PressedKey);
    if (Index < 0) return;

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
        }
        if (Keyboard)
        {
            Keyboard->OnKeyPressed(Index, CurrentlyPressedIndicesP2, KeyballCombo, leftMagic, rightMagic, isDoubleTap);
        }
    }
}

// Move the logic from OnAnyKeyReleased to this server-side function
void AKeyballPlayerController::HandleKeyRelease_Server(const FKey& ReleasedKey)
{
    int32 Index = GetIndexFromLayoutKey(ReleasedKey);
    if (Index < 0) return;

    int32 PlayerIndex = GetPlayerForIndex(Index);

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

    for (int i = 0; i < layout.Num(); i++)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
            FString::Printf(TEXT("layout[%d]: %s"), i, *layout[i]));
    }


    FString KeyString = InKey.GetFName().ToString();
    FString AltKeyString = InKey.ToString();

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("FName: %s | ToString: %s"), *KeyString, *AltKeyString));

    const FString* MappedGlyph = UnrealKeyLabelToNaturalGlyphMap.Find(KeyString);
    if (!MappedGlyph)
        MappedGlyph = UnrealKeyLabelToNaturalGlyphMap.Find(AltKeyString);

    if (MappedGlyph)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("FOUND MAPPED GLYPH: %s"), **MappedGlyph));
        return layout.Find(*MappedGlyph);
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NO MAPPED GLYPH: %s"), *KeyString));
    int32 layoutIndex = layout.Find(KeyString); // fallback
    // log the layoutIndex
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("FALLBACK: %d"), layoutIndex));
    return layoutIndex;
}

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

