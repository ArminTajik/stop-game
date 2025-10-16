
#pragma once

#include "CoreMinimal.h"
#include "StoryDataList.generated.h"

class UStoryData;
/**
 * 
 */
UCLASS()
class STOP_API UStoryDataList : public UDataAsset
{
	GENERATED_BODY()

public:
   	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UStoryData*> Stories;
};
