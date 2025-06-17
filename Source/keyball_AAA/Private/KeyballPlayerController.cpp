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
    int32 Index = GetIndexFromFKey(PressedKey);
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

void AKeyballPlayerController::OnAnyKeyReleased(FKey ReleasedKey)
{
    int32 Index = GetIndexFromFKey(ReleasedKey);
    int32 PlayerIndex = GetPlayerForIndex(Index);
    if (Index < 0) return;

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

int32 AKeyballPlayerController::GetPlayerForIndex(int32 Index) const
{
    if (Index < 0 || Index > 39) return -1;
    return (Index % 10 <= 4) ? 1 : 2;
}

void AKeyballPlayerController::updateLayout(const TArray<FString>& NewLayout)
{
    layout = NewLayout;
}
