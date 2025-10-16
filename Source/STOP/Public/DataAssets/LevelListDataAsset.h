
#pragma once

#include "CoreMinimal.h"
#include "LevelDataAsset.h"
#include "Engine/DataAsset.h"
#include "LevelListDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class STOP_API ULevelListDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = "false"))
	TArray<ULevelDataAsset*> Levels;
};
