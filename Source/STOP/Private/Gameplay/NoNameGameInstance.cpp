#include "Gameplay/NoNameGameInstance.h"
#include "DataAssets/LevelListDataAsset.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogGameInstance);

void UNoNameGameInstance::SetSelectedGameRules(UGameRulesDataAsset* SelectedGRules)
{
	if (!SelectedGRules || SelectedGRules->Name.IsEmpty())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Selected game rules are null or have an empty display name!"));
		return;
	}

	this->SelectedGameRules = SelectedGRules;
}

void UNoNameGameInstance::SetSelectedLevel(ULevelDataAsset* Level)
{
	if (!Level)
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Selected level is null!"));
		return;
	}

	TSoftObjectPtr<UWorld> MapRef = Level->MapReference;

	if (MapRef.IsNull())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Selected level map reference is null!"));
		return;
	}
	
	this->SelectedLevel = Level;

	const FName LevelPkgName = GetLevelPackageName(Level->MapReference);
	if (LevelPkgName.IsNone())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Invalid level package name!"));
		return;
	}
	SelectedLevelName = LevelPkgName.ToString();
	UE_LOG(LogGameInstance, Log, TEXT("Level to load: %s"), *SelectedLevelName);

	if (SelectedLevelName.IsEmpty())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Failed to get level name to load!"));
		return;
	}

	if (LevelList)
	{
		SelectedLevelIndex = LevelList->Levels.IndexOfByKey(Level);
	}
	else
	{
		SelectedLevelIndex = -1;
	}

	UE_LOG(LogGameInstance, Log, TEXT("Level to load: %s"), *SelectedLevelName);
}

void UNoNameGameInstance::StartGame()
{
	if (SelectedLevel)
	{
		CurrentLevelDataAsset = SelectedLevel;
	}
	else
	{
		UE_LOG(LogGameInstance, Warning, TEXT("No level selected! Cannot start game."));
		return;
	}

	if (SelectedLevelName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected level is empty!"));
		return;
	}

	CurrentLevelIndex = SelectedLevelIndex;

	if (SelectedGameRules)
	{
		SetPendingGameRules(SelectedGameRules);
	}
	
	UGameplayStatics::OpenLevel(this, FName(*SelectedLevelName), true);
}

void UNoNameGameInstance::LoadNextLevel()
{
	UE_LOG(LogGameInstance, Log, TEXT("Attempting to go to next level..."));
	if (!LevelList)
	{
		UE_LOG(LogGameInstance, Warning, TEXT("LevelListDataAsset is not set! Cannot go to next level."));
		return;
	}
	const int NextLevelIndex = CurrentLevelIndex + 1;

	if (NextLevelIndex >= LevelList->Levels.Num())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("No more levels to go to! Current level index: %d"),
		       NextLevelIndex);
		return;
	}

	ULevelDataAsset* NextLevelData = LevelList->Levels[NextLevelIndex];
	if (!NextLevelData)
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Next level data asset is null! Cannot go to next level."));
		return;
	}

	SetSelectedLevel(NextLevelData);
	StartGame();
}

bool UNoNameGameInstance::TryGetGameRules(UGameRulesDataAsset*& OutGameRules) const
{
	if (PendingGameRules)
	{
		OutGameRules = PendingGameRules;
		UE_LOG(LogGameInstance, Log, TEXT("Using Pending Game Rules: %s"),
		       *OutGameRules->GetName());
		return true;
	}

	if (CurrentLevelDataAsset && CurrentLevelDataAsset->GameRules)
	{
		OutGameRules = CurrentLevelDataAsset->GameRules;
		UE_LOG(LogGameInstance, Log, TEXT("Using Game Rules from Current Level Data Asset: %s"),
		       *OutGameRules->GetName());
		return true;
	}

	UE_LOG(LogGameInstance, Warning,
	       TEXT("No Game Rules found! Please set PendingGameRules or CurrentLevelDataAsset->GameRules."));
	return false;
}

