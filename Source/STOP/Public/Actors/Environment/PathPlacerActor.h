
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PathPlacerActor.generated.h"


class AEnterDoorActor;
class ATrapdoorActor;

UCLASS()
class STOP_API APathPlacerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APathPlacerActor();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Path Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Path")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Path")
	UInstancedStaticMeshComponent* InstancedMeshComponent;

	// Mesh Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings",
		meta=(ToolTip="The mesh that will be repeated along the path"))
	UStaticMesh* MeshToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings",
		meta=(ToolTip="The material that will be applied to the mesh"))
	UMaterialInstance* MaterialToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ClampMin="10.0", UIMin="10.0"))
	float Spacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings",
		meta=(ToolTip="Offset applied to each mesh instance"))
	FVector InstanceOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings",
		meta=(ToolTip="Rotation applied to each mesh instance"))
	FRotator InstanceRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings",
		meta=(ToolTip="Scale applied to each mesh instance", ClampMin="0.01", UIMin="0.01"))
	float InstanceScale = 1.0f;

	
};
