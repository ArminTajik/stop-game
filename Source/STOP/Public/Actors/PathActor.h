
#pragma once

#include "CoreMinimal.h"
#include "ExitDoorActor.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PathActor.generated.h"


class AEnterDoorActor;
class ATrapdoorActor;

UCLASS()
class STOP_API APathActor : public AActor
{
	GENERATED_BODY()

public:
	APathActor();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Path")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Path")
	UInstancedStaticMeshComponent* InstancedMeshComponent;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ToolTip="The mesh that will be repeated along the path"))
	UStaticMesh* MeshToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ToolTip="The material that will be applied to the mesh"))
	UMaterialInstance* MaterialToUse;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ClampMin="10.0", UIMin="10.0"))
	float Spacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ToolTip="Offset applied to each mesh instance"))
	FVector InstanceOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ToolTip="Scale applied to each mesh instance", ClampMin="0.01", UIMin="0.01"))
	float InstanceScale = 1.0f;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Path Spawns", meta=(ToolTip="The trapdoor actor placed in the middle of the path"))
	TSubclassOf<ATrapdoorActor> TrapdoorActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Path Spawns", meta=(ToolTip="where the trapdoor will be placed"))
	int32 EmptyFrom = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Path Spawns", meta=(ToolTip="where the trapdoor will be placed"))
	int32 EmptyTo = -1;

private:
	
	AActor* SpawnActorAtDistance(const TSubclassOf<AActor>& ActorClass, float Distance, FName NameTag) const;
};
