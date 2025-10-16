#include "Actors/EnterDoorActor.h"
#include "NavigationSystem.h"
#include "Actors/PathActor.h"
#include "Gameplay/GameModes/NoNameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/NpcManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogSpawner);

// Sets default values
AEnterDoorActor::AEnterDoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));\
	VisualMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMarker"));
	VisualMarker->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	// Load a basic cube mesh
	if (CubeMesh.Succeeded())
	{
		VisualMarker->SetStaticMesh(CubeMesh.Object);
		VisualMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisualMarker->SetGenerateOverlapEvents(false);
		VisualMarker->SetVisibility(true);
		VisualMarker->SetHiddenInGame(true);
	}
}

void AEnterDoorActor::HandleGameOver(FLevelData LevelData)
{
	StopSpawning();
}

// Called when the game starts or when spawned
void AEnterDoorActor::BeginPlay()
{
	Super::BeginPlay();

	if (ANoNameGameModeBase* Gm = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)); IsValid(Gm))
	{
		Gm->OnGameOver.AddDynamic(this, &AEnterDoorActor::HandleGameOver);

		if (Gm->ActiveGameRules)
		{
			// Use the values from the game rules if available
			SpawnRate = Gm->ActiveGameRules->SpawnRate;
			SpawnRateUpdateInterval = Gm->ActiveGameRules->SpawnRateUpdateInterval;
			MinSpawnRate = Gm->ActiveGameRules->MinSpawnRate;
			SpawnRateDecrement = Gm->ActiveGameRules->SpawnRateDecrement;
			SpawnableCharacters = Gm->ActiveGameRules->SpawnableCharacters;
		}
		else
		{
			// Fallback to default values
			SpawnRate = DefaultSpawnRate;
			SpawnRateUpdateInterval = 5.0f; // Default value
			MinSpawnRate = 0.5f; // Default value
			SpawnRateDecrement = 0.1f; // Default value
			SpawnableCharacters = DefaultSpawnableCharacters;
		}


		if (UNpcManagerSubsystem* Manager = GetGameInstance()->GetSubsystem<UNpcManagerSubsystem>(); IsValid(Manager))
		{
			SpawnerID = this->GetUniqueID();
			Manager->RegisterSpawner(SpawnerID);
		}
	}

	FTimerHandle DelayedInitHandle;
	GetWorldTimerManager().SetTimer(DelayedInitHandle, this, &AEnterDoorActor::InitializeSpawn, 0.1f, false);
}

void AEnterDoorActor::InitializeSpawn()
{
	UE_LOG(LogSpawner, Log, TEXT("Initializing spawn with SpawnRate: %f, MinSpawnRate: %f, SpawnRateDecrement: %f"),
	       SpawnRate, MinSpawnRate, SpawnRateDecrement);

	if (!IsValid(ExitDoorActor))
	{
		UE_LOG(LogSpawner, Warning, TEXT("ExitDoorActor is not set! Cannot initialize spawn."));
		return;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!IsValid(NavSys))
	{
		UE_LOG(LogSpawner, Warning, TEXT("Navigation system is not valid! Cannot project point to navigation."));
		return;
	}

	// Get the spawn location and destination location

	// Project the spawn location to navigation
	SpawnLocation = GetActorLocation();
	FNavLocation ProjectedLocation;
	if (!NavSys->ProjectPointToNavigation(
		SpawnLocation, ProjectedLocation, FVector(100.f, 100.f, 100.f)))
	{
		UE_LOG(LogSpawner, Warning, TEXT("Failed to project spawn point to navigation."));
		return;
	}

	// Project the exit door location to navigation
	DestinationLocation = ExitDoorActor->GetActorLocation();
	FNavLocation ProjectedDestinationLocation;
	if (!NavSys->ProjectPointToNavigation(DestinationLocation, ProjectedDestinationLocation,
	                                      FVector(100.f, 100.f, 100.f)))
	{
		UE_LOG(LogSpawner, Warning, TEXT("Failed to project destination point to navigation."));
		return;
	}

	SpawnLocation = ProjectedLocation.Location;
	SpawnRotation = this->GetActorRotation();
	DestinationLocation = ProjectedDestinationLocation.Location;

	UE_LOG(LogSpawner, Log, TEXT("Projected spawn point to navigation: %s \n projecting to destination: %s"),
	       *SpawnLocation.ToString(),
	       *DestinationLocation.ToString());


	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnterDoorActor::SpawnPassenger, SpawnRate, true);
	GetWorldTimerManager().SetTimer(SpawnRateUpdateHandle, this, &AEnterDoorActor::UpdateSpawnRate,
	                                SpawnRateUpdateInterval, true);
}