bool UNoNameGameInstance::TryGetGameRulesFromLevelList(UGameRulesDataAsset*& OutGameRules,
                                                       const ULevelListDataAsset* List) const
{
	UE_LOG(LogGameInstance, Log, TEXT("No Active Game Rules found. Checking Level List Data Asset: %s"),
	       *List->GetName());

	const FString LevelName = GetCleanLevelName();

	UE_LOG(LogGameInstance, Log, TEXT("Current Map Name: %s"), *LevelName);
	for (const ULevelDataAsset* LevelData : List->Levels)
	{
		if (!LevelData || !LevelData->MapReference)
			continue;
		UE_LOG(LogGameInstance, Log, TEXT("Checking Level Data Asset: %s"), *LevelData->GetName());
		if (LevelData->MapReference.ToString().Contains(LevelName))
		{
			// Found the level data asset for the current level
			UE_LOG(LogGameInstance, Log, TEXT("Found Level Data Asset: %s"), *LevelData->GetName());

			// Use the Game Rules from this Level Data Asset
			if (LevelData->GameRules)
			{
				OutGameRules = LevelData->GameRules;
				UE_LOG(LogGameInstance, Log, TEXT("Using Game Rules from Level Data Asset: %s"),
				       *OutGameRules->GetName());
				return true;
			}
		}
	}
	return false;
}

FString UNoNameGameInstance::GetCleanLevelName() const
{
	const FString RawMapName = GetWorld() ? GetWorld()->GetMapName() : FString();
	FString CleanMapName = RawMapName;

	const FString PIEPrefix = TEXT("UEDPIE_");
	if (int32 UnderscoreIndex; RawMapName.StartsWith(PIEPrefix) && RawMapName.
		FindChar('_', UnderscoreIndex))
	{
		if (int32 PrefixEnd = RawMapName.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromStart,
		                                      PIEPrefix.Len()); PrefixEnd != INDEX_NONE && PrefixEnd + 1 < RawMapName.
			Len())
		{
			CleanMapName = RawMapName.Mid(PrefixEnd + 1); // Correctly skips past "UEDPIE_0_"
		}
	}


	return CleanMapName;
}

void UNoNameGameInstance::SetPendingGameRules(UGameRulesDataAsset* NewGameRules)
{
	if (NewGameRules)
	{
		PendingGameRules = NewGameRules;
		UE_LOG(LogGameInstance, Log, TEXT("Pending Game Rules set to: %s"), *NewGameRules->GetName());
	}
	else
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Attempted to set Pending Game Rules to null!"));
	}
}


void UNoNameGameInstance::PurchaseStory(UStoryData* StoryData)
{
	if (!StoryData)
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Attempted to purchase null story data!"));
		return;
	}

	if (UDataPersistenceManager* DPM = GetSubsystem<UDataPersistenceManager>())
	{
		int32 PlayerScore = DPM->GetCurrentScore();
		if (PlayerScore < StoryData->Price)
		{
			UE_LOG(LogGameInstance, Warning, TEXT("Not enough score to purchase story '%s'. Required: %d, Current: %d"),
			       *StoryData->Title.ToString(), StoryData->Price, PlayerScore);
			return;
		}
		if (DPM->IsStoryUnlocked(StoryData->GetPrimaryAssetId()))
		{
			UE_LOG(LogGameInstance, Log, TEXT("Story '%s' is already unlocked in save data."), *StoryData->Title.ToString());
			return;
		}
		// Deduct score and unlock story
		PlayerScore -= StoryData->Price;
		DPM->SaveCurrentScore(PlayerScore);
		DPM->UnlockStoryById(StoryData->GetPrimaryAssetId());
	}
}


FString UNoNameGameInstance::GetLevelNameFromMap(const TSoftObjectPtr<UWorld>& Map)
{
	FString LevelName;
	if (Map.IsNull())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Selected level map pointer is null!"));
		return LevelName;
	}

	if (const FString AssetPath = Map.ToSoftObjectPath().GetAssetPathString(); AssetPath.IsEmpty())
	{
		UE_LOG(LogGameInstance, Warning, TEXT("Map pointer is invalid!"));
		return LevelName;
	}

	return FPackageName::GetShortName(Map.GetLongPackageName());
}

FName UNoNameGameInstance::GetLevelPackageName(const TSoftObjectPtr<UWorld>& Map)
{
	const FSoftObjectPath Path = Map.ToSoftObjectPath();
	const FString LongName = Path.GetLongPackageName();
	return FName(*LongName);
}
