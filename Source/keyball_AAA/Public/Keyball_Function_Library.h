#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KeyballKeyboard.h"
#include "Keyball_Function_Library.generated.h"

UCLASS()
class KEYBALL_AAA_API UKeyball_Function_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public: 
    UFUNCTION(BlueprintCallable, Category = "Keyball")
    static void GetKeysForKeyboard(const TArray<int32>& KeyboardIDs, TArray<FKeycapSpawnData>& OutKeys);

	UFUNCTION(BlueprintPure, Category = "Keyball")
		static void GetKeyCode(FKey Key, int& KeyCode, int& CharacterCode);

	UFUNCTION(BlueprintCallable, Category = "Keyball")
		static FString FixUnknownCharCode(const FString& CharCode);

	// UFUNCTION(BlueprintCallable, Category = "Keyball")
	// 	static TArray<FTransform> CalculateBaseTransforms();

	// UFUNCTION(BlueprintCallable, Category = "Keyball")
	// 	static TArray<FKeycapSpawnData> GetKeycapSpawnData(const TArray<int32>& KeyboardIDs);
};