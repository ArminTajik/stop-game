

#include "UIComponents/Menus/MainMenuDevUW.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "DataAssets/LevelListDataAsset.h"
#include "DataPersistance/DataPersistenceManager.h"
#include "Gameplay/NoNameGameInstance.h"


void UMainMenuDevUW::NativeConstruct()
{
	Super::NativeConstruct();
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuDevUW::OnPlayButtonClicked);
	}
	PopulateGameRulesDropdown();
	PopulateLevelDropdown();
	UpdateStats();
}

void UMainMenuDevUW::PopulateGameRulesDropdown()
{
	if (GameRulesDropdown && GameRulesList && GameRulesList->GameRules.Num() > 0)
	{
		GameRulesDropdown->ClearOptions();
		int32 NumLevelToShow = GameRulesList->GameRules.Num();

		if (const UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>())
		{
			const int32 MaxLevel = DPM->GetHighestUnlockedLevel();
			NumLevelToShow = FMath::Min(MaxLevel, NumLevelToShow);
		}

		for (int i = 0; i < NumLevelToShow; ++i)
		{
			const auto& GameRules = GameRulesList->GameRules[i];
			if (GameRules && GameRules->Name.IsEmpty() == false)
			{
				GameRulesDropdown->AddOption(GameRules->Name.ToString());
				GameRulesDropdown->SetToolTipText(GameRules->Description);
			}
		}
		GameRulesDropdown->OnSelectionChanged.AddDynamic(this, &UMainMenuDevUW::HandleGameRulesChanged);
		SelectGameRulesDropdownItem(0); // Select the first item by default
	}
}

void UMainMenuDevUW::PopulateLevelDropdown()
{
	if (LevelDropdown && LevelList && LevelList->Levels.Num() > 0)
	{
		LevelDropdown->ClearOptions();
		for (const auto& Level : LevelList->Levels)
		{
			if (Level && Level->DisplayName.IsEmpty() == false)
			{
				LevelDropdown->AddOption(Level->DisplayName.ToString());
				LevelDropdown->SetToolTipText(Level->Description);
			}
		}
		LevelDropdown->OnSelectionChanged.AddDynamic(this, &UMainMenuDevUW::HandleLevelChanged);
		SelectLevelDropdownItem(0); // Select the first item by default
	}
}

void UMainMenuDevUW::UpdateStats() const
{
	int32 HighScore = 0;
	int32 HighestUnlockedLevel = 1;
	if (const UDataPersistenceManager* DPM = GetGameInstance()->GetSubsystem<UDataPersistenceManager>())

	{
		HighScore = DPM->GetHighestScore();
		HighestUnlockedLevel = DPM->GetHighestUnlockedLevel();
	}
	HighScoreText->SetText(FText::FromString(FString::Printf(TEXT("High Score: %d"), HighScore)));
	HighestLevelText->SetText(
		FText::FromString(FString::Printf(TEXT("Highest Unlocked Level: %d"), HighestUnlockedLevel)));
}


void UMainMenuDevUW::HandleGameRulesChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!GameRulesList || GameRulesList->GameRules.Num() == 0)
		return;

	for (const auto& GameRules : GameRulesList->GameRules)
	{
		if (GameRules && GameRules->Name.ToString() == SelectedItem)
		{
			AssignGameRules(GameRules);
			break;
		}
	}
}

bool UMainMenuDevUW::TryAssignLevel(ULevelDataAsset* const& Level, int32 LevelNum)
{
	if (!Level)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected level is null!"));
		return false;
	}

	SelectedLevel = Level;

	if (LevelDescription && !Level->Description.IsEmpty())
	{
		LevelDescription->SetText(Level->Description);
	}

	if (GameRulesDropdown && GameRulesList && GameRulesList->GameRules.Num() > 0)
	{
		for (int i = 0; i < GameRulesList->GameRules.Num(); ++i)
		{
			if (GameRulesList->GameRules[i] && GameRulesList->GameRules[i] == Level->GameRules)
			{
				SelectGameRulesDropdownItem(i);
				break;
			}
		}
	}

	return true;
}

bool UMainMenuDevUW::AssignGameRules(UGameRulesDataAsset* const& GameRules)
{
	if (!GameRules || GameRules->Name.IsEmpty())
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Selected game mode is null or has an empty display name!"));
		return false;
	}

	SelectedGameRules = GameRules;

	if (GameRulesDescription)
	{
		GameRulesDescription->SetText(GameRules->Description);
	}
	return true;
}

void UMainMenuDevUW::HandleLevelChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!LevelList || LevelList->Levels.Num() == 0)
		return;
	for (const auto& Level : LevelList->Levels)
	{
		if (Level && Level->DisplayName.ToString() == SelectedItem)
		{
			if (!TryAssignLevel(Level, LevelList->Levels.IndexOfByKey(Level))) return;
			break;
		}
	}

	// UE_LOG(LogTemp, Log, TEXT("Level Selected: %s"), *LevelNameToLoad);
}

void UMainMenuDevUW::OnPlayButtonClicked()
{
	if (!SelectedLevel)
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("No level selected!"));
		return;
	}

	if (UNoNameGameInstance* GI = GetGameInstance<UNoNameGameInstance>())
	{
		GI->SetSelectedGameRules(SelectedGameRules);
		GI->SetSelectedLevel(SelectedLevel);
		GI->StartGame();
	}
	else
	{
		UE_LOG(LogMainMenuUserWidget, Warning, TEXT("Failed to get Game Instance!"));
	}
}

void UMainMenuDevUW::SelectGameRulesDropdownItem(int Index)
{
	if (!GameRulesDropdown || !GameRulesList || GameRulesList->GameRules.Num() <= Index)
		return;

	// Set the default selection to the first item in the list
	GameRulesDropdown->ClearSelection();
	GameRulesDropdown->SetSelectedIndex(Index);

	AssignGameRules(GameRulesList->GameRules[Index]);
}


void UMainMenuDevUW::SelectLevelDropdownItem(int Index)
{
	if (!LevelDropdown || !LevelList || LevelList->Levels.Num() == 0)
		return;

	// Set the default selection to the first item in the list
	LevelDropdown->ClearSelection();
	LevelDropdown->SetSelectedIndex(Index);
	TryAssignLevel(LevelList->Levels[Index], Index);
}
