
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ButtonUW.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonEvent);

class UButton;
/**
 * 
 */
UCLASS()
class STOP_API UButtonUW : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnButtonEvent OnClicked;

	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnButtonEvent OnPressed;

	UPROPERTY(BlueprintAssignable, Category="Button")
	FOnButtonEvent OnReleased;

	UPROPERTY(meta=(BindWidget))
	UButton* Button = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ButtonText = nullptr;
	
	UFUNCTION()
	virtual void HandleClick();

	UFUNCTION()
	virtual void HandlePress();

	UFUNCTION()
	virtual void HandleRelease();

	UFUNCTION()
	void SetButtonText(const FText& NewText) const;

protected:
	virtual void NativeOnInitialized() override;


	
};
