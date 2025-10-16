#include "DataPersistance/DataPersistenceManager.h"
#include "DataAssets/StoryData.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"


static const TCHAR* GKSaveSlot = TEXT("PlayerSaveSlot");
static constexpr int32 GKUserIdx = 0;
DEFINE_LOG_CATEGORY(LogDataPersistence);

void UDataPersistenceManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Save = LoadOrCreateSave();
}

UNoNameSaveGame* UDataPersistenceManager::LoadOrCreateSave()
{
	if (UGameplayStatics::DoesSaveGameExist(GKSaveSlot, GKUserIdx))
	{
		return Cast<UNoNameSaveGame>(UGameplayStatics::LoadGameFromSlot(GKSaveSlot, GKUserIdx));
	}
	return Cast<UNoNameSaveGame>(UGameplayStatics::CreateSaveGameObject(UNoNameSaveGame::StaticClass()));
}

void UDataPersistenceManager::SaveNow()
{
	if (Save)
	{
		UGameplayStatics::SaveGameToSlot(Save, GKSaveSlot, GKUserIdx);
	}
}

void UDataPersistenceManager::MarkDirtyAndSave()
{
	SaveNow();
}

void UDataPersistenceManager::UnlockStoryById(const FPrimaryAssetId& StoryId)
{
	if (!Save) Save = LoadOrCreateSave();

	if (!StoryId.IsValid()) return;

	if (Save->UnlockedStories.Contains(StoryId)) return;
	Save->UnlockedStories.Add(StoryId);
	MarkDirtyAndSave();
}

bool UDataPersistenceManager::IsStoryUnlocked(const FPrimaryAssetId& StoryId) const
{
	return Save && Save->UnlockedStories.Contains(StoryId);
}

TArray<FPrimaryAssetId> UDataPersistenceManager::GetUnlockedStoryIds() const
{
	TArray<FPrimaryAssetId> Out;
	if (Save)
	{
		Out = Save->UnlockedStories.Array();
	}
	return Out;
}

TArray<FPrimaryAssetId> UDataPersistenceManager::GetLockedStoryIds() const
{
	TArray<FPrimaryAssetId> Out;
	if (!Save) return Out;

	UAssetManager& AM = UAssetManager::Get();
	TArray<FPrimaryAssetId> AllStories;
	AM.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("Story")), AllStories);

	for (const FPrimaryAssetId& Id : AllStories)
	{
		if (!Save->UnlockedStories.Contains(Id))
		{
			Out.Add(Id);
		}
	}

	// Sort price low to high
	Out.Sort([&AM](const FPrimaryAssetId& A, const FPrimaryAssetId& B)
	{
		int32 PriceA = 0;
		int32 PriceB = 0;

		if (UObject* ObjA = AM.GetPrimaryAssetObject(A))
		{
			if (UStoryData* StoryA = Cast<UStoryData>(ObjA))
			{
				PriceA = StoryA->Price;
			}
		}

		if (UObject* ObjB = AM.GetPrimaryAssetObject(B))
		{
			if (UStoryData* StoryB = Cast<UStoryData>(ObjB))
			{
				PriceB = StoryB->Price;
			}
		}

		return PriceA < PriceB;
	});
	return Out;
}

void UDataPersistenceManager::LoadUnlockedStoriesAsync(
	TFunction<void(const TArray<UStoryData*>&)> OnLoaded)
{
	if (!Save)
	{
		OnLoaded({});
		return;
	}

	UAssetManager& AM = UAssetManager::Get();
	TArray<FPrimaryAssetId> Ids = Save->UnlockedStories.Array();

	// Bulk-load all primary assets (no bundles)
	AM.LoadPrimaryAssets(Ids, {}, FStreamableDelegate::CreateWeakLambda(this, [this, OnLoaded, Ids, &AM]()
	{
		TArray<UStoryData*> Result;
		Result.Reserve(Ids.Num());

		for (const FPrimaryAssetId& Id : Ids)
		{
			if (FSoftObjectPath Path = AM.GetPrimaryAssetPath(Id); Path.IsValid())
			{
				if (UObject* Obj = Path.ResolveObject())
				{
					if (auto* Story = Cast<UStoryData>(Obj))
						Result.Add(Story);
				}
			}
		}
		OnLoaded(Result);
	}));
}

int32 UDataPersistenceManager::GetCurrentScore() const
{
	return Save ? Save->CurrentScore : 0;
}

int32 UDataPersistenceManager::GetHighestUnlockedLevel() const
{
	return Save ? Save->HighestUnlockedLevel : 0;
}

int32 UDataPersistenceManager::GetHighestScore() const
{
	return Save ? Save->HighestScore : 0;
}

void UDataPersistenceManager::LevelCompleted(int32 LevelJustCompleted)
{
	if (!Save) Save = LoadOrCreateSave();
	Save->HighestUnlockedLevel = FMath::Max(Save->HighestUnlockedLevel, LevelJustCompleted);
	MarkDirtyAndSave();
}

void UDataPersistenceManager::CheckForHighScore(int32 NewScore)
{
	if (!Save) Save = LoadOrCreateSave();
	if (NewScore > Save->HighestScore)
	{
		Save->HighestScore = NewScore;
		MarkDirtyAndSave();
	}
}

void UDataPersistenceManager::SaveCurrentScore(int32 Score)
{
	if (!Save) Save = LoadOrCreateSave();
	Save->CurrentScore = Score;
	CheckForHighScore(Score);
	MarkDirtyAndSave();
}
