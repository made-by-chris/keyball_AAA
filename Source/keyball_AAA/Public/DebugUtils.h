#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugUtils.generated.h"

UCLASS()
class KEYBALL_AAA_API UDebugUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void WriteArrayToFile(const TArray<FString>& MyArray, const FString& Filename);
};
