#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KeyballPlayerState.generated.h"

UCLASS()
class KEYBALL_AAA_API AKeyballPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
        int32 TeamNumber = -1;

    UFUNCTION(BlueprintCallable, Category = "Team")
        int32 GetTeamNumber() const { return TeamNumber; }
};
