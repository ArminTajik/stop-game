#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/GameRulesDataAsset.h"

#include "StatsManagerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStatsManager, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

USTRUCT(Blueprintable)
struct FGameStats
{
	GENERATED_BODY()

	int32 CurrentHealth;
	int32 CurrentScore;
	int32 HighScore;
	int32 Combo;
};

enum class ECharacterEvent : uint8;
class APassengerCharacterBase;
class AScorePopupActor;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STOP_API UStatsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Init(FGameStats InitStats);


	void ApplyEvent(APassengerCharacterBase* Passenger, ECharacterEvent Event);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddHealth(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddCombo(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetCombo();
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetScore() const
	{
		return GameStats.CurrentScore;
	}

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetHealth() const
	{
		return GameStats.CurrentHealth;
	}

	UFUNCTION(BlueprintCallable, Category = "Stats")
	int GetCombo() const
	{
		return GameStats.Combo;
	}

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnLoadStats(FGameStats LoadedStats);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<AScorePopupActor> ScorePopupActorClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SpawnWorldStatsUI(int32 Score, FString ScoreType, FColor ScoreColor, const FVector& Location);

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnComboChanged;

private:
	FGameStats GameStats;
	UPROPERTY()
	UGameRulesDataAsset* ActiveGameRules;
	
	int32 StatUINum;
	void ResetStatsUI();

};
