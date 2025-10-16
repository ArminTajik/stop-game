#include "Actors/PathActor.h"
#include "EngineUtils.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
APathActor::APathActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(RootComponent);
}


void APathActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Clear previous instances
	InstancedMeshComponent->ClearInstances();

	if (!MeshToUse || SplineComponent->GetNumberOfSplinePoints() < 2)
		return;

	InstancedMeshComponent->SetStaticMesh(MeshToUse);
	InstancedMeshComponent->SetMaterial(0, MaterialToUse);

	const float SplineLength = SplineComponent->GetSplineLength();
	const int NumInstances = FMath::FloorToInt(SplineLength / Spacing);

	for (int i = 0; i <= NumInstances; ++i)
	{
		// Skip the last instance if it's the trapdoor
		if (i >= EmptyFrom && i <= EmptyTo)
		{
			continue;
		}

		float Distance = i * Spacing;
		FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local) +
			InstanceOffset;
		FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

		FTransform InstanceTransform(Rotation, Location, FVector(InstanceScale));
		InstancedMeshComponent->AddInstance(InstanceTransform);
	}
}


AActor* APathActor::SpawnActorAtDistance(const TSubclassOf<AActor>& ActorClass, float Distance, FName NameTag) const
{
	if (!ActorClass)
		return nullptr;

	const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	const FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	// Destroy existing actor with that tag to avoid duplicates
	for (TActorIterator<AActor> It(GetWorld(), ActorClass); It; ++It)
	{
		if (It->ActorHasTag(NameTag))
		{
			It->Destroy();
			break;
		}
	}

	FActorSpawnParameters Params;
	Params.Name = NameTag;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, Location, Rotation, Params);

	if (SpawnedActor)
		SpawnedActor->Tags.Add(NameTag);

	return SpawnedActor;
}
