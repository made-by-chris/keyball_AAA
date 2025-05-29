// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KEYBALL_AAA_API AC_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void PostSeamlessTravel() override;
virtual void PostSeamlessTravel_Implementation();
};
