#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KeyboardLayoutDetector.generated.h"

UCLASS()
class KEYBALL_AAA_API UKeyboardLayoutDetector : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Input")
    static FString GetCurrentKeyboardLayout();

    UFUNCTION(BlueprintCallable, Category = "Input")
    static FKey MapKeyToQWERTY(int32 KeyCode);
};