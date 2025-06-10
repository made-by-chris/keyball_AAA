// KeyballComboDetector.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KeyballComboDetector.generated.h"

UENUM(BlueprintType)
enum class EKeyballMoveType : uint8
{
    None UMETA(DisplayName = "None"),
    Whack,
    Stairs,
    Ripple,
    Wave,
    Tilt,
    Diagonal
};



UENUM(BlueprintType)
enum class EKeyballDirection : uint8
{
    None UMETA(DisplayName = "None"),
    Up,
    Down,
    Left,
    Right,
    UpLeft,
    UpRight,
    DownLeft,
    DownRight
};

USTRUCT(BlueprintType)
struct FKeyballComboResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EKeyballMoveType MoveType = EKeyballMoveType::None;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> Keys;

    UPROPERTY(BlueprintReadOnly)
    TArray<int32> KeysIndex;

    UPROPERTY(BlueprintReadOnly)
    EKeyballDirection Direction = EKeyballDirection::None;

    UPROPERTY(BlueprintReadOnly)
    bool bOverBorder = false;

    FString ToString() const
    {
        FString MoveTypeStr = UEnum::GetValueAsString(MoveType);
        FString DirectionStr = UEnum::GetValueAsString(Direction);
        FString KeysStr = FString::Join(Keys, TEXT(", "));
        FString KeysIndexStr = FString::JoinBy(KeysIndex, TEXT(", "), [](int32 Index) { return FString::FromInt(Index); });
        
        return FString::Printf(TEXT("MoveType=%s, Direction=%s, Keys=[%s], KeysIndex=[%s], bOverBorder=%s"),
            *MoveTypeStr,
            *DirectionStr,
            *KeysStr,
            *KeysIndexStr,
            bOverBorder ? TEXT("true") : TEXT("false"));
    }
};

UCLASS()
class KEYBALL_AAA_API UKeyballComboDetector : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Keyball")
    static FKeyballComboResult DetectKeyballCombo(
        const TArray<FString>& SelectedLayout,
        const TArray<FString>& PressedKeys
    );

private:
    static int32 FindKeyIndex(const TArray<FString>& Layout, const FString& Key);
    static EKeyballDirection GetDirection(int32 From, int32 To);
    static bool IsSameSide(int32 IndexA, int32 IndexB);
    static bool IsOverBorder(int32 IndexA, int32 IndexB);
};
