// KeyballComboDetector.cpp
#include "KeyballComboDetector.h"
#include "Algo/Reverse.h"   // for Algo::Reverse

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

            // Calculate differences between consecutive positions
            int32 DR1 = Row2 - Row1, DC1 = Col2 - Col1;
            int32 DR2 = Row3 - Row2, DC2 = Col3 - Col2;

            // Check if all three keys are in a straight line (including diagonals)
            // They must move in the same direction between each step
            if (DR1 == DR2 && DC1 == DC2 && (FMath::Abs(DR1) <= 1 && FMath::Abs(DC1) <= 1))
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

    // AXIS MOVES
    // build forward axes
    TArray<TArray<int32>> Axes;
    for (int32 Row = 0; Row < 4; ++Row) {
        TArray<int32> RowAxis;
        for (int32 Col = 0; Col < 10; ++Col)
            RowAxis.Add(Row * 10 + Col);
        Axes.Add(RowAxis);
    }
    for (int32 Col = 0; Col < 10; ++Col) {
        TArray<int32> ColAxis;
        for (int32 Row = 0; Row < 4; ++Row)
            ColAxis.Add(Row * 10 + Col);
        Axes.Add(ColAxis);
    }

    // duplicate each axis reversed, so we catch both directions
    TArray<TArray<int32>> AllAxes = Axes;
    for (auto& Axis : Axes) {
        TArray<int32> Rev = Axis;
        Algo::Reverse(Rev);
        AllAxes.Add(MoveTemp(Rev));
    }

    TSet<FString> PressedSet(PressedKeys);

    for (auto& Axis : AllAxes) {
        const int32 N = Axis.Num();

        // —— HORIZONTAL spans are N >= 5 ——  
        if (N >= 5) {
            // Wave: KEY-SKIP-SKIP-KEY-KEY  → offsets [0, +3, +4]
            for (int i = 0; i <= N - 5; ++i) {
                int A = Axis[i], D = Axis[i+3], E = Axis[i+4];
                auto KA = SelectedLayout[A], KD = SelectedLayout[D], KE = SelectedLayout[E];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KD) && PressedSet.Contains(KE)) {
                    Result.MoveType    = EKeyballMoveType::Wave;
                    Result.Keys        = {KA,KD,KE};
                    Result.KeysIndex   = {A,D,E};
                    Result.Direction   = GetDirection(A,E);
                    Result.bOverBorder = IsOverBorder(A,D) || IsOverBorder(D,E);
                    if (!Result.bOverBorder) return Result;
                }
            }
            // Ripple: KEY-KEY-SKIP-SKIP-KEY  → offsets [0, +1, +4]
            for (int i = 0; i <= N - 5; ++i) {
                int A = Axis[i], B = Axis[i+1], E = Axis[i+4];
                auto KA = SelectedLayout[A], KB = SelectedLayout[B], KE = SelectedLayout[E];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KB) && PressedSet.Contains(KE)) {
                    Result.MoveType    = EKeyballMoveType::Ripple;
                    Result.Keys        = {KA,KB,KE};
                    Result.KeysIndex   = {A,B,E};
                    Result.Direction   = GetDirection(A,E);
                    Result.bOverBorder = IsOverBorder(A,B) || IsOverBorder(B,E);
                    if (!Result.bOverBorder) return Result;
                }
            }
            // Tilt: endpoints only → offsets [0, +4]
            for (int i = 0; i <= N - 5; ++i) {
                int A = Axis[i], E = Axis[i+4];
                auto KA = SelectedLayout[A], KE = SelectedLayout[E];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KE)) {
                    Result.MoveType    = EKeyballMoveType::Tilt;
                    Result.Keys        = {KA,KE};
                    Result.KeysIndex   = {A,E};
                    Result.Direction   = GetDirection(A,E);
                    Result.bOverBorder = IsOverBorder(A,E);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }

        // —— VERTICAL spans are N == 4 ——  
        if (N == 4) {
            // Wave: KEY-SKIP-KEY-KEY → offsets [0, +2, +3]
            for (int i = 0; i <= N - 4; ++i) {
                int A = Axis[i], C = Axis[i+2], D = Axis[i+3];
                auto KA = SelectedLayout[A], KC = SelectedLayout[C], KD = SelectedLayout[D];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KC) && PressedSet.Contains(KD)) {
                    Result.MoveType    = EKeyballMoveType::Wave;
                    Result.Keys        = {KA,KC,KD};
                    Result.KeysIndex   = {A,C,D};
                    Result.Direction   = GetDirection(A,D);
                    Result.bOverBorder = IsOverBorder(A,C) || IsOverBorder(C,D);
                    if (!Result.bOverBorder) return Result;
                }
            }
            // Ripple: KEY-KEY-SKIP-KEY → offsets [0, +1, +3]
            for (int i = 0; i <= N - 4; ++i) {
                int A = Axis[i], B = Axis[i+1], D = Axis[i+3];
                auto KA = SelectedLayout[A], KB = SelectedLayout[B], KD = SelectedLayout[D];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KB) && PressedSet.Contains(KD)) {
                    Result.MoveType    = EKeyballMoveType::Ripple;
                    Result.Keys        = {KA,KB,KD};
                    Result.KeysIndex   = {A,B,D};
                    Result.Direction   = GetDirection(A,D);
                    Result.bOverBorder = IsOverBorder(A,B) || IsOverBorder(B,D);
                    if (!Result.bOverBorder) return Result;
                }
            }
            // Tilt: endpoints only → offsets [0, +3]
            for (int i = 0; i <= N - 4; ++i) {
                int A = Axis[i], D = Axis[i+3];
                auto KA = SelectedLayout[A], KD = SelectedLayout[D];
                if (PressedSet.Contains(KA) && PressedSet.Contains(KD)) {
                    Result.MoveType    = EKeyballMoveType::Tilt;
                    Result.Keys        = {KA,KD};
                    Result.KeysIndex   = {A,D};
                    Result.Direction   = GetDirection(A,D);
                    Result.bOverBorder = IsOverBorder(A,D);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }
    }


    // Diagonal combo detection (↘ ↖ ↙ ↗)
    int32 DiagonalDeltas[] = {11, -11, 9, -9};

    for (int32 i = 0; i < 40; ++i)
    {
        FString K0 = SelectedLayout[i];
        if (!PressedSet.Contains(K0)) continue;

        for (int d = 0; d < 4; ++d)
        {
            int32 Delta = DiagonalDeltas[d];
            int32 i1 = i + Delta;
            int32 i2 = i + Delta * 2;

            // Make sure all indices are valid
            if (i1 < 0 || i1 >= 40 || i2 < 0 || i2 >= 40) continue;

            FString K1 = SelectedLayout[i1];
            FString K2 = SelectedLayout[i2];

            if (PressedSet.Contains(K1) && PressedSet.Contains(K2))
            {
                // Check for border jumps (row wraparound)
                if (IsOverBorder(i, i1) || IsOverBorder(i1, i2)) continue;

                Result.MoveType    = EKeyballMoveType::Diagonal;
                Result.Keys        = {K0, K1, K2};
                Result.KeysIndex   = {i, i1, i2};
                Result.Direction   = GetDirection(i, i2);  // You may adjust if you want to distinguish ↗ from ↘
                Result.bOverBorder = false;
                return Result;
            }
        }
    }

    
    // Whack Combo Detection (2-key directional)
    if (PressedKeys.Num() >= 2)
    {
        FString FirstKey = PressedKeys[PressedKeys.Num() - 2];
        FString SecondKey = PressedKeys.Last();

        int32 FirstIndex = FindKeyIndex(SelectedLayout, FirstKey);
        int32 SecondIndex = FindKeyIndex(SelectedLayout, SecondKey);

        if (FirstIndex != INDEX_NONE && SecondIndex != INDEX_NONE)
        {
            // Check if keys are adjacent (including diagonals)
            int32 FirstRow = FirstIndex / 10;
            int32 FirstCol = FirstIndex % 10;
            int32 SecondRow = SecondIndex / 10;
            int32 SecondCol = SecondIndex % 10;

            // Calculate row and column differences
            int32 RowDiff = FMath::Abs(SecondRow - FirstRow);
            int32 ColDiff = FMath::Abs(SecondCol - FirstCol);

            // Keys are adjacent if they differ by at most 1 in both row and column
            bool bIsAdjacent = (RowDiff <= 1 && ColDiff <= 1) && (RowDiff + ColDiff > 0);

            if (bIsAdjacent)
            {
                Result.MoveType = EKeyballMoveType::Whack;
                Result.Keys = { FirstKey, SecondKey };
                Result.KeysIndex = { FirstIndex, SecondIndex };
                Result.Direction = GetDirection(FirstIndex, SecondIndex);
                Result.bOverBorder = IsOverBorder(FirstIndex, SecondIndex);
                if (!Result.bOverBorder) return Result;
            }
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
