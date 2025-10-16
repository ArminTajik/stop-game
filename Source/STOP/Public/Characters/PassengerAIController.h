
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PassengerAIController.generated.h"
class APassengerCharacterBase;
DECLARE_LOG_CATEGORY_EXTERN(LogPassengerAIController, Log, All);

UCLASS()
class STOP_API APassengerAIController : public AAIController
{
	GENERATED_BODY()

public:
	APassengerAIController();
	void SetDestination(const FVector& Destination);
	void StartMove();
	void StopMove();
	
	void OnStopAtAllWay(APassengerCharacterBase* ThisPassenger);
	void OnAllWayStopMyTurn(APassengerCharacterBase* ThisPassenger);
	void OnExitAllWay(const APassengerCharacterBase* ThisPassenger);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	// virtual void Tick(float DeltaTime) override;


private:
	FVector Destination;
	bool bIsStopped = false;
	FTimerHandle SpacingCheckTimerHandle;
	FTimerHandle StartMoveTimerHandle;
	void YieldForOther(APassengerCharacterBase* Other, APassengerCharacterBase* ThisPassenger);
	void MoveAhead(APassengerCharacterBase* ThisPassenger = nullptr);
	void CheckSpacingAndAct();
};
