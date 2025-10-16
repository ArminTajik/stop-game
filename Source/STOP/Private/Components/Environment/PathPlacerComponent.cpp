#include "PathPlacerComponent.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

UPathPlacerComponent::UPathPlacerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    CreateSubComponents();
}

void UPathPlacerComponent::CreateSubComponents()
{
    if (!SplineComponent)
    {
        SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
        SplineComponent->SetupAttachment(this);
        SplineComponent->bEditableWhenInherited = true;
    }

    if (!InstancedMeshComponent)
    {
        InstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
        InstancedMeshComponent->SetupAttachment(SplineComponent);
        InstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void UPathPlacerComponent::OnRegister()
{
    Super::OnRegister();
    GenerateInstances(); // Auto-generate at runtime
}

void UPathPlacerComponent::OnComponentCreated()
{
    Super::OnComponentCreated();
    GenerateInstances();
}

void UPathPlacerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);
    if (InstancedMeshComponent)
    {
        InstancedMeshComponent->ClearInstances();
    }
}

void UPathPlacerComponent::GenerateInstances()
{
    if (!SplineComponent || !InstancedMeshComponent || !MeshToUse) return;

    InstancedMeshComponent->ClearInstances();
    InstancedMeshComponent->SetStaticMesh(MeshToUse);
    if (MaterialToUse)
    {
        InstancedMeshComponent->SetMaterial(0, MaterialToUse);
    }

    const float SplineLength = SplineComponent->GetSplineLength();
    const int NumInstances = FMath::FloorToInt(SplineLength / Spacing);

    for (int i = 0; i <= NumInstances; ++i)
    {
        const float Distance = i * Spacing;
        FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local) + InstanceOffset;
        FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
        FTransform Transform(Rotation, Location, InstanceScale);
        InstancedMeshComponent->AddInstance(Transform);
    }
}

#if WITH_EDITOR
void UPathPlacerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    GenerateInstances();
}
#endif