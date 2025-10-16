
#pragma once
#include "CoreMinimal.h"
#include "Characters/PassengerCharacterBase.h"
#include "GameFramework/Actor.h"
#include "ScorePopUpActor.generated.h"


class UWidgetComponent;

UCLASS()
class STOP_API AScorePopupActor : public AActor
{
	GENERATED_BODY()

public:
	AScorePopupActor();

	void Init(FText Text, float Duration = 1.0f, FColor TextColor = FColor::White);

protected:
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> WidgetClass;

private:
	FTimerHandle DestroyTimerHandle;

	UFUNCTION()
	void DestroyPopup();
};
