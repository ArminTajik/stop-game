
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardUW.generated.h"

class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class STOP_API UCardUW : public UUserWidget
{
	GENERATED_BODY()

public:
	// Delegate to notify selection
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelected, UCardUW*, Card);

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnCardSelected OnCardSelected;

	UFUNCTION()
	virtual void SetSelected(bool bIsSelected);

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	virtual void HandleClicked();

	// UI Elements ==========================================================================
	UPROPERTY(meta=(BindWidget))
	UButton* RootButton = nullptr;
};
