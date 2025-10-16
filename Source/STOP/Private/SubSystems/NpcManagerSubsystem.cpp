#include "Subsystems/NpcManagerSubsystem.h"

#include "Actors/EnterDoorActor.h"
#include "Characters/PassengerCharacterBase.h"

void UNpcManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("NPCManagerSubsystem initialized."));
}

void UNpcManagerSubsystem::Deinitialize()
{
	RegisteredNPCs.Empty();
	SpatialGrid.Empty();
	Super::Deinitialize();
}

void UNpcManagerSubsystem::RegisterNPC(APassengerCharacterBase* NPC)
{
	if (IsValid(NPC))
	{
		UE_LOG(LogTemp, Log, TEXT("Registering NPC: %s"), *NPC->GetName());
		RegisteredNPCs.Add(NPC);
	}
}

void UNpcManagerSubsystem::UnregisterNPC(APassengerCharacterBase* NPC)
{
	if (IsValid(NPC))
	{
		UE_LOG(LogTemp, Log, TEXT("Unregistering NPC: %s"), *NPC->GetName());
		RegisteredNPCs.Remove(NPC);
	}
}

void UNpcManagerSubsystem::RegisterSpawner(int32 SpawnerId)
{
	if (!RegisteredSpawnerIds.Contains(SpawnerId))
	{
		RegisteredSpawnerIds.Add(SpawnerId);
		RegisteredSpawnerIds.Sort(); // optional: enforce deterministic A<B<C order
		TurnIndex = FMath::Clamp(TurnIndex, 0, RegisteredSpawnerIds.Num()-1);
		UE_LOG(LogTemp, Log, TEXT("Spawner %d registered. RR order: %s"), SpawnerId, *FString::JoinBy(RegisteredSpawnerIds, TEXT(","), [](int32 v){return FString::FromInt(v);} ));
	}
}

void UNpcManagerSubsystem::UnregisterSpawner(int32 SpawnerId)
{
	int32 RemovedIndex = RegisteredSpawnerIds.Find(SpawnerId);
	if (RemovedIndex != INDEX_NONE)
	{
		RegisteredSpawnerIds.RemoveAt(RemovedIndex);
		if (RegisteredSpawnerIds.Num() == 0) { TurnIndex = 0; }
		else { TurnIndex %= RegisteredSpawnerIds.Num(); }
	}
}

FIntPoint UNpcManagerSubsystem::GetGridCellFromLocation(const FVector& Location) const
{
	return FIntPoint(
		FMath::FloorToInt(Location.X / CellSize),
		FMath::FloorToInt(Location.Y / CellSize)
	);
}

void UNpcManagerSubsystem::RebuildSpatialGrid()
{
	SpatialGrid.Empty();

	for (APassengerCharacterBase* NPC : RegisteredNPCs)
	{
		if (!IsValid(NPC)) continue;
		FIntPoint Cell = GetGridCellFromLocation(NPC->GetActorLocation());
		SpatialGrid.FindOrAdd(Cell).Add(NPC);
	}
}

void UNpcManagerSubsystem::AdvanceTurn()
{
	if (RegisteredSpawnerIds.Num() == 0) return;
	TurnIndex = (TurnIndex + 1) % RegisteredSpawnerIds.Num();
	UE_LOG(LogTemp, Verbose, TEXT("Round-robin advanced. Now spawner %d"), GetCurrentTurnSpawner());
}

TArray<APassengerCharacterBase*> UNpcManagerSubsystem::GetNearbyNPCs(const FVector& Location, float Radius)
{
	RebuildSpatialGrid();

	TArray<APassengerCharacterBase*> Nearby;
	const FIntPoint Center = GetGridCellFromLocation(Location);
	const int32 Range = FMath::CeilToInt(Radius / CellSize);

	for (int32 DX = -Range; DX <= Range; ++DX)
	{
		for (int32 Dy = -Range; Dy <= Range; ++Dy)
		{
			if (const FIntPoint Cell = Center + FIntPoint(DX, Dy); SpatialGrid.Contains(Cell))
			{
				Nearby.Append(SpatialGrid[Cell]);
			}
		}
	}

	return Nearby;
}