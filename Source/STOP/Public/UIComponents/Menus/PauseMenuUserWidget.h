
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PauseMenuUserWidget.generated.h"

class UButtonUW;
class UTextBlock;
/**
 * 
 */
UCLASS()
class STOP_API UPauseMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	
	UFUNCTION(BlueprintCallable, Category="Pause Menu")
	void QuitToMainMenu();

	UFUNCTION(BlueprintCallable, Category="Pause Menu")
	void Resume();

protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	UButtonUW* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButtonUW* QuitToMainMenuButton;
	
};
