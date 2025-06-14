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
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("OnAnyKeyPressed: %s"), *PressedKey.ToString()));
    int32 Index = GetIndexFromFKey(PressedKey);
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("OnAnyKeyPressed: %d"), Index));
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

void AKeyballPlayerController::HandleAnyKeyPressed()
{
    // TODO: Implement or leave empty if not needed
}

void AKeyballPlayerController::HandleAnyKeyReleased()
{
    // TODO: Implement or leave empty if not needed
}

void AKeyballPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // TODO: Bind input if needed
}
