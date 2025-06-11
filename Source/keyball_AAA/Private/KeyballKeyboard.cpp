// KeyballKeyboard.cpp

#include "KeyballKeyboard.h"
#include "KeyballKey.h"
#include "Engine/World.h"
#include "TimerManager.h"

AKeyballKeyboard::AKeyballKeyboard()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AKeyballKeyboard::BeginPlay()
{
    Super::BeginPlay();

    // Initialize key map from public "Keys" array if set via BP
    for (AKeyballKey* Key : Keys)
    {
        if (Key)
        {
            FString Symbol = Key->GetSymbol();
            RegisterKey(Symbol, Key);
        }
    }
}

void AKeyballKeyboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update any key transforms (animated keys)
    for (auto& Pair : ActiveKeys)
    {
        if (AKeyballKey* Key = Pair.Value)
        {
            Key->UpdateKeyAnimation(DeltaTime);
        }
    }
}

void AKeyballKeyboard::RegisterKey(const FString& Symbol, AKeyballKey* Key)
{
    KeyMap.Add(Symbol, Key);
}

void AKeyballKeyboard::OnKeyPressed(const FString& PressedSymbol, const TArray<FString>& AllPressedKeys, const FKeyballComboResult& Combo)
{
    if (AKeyballKey* Key = KeyMap.FindRef(PressedSymbol))
    {
        Key->StartPressAnimation();
        ActiveKeys.Add(PressedSymbol, Key); // Track for per-frame updates
    }

    if (Combo.MoveType != EKeyballMoveType::None)
    {
        // TODO: Stub - Implement actual combo behavior later
        ApplyComboEffect(Combo);
    }
}

void AKeyballKeyboard::OnKeyReleased(const FString& ReleasedSymbol, const TArray<FString>& RemainingPressedKeys)
{
    if (AKeyballKey* Key = KeyMap.FindRef(ReleasedSymbol))
    {
        Key->StartReleaseAnimation();
        ActiveKeys.Remove(ReleasedSymbol);
    }
}

void AKeyballKeyboard::ApplyComboEffect(const FKeyballComboResult& Combo)
{
    // TODO: Implement combo-driven key animations (whack, wave, ripple, etc.)
}
