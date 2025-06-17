// KeyballComboDetector.cpp

#include "KeyballComboDetector.h"
#include "Algo/Reverse.h"

FKeyballComboResult UKeyballComboDetector::DetectKeyballCombo(const TArray<int32>& PressedIndices)
{
    // log the pressed indices
    FString PressedIndicesString;
    for (int32 Index : PressedIndices)
    {
        PressedIndicesString += FString::Printf(TEXT("%d "), Index);
    }
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, PressedIndicesString);

    FKeyballComboResult Result;

    // Stairs Detection (3 keys in a contiguous path)
    if (PressedIndices.Num() >= 3)
    {
        int32 I1 = PressedIndices[PressedIndices.Num() - 3];
        int32 I2 = PressedIndices[PressedIndices.Num() - 2];
        int32 I3 = PressedIndices.Last();

        int32 Row1 = I1 / 10, Col1 = I1 % 10;
        int32 Row2 = I2 / 10, Col2 = I2 % 10;
        int32 Row3 = I3 / 10, Col3 = I3 % 10;

        int32 DR1 = Row2 - Row1, DC1 = Col2 - Col1;
        int32 DR2 = Row3 - Row2, DC2 = Col3 - Col2;

        if (DR1 == DR2 && DC1 == DC2 && (FMath::Abs(DR1) <= 1 && FMath::Abs(DC1) <= 1))
        {
            Result.MoveType = EKeyballMoveType::Stairs;
            Result.KeysIndex = { I1, I2, I3 };
            Result.Direction = GetDirection(I1, I3);
            Result.bOverBorder = IsOverBorder(I1, I2) || IsOverBorder(I2, I3);
            if (!Result.bOverBorder) return Result;
        }
    }

    // Axis detection
    TArray<TArray<int32>> Axes;
    for (int32 Row = 0; Row < 4; ++Row)
    {
        TArray<int32> RowAxis;
        for (int32 Col = 0; Col < 10; ++Col)
            RowAxis.Add(Row * 10 + Col);
        Axes.Add(RowAxis);
    }
    for (int32 Col = 0; Col < 10; ++Col)
    {
        TArray<int32> ColAxis;
        for (int32 Row = 0; Row < 4; ++Row)
            ColAxis.Add(Row * 10 + Col);
        Axes.Add(ColAxis);
    }

    TArray<TArray<int32>> AllAxes = Axes;
    for (auto& Axis : Axes)
    {
        TArray<int32> Rev = Axis;
        Algo::Reverse(Rev);
        AllAxes.Add(Rev);
    }

    TArray<int32> PressedSet(PressedIndices);

    for (const auto& Axis : AllAxes)
    {
        int32 N = Axis.Num();

        if (N >= 5)
        {
            for (int i = 0; i <= N - 5; ++i)
            {
                int A = Axis[i], D = Axis[i+3], E = Axis[i+4];
                if (PressedSet.Contains(A) && PressedSet.Contains(D) && PressedSet.Contains(E))
                {
                    Result.MoveType = EKeyballMoveType::Wave;
                    Result.KeysIndex = {A, D, E};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexE = PressedIndices.Find(E);
                    if (IndexA != INDEX_NONE && IndexE != INDEX_NONE && IndexA > IndexE)
                        Result.Direction = GetDirection(E, A);
                    else
                        Result.Direction = GetDirection(A, E);
                    Result.bOverBorder = IsOverBorder(A, D) || IsOverBorder(D, E);
                    if (!Result.bOverBorder) return Result;
                }
            }

            for (int i = 0; i <= N - 5; ++i)
            {
                int A = Axis[i], B = Axis[i+1], E = Axis[i+4];
                if (PressedSet.Contains(A) && PressedSet.Contains(B) && PressedSet.Contains(E))
                {
                    Result.MoveType = EKeyballMoveType::Ripple;
                    Result.KeysIndex = {A, B, E};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexE = PressedIndices.Find(E);
                    if (IndexA != INDEX_NONE && IndexE != INDEX_NONE && IndexA > IndexE)
                        Result.Direction = GetDirection(E, A);
                    else
                        Result.Direction = GetDirection(A, E);
                    Result.bOverBorder = IsOverBorder(A, B) || IsOverBorder(B, E);
                    if (!Result.bOverBorder) return Result;
                }
            }

            for (int i = 0; i <= N - 5; ++i)
            {
                int A = Axis[i], E = Axis[i+4];
                if (PressedSet.Contains(A) && PressedSet.Contains(E))
                {
                    Result.MoveType = EKeyballMoveType::Tilt;
                    Result.KeysIndex = {A, E};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexE = PressedIndices.Find(E);
                    if (IndexA != INDEX_NONE && IndexE != INDEX_NONE && IndexA > IndexE)
                        Result.Direction = GetDirection(E, A);
                    else
                        Result.Direction = GetDirection(A, E);
                    Result.bOverBorder = IsOverBorder(A, E);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }

        if (N == 4)
        {
            for (int i = 0; i <= N - 4; ++i)
            {
                int A = Axis[i], C = Axis[i+2], D = Axis[i+3];
                if (PressedSet.Contains(A) && PressedSet.Contains(C) && PressedSet.Contains(D))
                {
                    Result.MoveType = EKeyballMoveType::Wave;
                    Result.KeysIndex = {A, C, D};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexD = PressedIndices.Find(D);
                    if (IndexA != INDEX_NONE && IndexD != INDEX_NONE && IndexA > IndexD)
                        Result.Direction = GetDirection(D, A);
                    else
                        Result.Direction = GetDirection(A, D);
                    Result.bOverBorder = IsOverBorder(A, C) || IsOverBorder(C, D);
                    if (!Result.bOverBorder) return Result;
                }
            }

            for (int i = 0; i <= N - 4; ++i)
            {
                int A = Axis[i], B = Axis[i+1], D = Axis[i+3];
                if (PressedSet.Contains(A) && PressedSet.Contains(B) && PressedSet.Contains(D))
                {
                    Result.MoveType = EKeyballMoveType::Ripple;
                    Result.KeysIndex = {A, B, D};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexD = PressedIndices.Find(D);
                    if (IndexA != INDEX_NONE && IndexD != INDEX_NONE && IndexA > IndexD)
                        Result.Direction = GetDirection(D, A);
                    else
                        Result.Direction = GetDirection(A, D);
                    Result.bOverBorder = IsOverBorder(A, B) || IsOverBorder(B, D);
                    if (!Result.bOverBorder) return Result;
                }
            }

            for (int i = 0; i <= N - 4; ++i)
            {
                int A = Axis[i], D = Axis[i+3];
                if (PressedSet.Contains(A) && PressedSet.Contains(D))
                {
                    Result.MoveType = EKeyballMoveType::Tilt;
                    Result.KeysIndex = {A, D};
                    int32 IndexA = PressedIndices.Find(A);
                    int32 IndexD = PressedIndices.Find(D);
                    if (IndexA != INDEX_NONE && IndexD != INDEX_NONE && IndexA > IndexD)
                        Result.Direction = GetDirection(D, A);
                    else
                        Result.Direction = GetDirection(A, D);
                    Result.bOverBorder = IsOverBorder(A, D);
                    if (!Result.bOverBorder) return Result;
                }
            }
        }
    }

    // Diagonal combos
    int32 Deltas[] = {11, -11, 9, -9};

    for (int32 i : PressedIndices)
    {
        for (int Delta : Deltas)
        {
            int32 i1 = i + Delta;
            int32 i2 = i + 2 * Delta;
            if (i1 < 0 || i1 >= 40 || i2 < 0 || i2 >= 40) continue;
            if (PressedSet.Contains(i1) && PressedSet.Contains(i2))
            {
                if (IsOverBorder(i, i1) || IsOverBorder(i1, i2)) continue;
                Result.MoveType = EKeyballMoveType::Diagonal;
                Result.KeysIndex = {i, i1, i2};
                int32 IndexI = PressedIndices.Find(i);
                int32 IndexI2 = PressedIndices.Find(i2);
                if (IndexI != INDEX_NONE && IndexI2 != INDEX_NONE && IndexI > IndexI2)
                    Result.Direction = GetDirection(i2, i);
                else
                    Result.Direction = GetDirection(i, i2);
                Result.bOverBorder = false;
                return Result;
            }
        }
    }

    // Whack
    if (PressedIndices.Num() >= 2)
    {
        int32 A = PressedIndices[PressedIndices.Num() - 2];
        int32 B = PressedIndices.Last();

        int32 RowA = A / 10, ColA = A % 10;
        int32 RowB = B / 10, ColB = B % 10;

        int32 RowDiff = FMath::Abs(RowA - RowB);
        int32 ColDiff = FMath::Abs(ColA - ColB);
        bool bAdjacent = RowDiff <= 1 && ColDiff <= 1 && (RowDiff + ColDiff > 0);

        if (bAdjacent)
        {
            Result.MoveType = EKeyballMoveType::Whack;
            Result.KeysIndex = {A, B};
            Result.Direction = GetDirection(A, B);
            Result.bOverBorder = IsOverBorder(A, B);
            if (!Result.bOverBorder) return Result;
        }
    }

    return FKeyballComboResult(); // fallback: no combo
}

EKeyballDirection UKeyballComboDetector::GetDirection(int32 From, int32 To)
{

    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("XXXXXX From: %d, To: %d"), From, To));


    int32 RowFrom = From / 10, ColFrom = From % 10;
    int32 RowTo = To / 10, ColTo = To % 10;
    int32 RowDiff = RowTo - RowFrom;
    int32 ColDiff = ColTo - ColFrom;



    if (RowFrom == RowTo)
    {
        if (ColTo > ColFrom) return EKeyballDirection::Right;
        if (ColTo < ColFrom) return EKeyballDirection::Left;
    }
    else if (ColFrom == ColTo)
    {
        if (RowTo > RowFrom) return EKeyballDirection::Down;
        if (RowTo < RowFrom) return EKeyballDirection::Up;
    }
    else if (FMath::Abs(RowDiff) == FMath::Abs(ColDiff))
    {
        if (RowDiff < 0 && ColDiff > 0) return EKeyballDirection::UpRight;
        if (RowDiff < 0 && ColDiff < 0) return EKeyballDirection::UpLeft;
        if (RowDiff > 0 && ColDiff > 0) return EKeyballDirection::DownRight;
        if (RowDiff > 0 && ColDiff < 0) return EKeyballDirection::DownLeft;
    }

    return EKeyballDirection::None;
}

bool UKeyballComboDetector::IsSameSide(int32 A, int32 B)
{
    return (A % 10 <= 4) == (B % 10 <= 4);
}

bool UKeyballComboDetector::IsOverBorder(int32 A, int32 B)
{
    return !IsSameSide(A, B);
}
