#include "Keyball_Function_Library.h"
#include "Misc/Parse.h"

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