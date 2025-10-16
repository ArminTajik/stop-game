
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameRulesDataAsset.h"
#include "GameRulesListDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STOP_API UGameRulesListDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UGameRulesDataAsset*> GameRules;
};
