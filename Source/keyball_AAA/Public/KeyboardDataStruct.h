#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // For FTableRowBase
#include "Engine/StaticMesh.h"
#include "KeyboardDataStruct.generated.h"

// Define enums based on your Blueprint struct (adjust if defined elsewhere)
UENUM(BlueprintType)
enum class EKeyboard_Status : uint8
{
    Free,
    Locked,
    Unlocked
};

UENUM(BlueprintType)
enum class EKeyboard_Type : uint8
{
    Regular40,
    LessThan40,
    Sliced
};

UENUM(BlueprintType)
enum class Less_Than_40Algo : uint8
{
    Loop,
    Halves,
    HalvesRotated,
    edgesThenCenter
};

USTRUCT(BlueprintType)
struct FKeyboardDataStruct : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    FName keyboardName;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    TArray<TSoftObjectPtr<UStaticMesh>> staticMesh;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    EKeyboard_Status status;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    bool active;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    EKeyboard_Type keyboardType;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    float displayScale;

    UPROPERTY(BlueprintReadWrite, Category = "Keyboard")
    Less_Than_40Algo lessThan40Algo;

    FKeyboardDataStruct()
    {
        keyboardName = NAME_None;
        status = EKeyboard_Status::Free;
        active = false;
        keyboardType = EKeyboard_Type::LessThan40;
        displayScale = 1.0f;
        lessThan40Algo = Less_Than_40Algo::edgesThenCenter;
    }
};