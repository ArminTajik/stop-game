
#pragma once

#include "CoreMinimal.h"
#include "MainMenuUserWidget.h"
#include "DataAssets/GameRulesDataAsset.h"
#include "DataAssets/GameRulesListDataAsset.h"
#include "DataAssets/LevelDataAsset.h"
#include "MainMenuDevUW.generated.h"

class ULevelListDataAsset;
class UGameModeListDataAsset;
class UTextBlock;
class UComboBoxString;
/**
 * 
 */
UCLASS()
class STOP_API UMainMenuDevUW : public UMainMenuUserWidget
{
	GENERATED_BODY()

protected:
	// UI Elements ==========================================================================
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HighScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HighestLevelText;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	// Dropdown for selecting level
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* LevelDropdown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Data Assets")
	ULevelListDataAsset* LevelList;

	// Dropdown for selecting game mode
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* GameRulesDropdown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameRulesDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Data Assets", meta = (AllowPrivateAccess = "true"))
	UGameRulesListDataAsset* GameRulesList;

	UPROPERTY()
	UGameRulesDataAsset* SelectedGameRules;

	UPROPERTY()
	ULevelDataAsset* SelectedLevel;
	// Functions ==========================================================================
	virtual void NativeConstruct() override;

	void SelectGameRulesDropdownItem(int Index);
	void SelectLevelDropdownItem(int Index);
	void PopulateGameRulesDropdown();
	void PopulateLevelDropdown();
	void UpdateStats() const;
	bool TryAssignLevel(ULevelDataAsset* const& Level, int32 LevelNum);
	bool AssignGameRules(UGameRulesDataAsset* const& GameRules);

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void HandleGameRulesChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleLevelChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};
