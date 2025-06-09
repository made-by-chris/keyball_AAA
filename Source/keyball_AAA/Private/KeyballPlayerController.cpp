#include "KeyballPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void AKeyballPlayerController::ClientKeyPressed(FKey Key)
{
    // Forward to server
    if (HasAuthority())
    {
        ServerHandleKeyPress(Key.ToString());
    }
    else
    {
        ServerHandleKeyPress(Key.ToString()); // will trigger RPC to server
    }
}

void AKeyballPlayerController::ServerHandleKeyPress_Implementation(const FString& PressedKey)
{
    HandleKeyPress(PressedKey);
}

void AKeyballPlayerController::HandleKeyPress(const FString& PressedKey)
{
    bool bIsValid = false;
    bool bIsSpecial = false;
    if (!CheckIsValidKey(PressedKey, bIsValid, bIsSpecial))
        return;

    if (!CheckKeyOwner(PressedKey))
        return;

    if (!CanPlayerPressMoreKeys())
        return;

    TArray<AActor*> TargetActors = KeyStringToActors(PressedKey);
    for (AActor* Actor : TargetActors)
    {
        UE_LOG(LogTemp, Log, TEXT("Acting on key actor: %s"), *Actor->GetName());
        // TODO: trigger animation / event
    }

    if (CheckForTilt(PressedKey))
    {
        UE_LOG(LogTemp, Log, TEXT("Tilt detected for key: %s"), *PressedKey);
    }

    FString MirrorKey = CalculateSymmetricalKey(CurrentlyPressedKeys.Num());
    UE_LOG(LogTemp, Log, TEXT("Mirror key is: %s"), *MirrorKey);
}

// === Helper Logic ===

bool AKeyballPlayerController::CheckIsValidKey(const FString& Key, bool& bIsValid, bool& bIsSpecial) const
{
    bIsSpecial = specialMoveKeys.Contains(Key);

    const bool bInLayout = selectedLayout.Contains(Key);
    const bool bInLabels = unrealKeyLabels.Contains(Key);

    bIsValid = bIsSpecial || bInLayout || bInLabels;
    return bIsValid;
}

bool AKeyballPlayerController::CheckIfOwningPlayerCanPressMoreKeys(int32 Team, int32 Limit) const
{
    int32 ActiveKeyCount = 0;

    switch (Team)
    {
        case 0:
            ActiveKeyCount = team1ActiveKeyCount;
            break;

        case 1:
            ActiveKeyCount = team2ActiveKeyCount;
            break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown team: %d"), Team);
            return false;
    }

    return ActiveKeyCount < Limit;
}

bool AKeyballPlayerController::CheckKeyOwner(const FString& Key)
{
    return true; // stub, replace with ownership logic
}

bool AKeyballPlayerController::CanPlayerPressMoreKeys()
{
    return CurrentlyPressedKeys.Num() < MaxKeysAllowed;
}

bool AKeyballPlayerController::CheckForTilt(const FString& Key)
{
    return false; // stub, implement real tilt logic
}

FString AKeyballPlayerController::CalculateSymmetricalKey(int32 Index)
{
    return FString::Printf(TEXT("Mirror_%d"), Index); // example stub
}

TArray<AActor*> AKeyballPlayerController::KeyStringToActors(const FString& Key)
{
    TArray<AActor*> OutActors;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        if (It->GetName().Contains(Key))
        {
            OutActors.Add(*It);
        }
    }
    return OutActors;
}
