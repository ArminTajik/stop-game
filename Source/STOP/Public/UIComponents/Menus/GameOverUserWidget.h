
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUserWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class STOP_API UGameOverUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="GameOver")
	void GameOver(FLevelData LevelData);

	UFUNCTION(BlueprintCallable, Category="GameOver")
	void QuitToMainMenu();

	UFUNCTION(BlueprintCallable, Category="GameOver")
	void RestartLevel();

	UFUNCTION(BlueprintCallable, Category="GameOver")
	void NextLevel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameOver")
	FString MainMenuLevelName = "MainMenu";

protected:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WinText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LoseText;

	UPROPERTY(meta=(BindWidget))
	UButton* RestartButton;

	UPROPERTY(meta=(BindWidget))
	UButton* NextLevelButton;

	UPROPERTY(meta=(BindWidget))
	UButton* QuitToMainMenuButton;
	
};
