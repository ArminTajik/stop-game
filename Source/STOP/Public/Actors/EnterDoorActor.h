
#pragma once

#include "CoreMinimal.h"
#include "Characters/PassengerCharacterBase.h"
#include "GameFramework/Actor.h"
#include "EnterDoorActor.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(LogSpawner, Log, All);

struct FSpawnableCharacterEntry;
class AExitDoorActor;
class APathActor;

UCLASS()
class STOP_API AEnterDoorActor : public AActor
{
	GENERATED_BODY()

public:
	AEnterDoorActor();
	UPROPERTY()
	int32 SpawnerID;

protected:
	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void HandleGameOver(FLevelData LevelData);
	
	virtual void BeginPlay() override;

	FTimerHandle SpawnTimerHandle;
	FTimerHandle SpawnRateUpdateHandle;


	UFUNCTION()
	void SpawnPassenger();

	UFUNCTION()
	void UpdateSpawnRate();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VisualMarker;


	float SpawnRate = 10.0f;
	float SpawnRateUpdateInterval = 5.0f;
	float MinSpawnRate = 0.5f;
	float SpawnRateDecrement = 0.1f;

public:
	UFUNCTION()
	void InitializeSpawn();
	TSubclassOf<APassengerCharacterBase> PickRandomCharacter();

	UFUNCTION()
	void StopSpawning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Exit")
	AExitDoorActor* ExitDoorActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "List of character data assets to be spawned"))
	TArray<FSpawnableCharacterEntry> DefaultSpawnableCharacters;

	// Time between spawns (in seconds)
	UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0.01"))
	float DefaultSpawnRate = 10.0f;

	// Interval to update spawn rate (in seconds)
	UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0.01"))
	float DefaultSpawnRateUpdateInterval = 5.0f;

	// Minimum spawn rate (in seconds)
	UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0.01"))
	float DefaultMinSpawnRate = 0.5f;

	// Decrement value for spawn rate
	UPROPERTY(EditAnywhere, Category="Spawning", meta=(ClampMin="0.01"))
	float DefaultSpawnRateDecrement = 0.1f;

private:
	UPROPERTY()
	TArray<FSpawnableCharacterEntry> SpawnableCharacters;

	UPROPERTY()
	FRotator SpawnRotation;

	UPROPERTY()
	FVector SpawnLocation;
	
	UPROPERTY()
	FVector DestinationLocation;
};
