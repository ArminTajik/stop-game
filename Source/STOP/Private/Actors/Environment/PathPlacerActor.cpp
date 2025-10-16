#include "Actors/Environment/PathPlacerActor.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
APathPlacerActor::APathPlacerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	InstancedMeshComponent->SetupAttachment(RootComponent);
}


void APathPlacerActor::OnConstruction(const FTransform& Transform)
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
		float Distance = i * Spacing;
		FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local) +
			InstanceOffset;
		FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local) +
			InstanceRotationOffset;

		FTransform InstanceTransform(Rotation, Location, FVector(InstanceScale));
		InstancedMeshComponent->AddInstance(InstanceTransform);
	}
}
