// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Keyball_Function_Library.generated.h"
//#include "SDL.h"


/**
 * 
 */
UCLASS()
class KEYBALL_AAA_API UKeyball_Function_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public: 
	UFUNCTION(BlueprintPure)
		static void GetKeyCode(FKey Key, int& KeyCode, int& CharacterCode);

};
