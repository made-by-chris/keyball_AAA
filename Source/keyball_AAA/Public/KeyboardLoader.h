#pragma once

#include "CoreMinimal.h"
#include "KeyboardDataStruct.h"
#include "KeyboardLoader.generated.h"

UCLASS()
class KEYBALL_AAA_API UKeyboardLoader : public UObject
{
    GENERATED_BODY()

public:

    /** Loads all keyboards from keyboards.json */
    UFUNCTION(BlueprintCallable, Category = "Keyboard", meta = (DisplayName = "Load All Keyboards From JSON"))
    static TArray<FKeyboardDataStruct> LoadKeyboardsFromJson();

private:

    static EKeyboard_Status ParseStatus(const FString& StatusString);
    static EKeyboard_Type ParseKeyboardType(const FString& TypeString);
    static Less_Than_40Algo ParseLessThan40Algo(const FString& AlgoString);
};
