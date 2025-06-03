#include "DebugUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UDebugUtils::WriteArrayToFile(const TArray<FString>& MyArray, const FString& Filename)
{
    FString OutputString;
    for (const FString& Item : MyArray)
    {
        OutputString += Item + LINE_TERMINATOR;
    }

    FString FilePath = FPaths::ProjectLogDir() + Filename;
    FFileHelper::SaveStringToFile(OutputString, *FilePath);
}
