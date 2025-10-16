#pragma once

#include "CoreMinimal.h"
#include "GameModeDataAsset.h"
#include "Engine/DataAsset.h"
#include "GameModeListDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STOP_API UGameModeListDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UGameModeDataAsset*> GameModes;
};
