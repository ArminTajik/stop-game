
#pragma once

#include "CoreMinimal.h"
#include "NoNameSaveGame.h"
#include "DataPersistenceManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDataPersistence, Log, All);

/**
 * 
 */
UCLASS()
class STOP_API UDataPersistenceManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LevelCompleted(int32 LevelJustCompleted);
	void CheckForHighScore(int32 NewScore);
	void SaveCurrentScore(int32 INT32);

	UFUNCTION(BlueprintCallable)
	void UnlockStoryById(const FPrimaryAssetId& StoryId);

	UFUNCTION(BlueprintPure)
	bool IsStoryUnlocked(const FPrimaryAssetId& StoryId) const;

	UFUNCTION(BlueprintPure)
	TArray<FPrimaryAssetId> GetUnlockedStoryIds() const;

	UFUNCTION(BlueprintCallable)
	TArray<FPrimaryAssetId> GetLockedStoryIds() const;

	UFUNCTION(BlueprintCallable)
	void SaveNow();

	void LoadUnlockedStoriesAsync(TFunction<void(const TArray<UStoryData*>&)> OnLoaded);
	int32 GetCurrentScore() const;
	int32 GetHighestUnlockedLevel() const;
	int32 GetHighestScore() const;


	static FString SaveSlotName;

private:
	UPROPERTY()
	UNoNameSaveGame* Save = nullptr;

	UNoNameSaveGame* LoadOrCreateSave();
	void MarkDirtyAndSave();
};