void AEnterDoorActor::SpawnPassenger()
{
	const TSubclassOf<APassengerCharacterBase> PassengerClass = PickRandomCharacter();

	if (!PassengerClass)
	{
		UE_LOG(LogSpawner, Warning, TEXT("Passenger class is null! Cannot spawn."));
		return;
	}

	// Check if there is enough space to spawn the character
	if (const UGameInstance* GI = GetGameInstance(); IsValid(GI))
	{
		if (UNpcManagerSubsystem* Manager = GI->GetSubsystem<UNpcManagerSubsystem>(); IsValid(Manager))
		{
			// Check for nearby NPCs to ensure there's enough space to spawn
			constexpr float CheckRadius = 50.f;
			TArray<APassengerCharacterBase*> Nearby = Manager->GetNearbyNPCs(
				SpawnLocation, CheckRadius);
			if (Nearby.Num() > 0)
			{
				UE_LOG(LogSpawner, Warning, TEXT("Not enough space to spawn character at: %s"),
				       *SpawnLocation.ToString());
				return; // Not enough space to spawn
			}
		}
	}
	if (APassengerCharacterBase* Passenger = GetWorld()->SpawnActor<APassengerCharacterBase>(
		PassengerClass, SpawnLocation, SpawnRotation))
	{
		Passenger->SetSpawnerID(SpawnerID);
		// Assuming PassengerBase has a function to assign a spline
		Passenger->InitializeMovement(DestinationLocation);

		if (const ANoNameGameModeBase* Gm = Cast<ANoNameGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			Gm->OnCharacterEntered.Broadcast(Passenger);
		}
	}
}

void AEnterDoorActor::StopSpawning()
{
	UE_LOG(LogSpawner, Log, TEXT("Stopping spawning."));
	// Stop the spawn timer
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(SpawnRateUpdateHandle);
}

TSubclassOf<APassengerCharacterBase> AEnterDoorActor::PickRandomCharacter()
{
	if (SpawnableCharacters.Num() <= 0)
	{
		UE_LOG(LogSpawner, Warning, TEXT("No characters available to spawn!"));
		return nullptr;
	}

	float TotalWeight = 0.0f;
	for (const FSpawnableCharacterEntry& Entry : SpawnableCharacters)
	{
		TotalWeight += Entry.SpawnProbability;
	}

	if (TotalWeight <= 0.f)
	{
		UE_LOG(LogSpawner, Warning, TEXT("No characters to spawn!"));
		return nullptr;
	}

	float Pick = FMath::FRandRange(0.0f, TotalWeight);
	float RunningSum = 0.0f;

	for (const FSpawnableCharacterEntry& Entry : SpawnableCharacters)
	{
		RunningSum += Entry.SpawnProbability;
		if (Pick <= RunningSum)
		{
			return Entry.CharacterClass;
		}
	}

	return nullptr; // fallback if no character was picked
}

void AEnterDoorActor::UpdateSpawnRate()
{
	if (SpawnRate > MinSpawnRate)
	{
		SpawnRate = FMath::Max(MinSpawnRate, SpawnRate - SpawnRateDecrement);
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnterDoorActor::SpawnPassenger, SpawnRate, true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnRateUpdateHandle);
	}
}
