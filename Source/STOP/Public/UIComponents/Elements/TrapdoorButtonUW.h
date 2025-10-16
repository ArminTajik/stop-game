
#pragma once

#include "CoreMinimal.h"
#include "ButtonUW.h"
#include "TrapdoorButtonUW.generated.h"


class ATrapdoorActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrapdoorButtonEvent, ATrapdoorActor*, TrapdoorActor);

/**
 * The TrapdoorButtonUW class extends UButtonUW to handle interactions with a trapdoor actor.
 **/
UCLASS()
class STOP_API UTrapdoorButtonUW : public UButtonUW
{
	GENERATED_BODY()

public:
	virtual void HandleClick() override;

	virtual void HandlePress() override;

	virtual void HandleRelease() override;

	UPROPERTY(BlueprintReadOnly, Category="Trapdoor")
	ATrapdoorActor* TrapdoorActor;

	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnTrapdoorButtonEvent OnTrapdoorButtonClicked;

	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnTrapdoorButtonEvent OnTrapdoorButtonPressed;

	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnTrapdoorButtonEvent OnTrapdoorButtonReleased;

	// protected:
	// 	virtual void NativeOnInitialized() override;
};
