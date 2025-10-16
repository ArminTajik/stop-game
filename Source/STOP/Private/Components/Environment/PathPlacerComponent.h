#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PathPlacerComponent.generated.h"

class USplineComponent;
class UInstancedStaticMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STOP_API UPathPlacerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPathPlacerComponent();

	// Editable config
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer")
	UStaticMesh* MeshToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer")
	UMaterialInterface* MaterialToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer", meta = (ClampMin = 1.0))
	float Spacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer")
	FVector InstanceOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer")
	FRotator InstanceRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path Placer")
	FVector InstanceScale = FVector(1.0f);

	// Called to regenerate instances manually
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Path Placer")
	void GenerateInstances();

protected:
	virtual void OnRegister() override;
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	// Editor-visible subcomponents
	UPROPERTY(VisibleAnywhere, Category = "Path Placer")
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, Category = "Path Placer")
	UInstancedStaticMeshComponent* InstancedMeshComponent;

	void CreateSubComponents();
};