
#pragma once

#include "CoreMinimal.h"
#include "MenuScreenUW.h"
#include "SelectLevelUserWidget.generated.h"



class UCardUW;
class ULevelCardUserWidget;
DECLARE_LOG_CATEGORY_EXTERN(LogLevelSelectionMenu, Log, All);

class ULevelListDataAsset;
/**
 * 
 */
UCLASS()
class STOP_API USelectLevelUserWidget : public UMenuScreenUW
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULevelCardUserWidget> LevelCardClass;

protected:
	// UI Elements ==========================================================================
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* Grid_Levels = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButtonUW* StartButton = nullptr;

	UPROPERTY()
	TObjectPtr<ULevelCardUserWidget> SelectedCard;


	UPROPERTY(EditDefaultsOnly, Category = "Data Assets")
	ULevelListDataAsset* LevelList;

	// Functions ==========================================================================
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;


	void BuildCards();

	UFUNCTION()
	void HandleCardSelected(UCardUW* Card);

	UFUNCTION()
	void HandleStart();

private:
	int32 LevelNumToLoad = -1;
	FString LevelToLoad;
};
