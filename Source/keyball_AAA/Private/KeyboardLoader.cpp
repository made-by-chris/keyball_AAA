#include "KeyboardLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"
#include "JsonUtilities.h"

TArray<FKeyboardDataStruct> UKeyboardLoader::LoadKeyboardsFromJson()
{
    TArray<FKeyboardDataStruct> AllKeyboards;

    FString FilePath = FPaths::ProjectContentDir() / TEXT("data/keyboards.json");
    FString JsonString;

    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file at: %s"), *FilePath);
        return AllKeyboards;
    }

    TSharedPtr<FJsonValue> ParsedJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, ParsedJson) || !ParsedJson.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON file."));
        return AllKeyboards;
    }

    const TArray<TSharedPtr<FJsonValue>>* JsonArray;
    if (!ParsedJson->TryGetArray(JsonArray))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON root is not an array."));
        return AllKeyboards;
    }

    for (const TSharedPtr<FJsonValue>& Value : *JsonArray)
    {
        const TSharedPtr<FJsonObject>* Obj;
        if (!Value->TryGetObject(Obj)) continue;

        FKeyboardDataStruct Keyboard;

        Keyboard.keyboardName = FName((*Obj)->GetStringField("name"));

        const TArray<TSharedPtr<FJsonValue>>* MeshArray;
        if ((*Obj)->TryGetArrayField("staticMesh", MeshArray))
        {
            for (const TSharedPtr<FJsonValue>& MeshVal : *MeshArray)
            {
                FString Path = MeshVal->AsString();
                if (Path != "None")
                {
                    Keyboard.staticMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(Path)));
                }
            }
        }

        Keyboard.status = ParseStatus((*Obj)->GetStringField("status"));
        Keyboard.active = (*Obj)->GetBoolField("active");
        Keyboard.keyboardType = ParseKeyboardType((*Obj)->GetStringField("keyboardType"));
        Keyboard.displayScale = (*Obj)->GetNumberField("displayScale");
        Keyboard.lessThan40Algo = ParseLessThan40Algo((*Obj)->GetStringField("LessThan40Algo"));

        AllKeyboards.Add(Keyboard);

    }

    return AllKeyboards;
}

EKeyboard_Status UKeyboardLoader::ParseStatus(const FString& StatusString)
{
    if (StatusString == "Free") return EKeyboard_Status::Free;
    if (StatusString == "Locked") return EKeyboard_Status::Locked;
    if (StatusString == "Unlocked") return EKeyboard_Status::Unlocked;
    return EKeyboard_Status::Free;
}

EKeyboard_Type UKeyboardLoader::ParseKeyboardType(const FString& TypeString)
{
    if (TypeString == "Regular40") return EKeyboard_Type::Regular40;
    if (TypeString == "LessThan40") return EKeyboard_Type::LessThan40;
    if (TypeString == "Sliced") return EKeyboard_Type::Sliced;
    return EKeyboard_Type::LessThan40;
}

Less_Than_40Algo UKeyboardLoader::ParseLessThan40Algo(const FString& AlgoString)
{
    if (AlgoString == "Loop") return Less_Than_40Algo::Loop;
    if (AlgoString == "Halves") return Less_Than_40Algo::Halves;
    if (AlgoString == "HalvesRotated") return Less_Than_40Algo::HalvesRotated;
    if (AlgoString == "edgesThenCenter") return Less_Than_40Algo::edgesThenCenter;
    return Less_Than_40Algo::Loop;
}
