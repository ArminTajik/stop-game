
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "DataAssets/GameRulesDataAsset.h"
#include "HUDUserWidget.generated.h"

class UTrapdoorButtonUW;
class UButton;
class UStatsItemUW;
class UHealthBarUW;
class UTextBlock;
/**
 * 
 */
UCLASS()
class STOP_API UHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateScore(int32 NewScore) const;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateHealth(int32 NewHealth);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateTime(float NewTime) const;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void AddTrapdoorButton(ATrapdoorActor* TrapdoorActor);

	UFUNCTION()
	void OnPauseMenuButtonClicked();

	UFUNCTION()
	void OnTrapdoorButtonPressed(ATrapdoorActor* TrapdoorActor);

	UFUNCTION()
	void OnTrapdoorButtonReleased(ATrapdoorActor* TrapdoorActor);

	UFUNCTION(BlueprintCallable, Category="Debug")
	void AddToDebugText(const FString& String);

	UFUNCTION(BlueprintCallable, Category="Debug")
	void ClearLog();

	UFUNCTION()
	void OnSpecialMoveButtonClicked();
	
	UFUNCTION(BlueprintCallable, Category="Special Move")
	void SetPowerUp(EPowerUp MoveType);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTrapdoorButtonUW> TrapdoorButtonClass;
	
	UPROPERTY(meta = (BindWidget))
	UStatsItemUW* ScoreStat;

	UPROPERTY(meta = (BindWidget))
	UHealthBarUW* HealthBarWidget;

	UPROPERTY(meta=(BindWidget))
	UStatsItemUW* TimeStat;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LogText;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TrapdoorButtonBox;

	UPROPERTY(meta = (BindWidget))
	UButton* PauseMenuButton;

	UPROPERTY(meta=(BindWidget))
	UButton* PowerUpButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PowerUpText;

	UPROPERTY()
	EPowerUp CurrentSpecialMove = EPowerUp::None;

};
