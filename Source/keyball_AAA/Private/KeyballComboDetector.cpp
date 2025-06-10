// KeyballComboDetector.cpp
#include "KeyballComboDetector.h"

FKeyballComboResult UKeyballComboDetector::DetectKeyballCombo(
    const TArray<FString>& SelectedLayout,
    const TArray<FString>& PressedKeys
)
{
    FKeyballComboResult Result;

    // Log the selected layout and pressed keys
    FString LayoutStr = FString::Join(SelectedLayout, TEXT(", "));
    FString PressedKeysStr = FString::Join(PressedKeys, TEXT(", "));
    UE_LOG(LogTemp, Log, TEXT("Selected Layout: [%s]"), *LayoutStr);
    UE_LOG(LogTemp, Log, TEXT("Pressed Keys: [%s]"), *PressedKeysStr);
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Selected Layout: [%s]"), *LayoutStr));
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Pressed Keys: [%s]"), *PressedKeysStr));
    if (SelectedLayout.Num() != 40)
        return Result;

    // Whack Combo Detection (2-key directional)
    if (PressedKeys.Num() >= 2)
    {
        FString FirstKey = PressedKeys[PressedKeys.Num() - 2];
        FString SecondKey = PressedKeys.Last();

        int32 FirstIndex = FindKeyIndex(SelectedLayout, FirstKey);
        int32 SecondIndex = FindKeyIndex(SelectedLayout, SecondKey);

        if (FirstIndex != INDEX_NONE && SecondIndex != INDEX_NONE)
        {
            Result.MoveType = EKeyballMoveType::Whack;
            Result.Keys = { FirstKey, SecondKey };
            Result.KeysIndex = { FirstIndex, SecondIndex };
            Result.Direction = GetDirection(FirstIndex, SecondIndex);
            Result.bOverBorder = IsOverBorder(FirstIndex, SecondIndex);
            if (!Result.bOverBorder) return Result;
        }
    }

    // Stairs Detection (3 keys in a contiguous path)
    if (PressedKeys.Num() >= 3)
    {
        FString K1 = PressedKeys[PressedKeys.Num() - 3];
        FString K2 = PressedKeys[PressedKeys.Num() - 2];
        FString K3 = PressedKeys.Last();

        int32 I1 = FindKeyIndex(SelectedLayout, K1);
        int32 I2 = FindKeyIndex(SelectedLayout, K2);
        int32 I3 = FindKeyIndex(SelectedLayout, K3);

        if (I1 != INDEX_NONE && I2 != INDEX_NONE && I3 != INDEX_NONE)
        {
            int32 Row1 = I1 / 10, Col1 = I1 % 10;
            int32 Row2 = I2 / 10, Col2 = I2 % 10;
            int32 Row3 = I3 / 10, Col3 = I3 % 10;

            int32 DR1 = Row2 - Row1, DC1 = Col2 - Col1;
            int32 DR2 = Row3 - Row2, DC2 = Col3 - Col2;

            if (DR1 == DR2 && DC1 == DC2) // Same direction
            {
                Result.MoveType = EKeyballMoveType::Stairs;
                Result.Keys = { K1, K2, K3 };
                Result.KeysIndex = { I1, I2, I3 };
                Result.Direction = GetDirection(I1, I3);
                Result.bOverBorder = IsOverBorder(I1, I2) || IsOverBorder(I2, I3);
                if (!Result.bOverBorder) return Result;
            }
        }
    }

    // Axis-based moves: Wave, Ripple, Tilt (check each axis)
    TArray<TArray<int32>> Axes;
    for (int32 Row = 0; Row < 4; ++Row) // Rows
    {
        TArray<int32> Axis;
        for (int32 Col = 0; Col < 10; ++Col)
            Axis.Add(Row * 10 + Col);
        Axes.Add(Axis);
    }
    for (int32 Col = 0; Col < 10; ++Col) // Columns
    {
        TArray<int32> Axis;
        for (int32 Row = 0; Row < 4; ++Row)
            Axis.Add(Row * 10 + Col);
        Axes.Add(Axis);
    }

    TSet<FString> PressedSet(PressedKeys);

    for (const TArray<int32>& Axis : Axes)
    {
        // Wave: key - skip - skip - key - key
        if (Axis.Num() >= 5)
        {
            for (int32 i = 0; i <= Axis.Num() - 5; ++i)
            {
                int32 A = Axis[i], D = Axis[i + 3], E = Axis[i + 4];
                FString KA = SelectedLayout[A], KD = SelectedLayout[D], KE = SelectedLayout[E];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KD) && PressedSet.Contains(KE))
                {
                    Result.MoveType = EKeyballMoveType::Wave;
                    Result.Keys = { KA, KD, KE };
                    Result.KeysIndex = { A, D, E };
                    Result.Direction = GetDirection(A, E);
                    Result.bOverBorder = IsOverBorder(A, D) || IsOverBorder(D, E);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }

        // Ripple: key - key - skip - key (or) key - key - skip - skip - key
        if (Axis.Num() >= 4)
        {
            for (int32 i = 0; i <= Axis.Num() - 4; ++i)
            {
                int32 A = Axis[i], B = Axis[i + 1], D = Axis[i + 3];
                FString KA = SelectedLayout[A], KB = SelectedLayout[B], KD = SelectedLayout[D];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KB) && PressedSet.Contains(KD))
                {
                    Result.MoveType = EKeyballMoveType::Ripple;
                    Result.Keys = { KA, KB, KD };
                    Result.KeysIndex = { A, B, D };
                    Result.Direction = GetDirection(A, D);
                    Result.bOverBorder = IsOverBorder(A, B) || IsOverBorder(B, D);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }

        // Tilt: key - key - skip - key
        if (Axis.Num() >= 4)
        {
            for (int32 i = 0; i <= Axis.Num() - 4; ++i)
            {
                int32 A = Axis[i], B = Axis[i + 1], D = Axis[i + 3];
                FString KA = SelectedLayout[A], KB = SelectedLayout[B], KD = SelectedLayout[D];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KB) && PressedSet.Contains(KD))
                {
                    Result.MoveType = EKeyballMoveType::Tilt;
                    Result.Keys = { KA, KB, KD };
                    Result.KeysIndex = { A, B, D };
                    Result.Direction = GetDirection(A, D);
                    Result.bOverBorder = IsOverBorder(A, B) || IsOverBorder(B, D);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }
    }

    // Diagonal/skew combo detection (corner-to-corner)
    TArray<TArray<int32>> SkewCombos = {
        {0, 11}, {11, 0}, // top-left to mid-right
        {29, 38}, {38, 29}, // bottom-right to mid-left
        {9, 18}, {18, 9}, // top-right to mid-left
        {20, 31}, {31, 20} // bottom-left to mid-right
    };
    for (const TArray<int32>& Pair : SkewCombos)
    {
        FString KA = SelectedLayout[Pair[0]], KB = SelectedLayout[Pair[1]];
        if (PressedSet.Contains(KA) && PressedSet.Contains(KB))
        {
            Result.MoveType = EKeyballMoveType::Diagonal;
            Result.Keys = { KA, KB };
            Result.KeysIndex = { Pair[0], Pair[1] };
            Result.Direction = GetDirection(Pair[0], Pair[1]);
            Result.bOverBorder = IsOverBorder(Pair[0], Pair[1]);
            if (!Result.bOverBorder) return Result;
        }
    }

    return FKeyballComboResult(); // Nothing matched
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
