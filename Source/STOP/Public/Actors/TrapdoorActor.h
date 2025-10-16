
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HoldableInterface.h"
#include "TrapdoorActor.generated.h"
class UBoxComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogTrapdoor, Log, All);

UCLASS()
class STOP_API ATrapdoorActor : public AActor, public IHoldableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrapdoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void RebuildNavMesh();

	bool bIsActive = true;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh Settings", meta=(ToolTip="The mesh that will be used for the trapdoor"))
	UStaticMeshComponent* TrapdoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger Settings", meta=(ToolTip="The trigger box that will be used to detect player interaction"))
	UBoxComponent* TapTrigger;
	
	UFUNCTION(BlueprintCallable, Category="Trapdoor")
	void SetActiveTrap(bool bActive);

	UFUNCTION(BlueprintCallable, Category="Trapdoor")
	void ToggleTrap();

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* CloseSoundCue;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* OpenSoundCue;

	// INTERFACE IMPLEMENTATIONS =========================================

	// IHoldableInterface implementation ---------------------
	void OnHoldStart_Implementation(APlayerController* Caller) override; // Blueprint Native Event override
	virtual void OnHoldStartPure(APlayerController* Caller) override;
	void OnHoldEnd_Implementation(APlayerController* Caller) override; // Blueprint Native Event override
	virtual void OnHoldEndPure(APlayerController* Caller) override;
	
};

