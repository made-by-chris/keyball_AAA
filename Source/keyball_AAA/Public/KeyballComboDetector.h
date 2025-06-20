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
    Wave,
    Tilt,
    Terrain
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
    TArray<int32> KeysIndex;

    UPROPERTY(BlueprintReadOnly)
    EKeyballDirection Direction = EKeyballDirection::None;

    UPROPERTY(BlueprintReadOnly)
    bool bOverBorder = false;

    FString ToString() const
    {
        FString MoveTypeStr = UEnum::GetValueAsString(MoveType);
        FString DirectionStr = UEnum::GetValueAsString(Direction);
        FString KeysIndexStr = FString::JoinBy(KeysIndex, TEXT(", "), [](int32 Index) { return FString::FromInt(Index); });
        
        return FString::Printf(TEXT("MoveType=%s, Direction=%s, KeysIndex=[%s], bOverBorder=%s"),
            *MoveTypeStr,
            *DirectionStr,
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
    static FKeyballComboResult DetectKeyballCombo(const TArray<int32>& PressedIndices);

private:
    static EKeyballDirection GetDirection(int32 From, int32 To);
    static bool IsSameSide(int32 IndexA, int32 IndexB);
    static bool IsOverBorder(int32 IndexA, int32 IndexB);
};
