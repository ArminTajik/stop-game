
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameRulesDataAsset.h"
#include "LevelDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class STOP_API ULevelDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level", meta=(MultiLine=true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TSoftObjectPtr<UWorld> MapReference;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	UGameRulesDataAsset* GameRules;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bInitiallyUnlocked = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer Tags;
};
