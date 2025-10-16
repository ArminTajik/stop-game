
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ScannerActor.generated.h"

class USpotLightComponent;

UCLASS()
class STOP_API AScannerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScannerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnStartScanOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndScanOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                      int32 OtherBodyIndex);

	

public:	

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ScanTrigger;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* ScannerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Light")
	USpotLightComponent* ScannerLight;

	UFUNCTION(BlueprintCallable, Category="Scanner")
	void SetLightColor(FLinearColor LinearColor) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	bool bIsScanning = false;
};
