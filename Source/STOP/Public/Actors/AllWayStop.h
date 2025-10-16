
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AllWayStop.generated.h"
class APassengerCharacterBase;
class UBoxComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogAllWayStop, Log, All);

struct FSpawnableCharacterEntry;
class AExitDoorActor;
class APathActor;

UCLASS()
class STOP_API AAllWayStop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAllWayStop();
	
	void Register(APassengerCharacterBase* Passenger);
	void Unregister(APassengerCharacterBase* Passenger);
protected:
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* QueueZone;
	//
	// // signals I'm out
	// UPROPERTY(VisibleAnywhere)
	// UBoxComponent* ExitZone;

	// FIFO
	UPROPERTY()
	TArray<TWeakObjectPtr<APassengerCharacterBase>> Queue;

	// Who currently owns the token (is allowed to proceed)
	TWeakObjectPtr<APassengerCharacterBase> Current;

	UFUNCTION()
	void OnQueueBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
					  int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnQueueEnd(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
					int32 BodyIndex);

	// UFUNCTION()
	// void OnExitBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
	// 				 int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);

	void TryAdvance();                // Give token to head of queue
	void CleanupDeadEntries();        // Remove null/destroyed entries
	void Grant(APassengerCharacterBase* Passenger); // grant the token to the passenger
	void Revoke(APassengerCharacterBase* Passenger); 
	
};
