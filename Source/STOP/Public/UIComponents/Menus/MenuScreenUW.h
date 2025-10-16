
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuScreenUW.generated.h"

class UButtonUW;
class UButton;
/**
 * 
 */
UCLASS()
class STOP_API UMenuScreenUW : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleBack();

	UPROPERTY(meta=(BindWidget))
	UButtonUW* BackButton = nullptr;
};
