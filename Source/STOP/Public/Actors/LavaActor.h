
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LavaActor.generated.h"

class UBoxComponent;

UCLASS()
class STOP_API ALavaActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALavaActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnLavaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lava")
	UBoxComponent* LavaTrigger;
};
