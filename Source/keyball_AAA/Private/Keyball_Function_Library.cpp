#include "Keyball_Function_Library.h"
#include "Misc/Parse.h"
#include "Keyball_Function_Library.h"
#include "KeyboardLoader.h"
#include "KeyboardDataStruct.h"
#include "KeyballKeyboard.h"


void UKeyball_Function_Library::GetKeyCode(FKey Key, int& KeyCode, int& CharacterCode)
{
    const uint32* KeyCodePtr;
    const uint32* CharCodePtr;
    FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);
    
    KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
    CharacterCode = CharCodePtr ? *CharCodePtr : 0;
}

FString UKeyball_Function_Library::FixUnknownCharCode(const FString& CharCode)
{
    // Check if the input starts with "_"
    if (!CharCode.StartsWith(TEXT("_")))
    {
        return FString(TEXT("Invalid input: Expected '_<number>'"));
    }

    // Extract the number part (e.g., "246" from "_246")
    FString NumberPart = CharCode.Mid(1);

    // Convert to integer
    int32 CodePoint;
    if (!NumberPart.IsNumeric())
    {
        return FString(TEXT("Invalid input: Number expected after '_'"));
    }
    LexFromString(CodePoint, *NumberPart);

    // Validate code point (basic check for valid Unicode range)
    if (CodePoint < 0 || CodePoint > 0x10FFFF)
    {
        return FString(TEXT("Invalid Unicode code point"));
    }

    // Convert code point to TCHAR (UTF-16)
    TCHAR Char = static_cast<TCHAR>(CodePoint);
    return FString(1, &Char);
}



TArray<FTransform> CalculateBaseTransforms()
{
    TArray<FTransform> Out;
    for (int32 Row = 0; Row < 4; ++Row)
    {
        for (int32 Col = 0; Col < 10; ++Col)
        {
            FVector Location(Row * -60.f, Col * 60.f, 0.f); // flip Y for screen-space feel
            FRotator Rotation(0.f, 0.f, 0.f);
            FVector Scale(2.f, 2.f, 2.f);
            Out.Add(FTransform(Rotation, Location, Scale));
        }
    }
    return Out;
}

void UKeyball_Function_Library::GetKeysForKeyboard(const TArray<int32>& InKeyboardIDs, TArray<FKeycapSpawnData>& OutKeys)
{
    OutKeys.Empty();

    // Step 1: Pad to 40
    TArray<int32> FixedIDs = InKeyboardIDs;
    if (FixedIDs.Num() < 40 && FixedIDs.Num() > 0)
    {
        while (FixedIDs.Num() < 40)
            FixedIDs.Add(FixedIDs[FixedIDs.Num() % InKeyboardIDs.Num()]);
    }
    else if (FixedIDs.Num() == 0)
    {
        FixedIDs.Init(0, 40);
    }

    // Step 2: Load keyboard configs
    const TArray<FKeyboardDataStruct> OfficialKeyboards = UKeyboardLoader::LoadKeyboardsFromJson();

    // Step 3: Generate base transforms
    const TArray<FTransform> BaseTransforms = CalculateBaseTransforms();

    // Step 4: Build FKeycapSpawnData
    for (int32 i = 0; i < 40; ++i)
    {
        FKeycapSpawnData Data;
        Data.Index = i;
        Data.Transform = BaseTransforms[i];

        // log mesh for this key
        int32 KeyboardID = FixedIDs[i];
        const FKeyboardDataStruct* Config = OfficialKeyboards.IsValidIndex(KeyboardID) ? &OfficialKeyboards[KeyboardID] : nullptr;

        if (!Config || Config->staticMesh.Num() == 0)
        {
            Data.Mesh = nullptr;
            OutKeys.Add(Data);
            continue;
        }

        switch (Config->keyboardType)
        {
        case EKeyboard_Type::Regular40:
            Data.Mesh = Config->staticMesh.IsValidIndex(i) ? Config->staticMesh[i].LoadSynchronous() : nullptr;
            break;

        case EKeyboard_Type::LessThan40:
        {
            int Row = i / 10;
            int Col = i % 10;

            switch (Config->lessThan40Algo)
            {
            case Less_Than_40Algo::Loop:
                Data.Mesh = Config->staticMesh[i % Config->staticMesh.Num()].LoadSynchronous();
                break;

            case Less_Than_40Algo::Halves:
                Data.Mesh = Config->staticMesh[(Col < 5) ? 0 : 1].LoadSynchronous();
                break;

            case Less_Than_40Algo::HalvesRotated:
                Data.Mesh = Config->staticMesh[0].LoadSynchronous();
                if (Col >= 5)
                {
                    FRotator Rot = Data.Transform.GetRotation().Rotator();
                    Rot.Yaw += 180.f;
                    Data.Transform.SetRotation(FQuat(Rot));
                }
                break;

            case Less_Than_40Algo::edgesThenCenter:
            {
                const int idx = [&]()
                {
                    bool Top = Row == 0, Bottom = Row == 3, Left = Col == 0, Right = Col == 9;

                    if (Top && Left) return 0;
                    if (Top && Right) return 1;
                    if (Bottom && Right) return 2;
                    if (Bottom && Left) return 3;
                    if (Top) return 4;
                    if (Right) return 5;
                    if (Bottom) return 6;
                    if (Left) return 7;
                    return 8;
                }();

                Data.Mesh = Config->staticMesh.IsValidIndex(idx) ? Config->staticMesh[idx].LoadSynchronous() : nullptr;
                break;
            }

            default:
                Data.Mesh = Config->staticMesh[0].LoadSynchronous();
                break;
            }
            break;
        }

        default:
            Data.Mesh = nullptr;
            break;
        }

        OutKeys.Add(Data);
    }
}
