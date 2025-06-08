// KeyballComboDetector.cpp
#include "KeyballComboDetector.h"

FKeyballComboResult UKeyballComboDetector::DetectKeyballCombo(
    const TArray<FString>& SelectedLayout,
    const TArray<FString>& PressedKeys
)
{
    FKeyballComboResult Result;

    if (PressedKeys.Num() < 2 || SelectedLayout.Num() != 40)
        return Result; // Not enough keys or invalid layout

    FString FirstKey = PressedKeys[PressedKeys.Num() - 2];
    FString SecondKey = PressedKeys.Last();

    int32 FirstIndex = FindKeyIndex(SelectedLayout, FirstKey);
    int32 SecondIndex = FindKeyIndex(SelectedLayout, SecondKey);

    if (FirstIndex == INDEX_NONE || SecondIndex == INDEX_NONE)
        return Result; // Invalid keys

    if (!IsSameSide(FirstIndex, SecondIndex))
    {
        Result.bOverBorder = true;
        Result.MoveType = EKeyballMoveType::Whack;
        Result.Keys = { FirstKey, SecondKey };
        Result.KeysIndex = { FirstIndex, SecondIndex };
        Result.Direction = GetDirection(FirstIndex, SecondIndex);
        return Result;
    }

    // Whack logic (2-key directional)
    Result.MoveType = EKeyballMoveType::Whack;
    Result.Keys = { FirstKey, SecondKey };
    Result.KeysIndex = { FirstIndex, SecondIndex };
    Result.Direction = GetDirection(FirstIndex, SecondIndex);

    return Result;
}

int32 UKeyballComboDetector::FindKeyIndex(const TArray<FString>& Layout, const FString& Key)
{
    return Layout.IndexOfByKey(Key);
}

EKeyballDirection UKeyballComboDetector::GetDirection(int32 From, int32 To)
{
    int32 FromRow = From / 10;
    int32 FromCol = From % 10;
    int32 ToRow = To / 10;
    int32 ToCol = To % 10;

    int32 RowDiff = ToRow - FromRow;
    int32 ColDiff = ToCol - FromCol;

    if (RowDiff == -1 && ColDiff == 0) return EKeyballDirection::Up;
    if (RowDiff == 1 && ColDiff == 0) return EKeyballDirection::Down;
    if (RowDiff == 0 && ColDiff == -1) return EKeyballDirection::Left;
    if (RowDiff == 0 && ColDiff == 1) return EKeyballDirection::Right;
    if (RowDiff == -1 && ColDiff == -1) return EKeyballDirection::UpLeft;
    if (RowDiff == -1 && ColDiff == 1) return EKeyballDirection::UpRight;
    if (RowDiff == 1 && ColDiff == -1) return EKeyballDirection::DownLeft;
    if (RowDiff == 1 && ColDiff == 1) return EKeyballDirection::DownRight;

    return EKeyballDirection::None;
}

bool UKeyballComboDetector::IsSameSide(int32 IndexA, int32 IndexB)
{
    bool ALeft = IndexA % 10 <= 4;
    bool BLeft = IndexB % 10 <= 4;
    return ALeft == BLeft;
}

bool UKeyballComboDetector::IsOverBorder(int32 IndexA, int32 IndexB)
{
    return !IsSameSide(IndexA, IndexB);
}
