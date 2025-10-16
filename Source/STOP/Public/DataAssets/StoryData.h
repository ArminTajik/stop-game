
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StoryData.generated.h"

/**
 * 
 */
UCLASS()
class STOP_API UStoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Story"), GetFName());
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FText Body;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0))
	int32 Price;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bInitiallyUnlocked = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer Tags;
};
