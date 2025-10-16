#include "Components/StatsManagerComponent.h"
#include "Actors/UI/ScorePopUpActor.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Gameplay/NoNamePlayerController.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogStatsManager);

// Sets default values for this component's properties
void UStatsManagerComponent::Init(FGameStats InitStats)
{
	GameStats = InitStats;

	if (GetWorld())
	{
		if (const ANoNameGameModeBase* GM = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			ActiveGameRules = GM->ActiveGameRules;
		}
	}
}

void UStatsManagerComponent::ApplyEvent(APassengerCharacterBase* Passenger, ECharacterEvent Event)
{
	UE_LOG(LogStatsManager, Log, TEXT("Applying event %s to passenger: %s"), *UEnum::GetValueAsString(Event),
	       *GetNameSafe(Passenger));
	if (!IsValid(Passenger)) return;
	if (!IsValid(ActiveGameRules))
	{
		UE_LOG(LogStatsManager, Warning, TEXT("ActiveGameRules is not set! Cannot apply event."));
		return;
	}

	int ScoreAdded = 0;
	int HealthAdded = 0;

	for (const auto& [CharacterTag, EventType, StatsType, ScoreDelta] : ActiveGameRules->CharacterEventScores)
	{
		if (EventType != Event || !Passenger->CharacterType.Tags.HasTag(CharacterTag))
		{
			continue;
		}

		switch (StatsType)
		{
		case EStatsType::Score:
			ScoreAdded += ScoreDelta;
			break;
		case EStatsType::Health:
			HealthAdded += ScoreDelta;
			break;
		default:
			UE_LOG(LogStatsManager, Warning, TEXT("Unknown stats type for event %s"), *UEnum::GetValueAsString(Event));
			break;
		}
	}
	const FVector PassengerLocation = Passenger->GetActorLocation();

	if (ScoreAdded != 0)
	{
		AddScore(ScoreAdded);
		const FColor ScoreColor = (ScoreAdded >= 0) ? FColor::Yellow : FColor::Orange;
		SpawnWorldStatsUI(ScoreAdded, "Score", ScoreColor, PassengerLocation);
	}

	if (HealthAdded != 0)
	{
		AddHealth(HealthAdded);
		const FColor HealthColor = (HealthAdded >= 0) ? FColor::Green : FColor::Red;
		SpawnWorldStatsUI(HealthAdded, "Health", HealthColor, PassengerLocation);
	}

	if (ScoreAdded < 0 || HealthAdded < 0)
	{
		ResetCombo();
	}
	if (ScoreAdded > 0 && (Event == ECharacterEvent::Exit || Event == ECharacterEvent::Destroy))
	{
		AddCombo(1);
		const FColor ComboColor = FColor::White;
		SpawnWorldStatsUI(GameStats.Combo, "Combo", ComboColor, PassengerLocation);
	}

	ResetStatsUI();
}

void UStatsManagerComponent::AddScore(int32 Amount)
{
	if (Amount == 0) return;

	GameStats.CurrentScore += Amount;

	
	GameStats.CurrentScore = FMath::Max(0, GameStats.CurrentScore);
	
	if (UDataPersistenceManager* DPM = GetWorld()->GetGameInstance()->GetSubsystem<UDataPersistenceManager>())
	{
		DPM->SaveCurrentScore(GameStats.CurrentScore);
	}

	OnScoreChanged.Broadcast(GameStats.CurrentScore);
}

void UStatsManagerComponent::AddHealth(int32 Amount)
{
	if (Amount == 0) return;

	GameStats.CurrentHealth += Amount;
	GameStats.CurrentHealth = FMath::Max(0, GameStats.CurrentHealth);

	if (GameStats.CurrentHealth <= 0)
	{
		// Handle game over logic here if needed
		UE_LOG(LogStatsManager, Warning, TEXT("Health reached zero! Game Over!"));
	}

	OnHealthChanged.Broadcast(GameStats.CurrentHealth);
}

void UStatsManagerComponent::AddCombo(int32 Amount)
{
	if (Amount == 0) return;

	GameStats.Combo += Amount;
	GameStats.Combo = FMath::Max(0, GameStats.Combo);

	OnComboChanged.Broadcast(GameStats.Combo);
}

void UStatsManagerComponent::ResetCombo()
{
	GameStats.Combo = 0;
}

void UStatsManagerComponent::OnLoadStats(const FGameStats LoadedStats)
{
	GameStats = LoadedStats;
}

void UStatsManagerComponent::SpawnWorldStatsUI(int32 Score, const FString ScoreType, const FColor ScoreColor,
                                               const FVector& Location)
{
	if (!IsValid(ScorePopupActorClass))
	{
		UE_LOG(LogStatsManager, Warning, TEXT("ScorePopupActorClass is not set! Cannot spawn score UI."));
		return;
	}
	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogStatsManager, Warning, TEXT("World is not valid! Cannot spawn score UI."));
		return;
	}

	UE_LOG(LogStatsManager, Log, TEXT("Spawning score UI at location: %s with score: %d"),
	       *Location.ToString(), Score);

	const FVector SpawnLocation = Location + FVector(0, 0, 100 + (StatUINum * 50));
	const FActorSpawnParameters Params;

	if (AScorePopupActor* Popup = GetWorld()->SpawnActor<AScorePopupActor>(ScorePopupActorClass, SpawnLocation,
	                                                                       FRotator::ZeroRotator, Params))
	{
		const FString ScoreSign = (Score >= 0) ? TEXT("+") : TEXT("");
		Popup->Init(FText::FromString(ScoreSign + FString::FromInt(Score) + " " + ScoreType),
		            1.5f, ScoreColor);
		StatUINum++;
	}
}


void UStatsManagerComponent::ResetStatsUI()
{
	UE_LOG(LogStatsManager, Verbose, TEXT("Resetting Stats UI counter from %d to 0"), StatUINum);
	StatUINum = 0;
}
