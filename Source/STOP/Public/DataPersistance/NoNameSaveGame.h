
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NoNameSaveGame.generated.h"


class UStoryData;
/**
 * 
 */
UCLASS()
class STOP_API UNoNameSaveGame : public USaveGame
{
	GENERATED_BODY()

	void SaveGame();
	void LevelCompleted(int32 LevelJustCompleted);

	// Game Data

public:
	
	UPROPERTY(SaveGame)
	int HighestScore = 0;

	UPROPERTY(SaveGame)
	int HighestUnlockedLevel = 0;

	UPROPERTY(SaveGame)
	int CurrentScore = 0;

	UPROPERTY(SaveGame)
	TArray<float> BestTimesPerLevel;

	UPROPERTY(SaveGame)
	TSet<FPrimaryAssetId> UnlockedStories;
};
