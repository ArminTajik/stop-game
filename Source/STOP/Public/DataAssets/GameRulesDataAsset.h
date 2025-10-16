
#pragma once

#include "CoreMinimal.h"
#include "CharacterDataAsset.h"
#include "Engine/DataAsset.h"
#include "GameRulesDataAsset.generated.h"


class APassengerCharacterBase;
class USpawnDataAsset;

UENUM(BlueprintType)
enum class ECharacterEvent : uint8
{
	Enter,
	Exit,
	Interact,
	Destroy,
	Scan,
};

UENUM(BlueprintType)
enum class EStatsType : uint8
{
	Score,
	Health,
};

UENUM(BlueprintType)
enum class EPowerUp : uint8
{
	None,
	EliminateAllAliens,
	RevealAllCharacters,
	StopRandomSpawner,
};

USTRUCT(BlueprintType)
struct FCharacterSpecialMove
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CharacterTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPowerUp PowerUpType = EPowerUp::None;
};

USTRUCT(BlueprintType)
struct FCharacterEventScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CharacterTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterEvent EventType = ECharacterEvent::Enter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatsType StatsType = EStatsType::Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;
};

USTRUCT(BlueprintType)
struct FComboPowerUp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="1", ToolTip="Combo count required to gain this power up"))
	int32 ComboCount = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="The power up granted when reaching the combo count"))
	EPowerUp PowerUpType = EPowerUp::None;
};

USTRUCT(BlueprintType)
struct FSpawnableCharacterEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APassengerCharacterBase> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta = (ClampMin = "0.0", ToolTip =
			"Probability of this character being spawned. Relative weight (not required to add up to 1)"))
	float SpawnProbability = 1.0f; 
};

/**
 * 
 */
UCLASS()
class STOP_API UGameRulesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="General", meta=(MultiLine=true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	int32 MaxPassengers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	int32 UnlockNextLevelAtScore = 1000;

	// Stats =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxScore = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 InitialHealth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TArray<FComboPowerUp> ComboPowerUps;

	// Spawn Settings ================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FSpawnableCharacterEntry> SpawnableCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn",
		meta = (ClampMin = "0.01", ToolTip = "Time between spawns (in seconds)"))
	float SpawnRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn",
		meta = (ClampMin = "0.01", ToolTip = "Interval to update spawn rate (in seconds)"))
	float SpawnRateUpdateInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn",
		meta = (ClampMin = "0.01", ToolTip = "Amount to subtract from spawn rate at each interval"))
	float SpawnRateDecrement = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn",
		meta = (ClampMin = "0.01", ToolTip = "Minimum value the spawn rate can reach"))
	float MinSpawnRate = 0.5f;

	// Character Settings =========================================

	UPROPERTY(EditAnywhere, Category="WalkSpeed", meta=(ClampMin="0.01", ToolTip = "Initial walking speed"))
	float InitialWalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category="WalkSpeed",
		meta=(ClampMin="0.01", ToolTip = "Interval to update walking speed (in seconds)"))
	float WalkSpeedUpdateInterval = 5.0f;

	UPROPERTY(EditAnywhere, Category="WalkSpeed", meta=(ClampMin="0.01", ToolTip = "Maximum walking speed"))
	float MaxWalkSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category="WalkSpeed",
		meta=(ClampMin="0.01", ToolTip = "Amount to increment walking speed at each interval"))
	float WalkSpeedIncrement = 0.1f;

	// Event Rules ================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Rules")
	TArray<FCharacterEventScore> CharacterEventScores;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event Rules")
	TArray<FCharacterSpecialMove> CharacterPowerUps;
};
