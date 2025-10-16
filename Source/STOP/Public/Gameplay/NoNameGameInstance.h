#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DataAssets/GameRulesDataAsset.h"
#include "DataAssets/LevelDataAsset.h"
#include "DataAssets/StoryData.h"
#include "NoNameGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameInstance, Log, All);

class ULevelListDataAsset;
/**
 * Custom GameInstance for storing data between levels
 */
UCLASS()
class STOP_API UNoNameGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	void SetSelectedGameRules(UGameRulesDataAsset* SelectedGRules);
	
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetSelectedLevel(ULevelDataAsset* Level);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Level")
	void LoadNextLevel();

	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	bool TryGetGameRules(UGameRulesDataAsset*& OutGameRules) const;

	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	bool TryGetGameRulesFromLevelList(UGameRulesDataAsset*& OutGameRules, const ULevelListDataAsset* List) const;

	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	void SetPendingGameRules(UGameRulesDataAsset* NewGameRules);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void PurchaseStory(UStoryData* StoryData);
	

	static FString GetLevelNameFromMap(const TSoftObjectPtr<UWorld>& Map);
	static FName GetLevelPackageName(const TSoftObjectPtr<UWorld>& Map);
	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	FString GetCleanLevelName() const;

	int32 CurrentLevelIndex = -1;

	UPROPERTY()
	UGameRulesDataAsset* SelectedGameRules;
	ULevelListDataAsset* GetLevelList() const { return LevelList; }

protected:
	// Stores the selected GameRules asset to be used in the next level
	UPROPERTY(BlueprintReadWrite, Category = "Game Rules")
	UGameRulesDataAsset* PendingGameRules = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Game Rules")
	ULevelDataAsset* CurrentLevelDataAsset = nullptr;

	UPROPERTY()
	ULevelDataAsset* SelectedLevel = nullptr;
	
	FString SelectedLevelName;
	int32 SelectedLevelIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level List")
	ULevelListDataAsset* LevelList = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	TArray<UStoryData*> OwnedStories;
	
};
