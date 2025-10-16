#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NPCManagerSubsystem.generated.h"

class AEnterDoorActor;
class APassengerCharacterBase;

UCLASS()
class STOP_API UNpcManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// NPC management
	void RegisterNPC(APassengerCharacterBase* NPC);
	void UnregisterNPC(APassengerCharacterBase* NPC);

	void RegisterSpawner(int32 SpawnerId);
	void UnregisterSpawner(int32 SpawnerId);

	TArray<APassengerCharacterBase*> GetNearbyNPCs(const FVector& Location, float Radius);


	int32 GetCurrentTurnSpawner() const
	{
		return (RegisteredSpawnerIds.Num() > 0) ? RegisteredSpawnerIds[TurnIndex] : -1;
	}

	bool IsSpawnerTurn(const int32 SpawnerId) const { return SpawnerId == GetCurrentTurnSpawner(); }

private:
	float CellSize = 100.0f;

	TMap<FIntPoint, TArray<APassengerCharacterBase*>> SpatialGrid;

	UPROPERTY()
	TSet<APassengerCharacterBase*> RegisteredNPCs;

	UPROPERTY()
	TArray<int32> RegisteredSpawnerIds;
	int32 TurnIndex = 0;


	UFUNCTION()
	void RebuildSpatialGrid();

	UFUNCTION()
	void AdvanceTurn();

	UFUNCTION()
	FIntPoint GetGridCellFromLocation(const FVector& Location) const;
};
