
#include "UIComponents/Menus/SettingsUW.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Gameplay/MainMenuPlayerController.h"
#include "UIComponents/Elements/ButtonUW.h"

void USettingsUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &USettingsUW::HandleReset);
		ResetButton->SetButtonText(FText::FromString("Reset to Default"));
	}

	if (SfxVolumeSlider)
	{
		SfxVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsUW::HandleSfxVolumeChanged);
		SfxVolumeSlider->SetValue(0.5f);
	}

	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsUW::HandleMusicVolumeChanged);
		MusicVolumeSlider->SetValue(0.5f);
	}

	if (BrightnessSlider)
	{
		BrightnessSlider->OnValueChanged.AddDynamic(this, &USettingsUW::HandleBrightnessChanged);
		BrightnessSlider->SetValue(0.5f);
	}
}

void USettingsUW::HandleReset()
{
	if (SfxVolumeSlider)
	{
		SfxVolumeSlider->SetValue(0.5f);
	}
	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->SetValue(0.5f);
	}
	if (BrightnessSlider)
	{
		BrightnessSlider->SetValue(0.5f);
	}
}

void USettingsUW::HandleSfxVolumeChanged(float Value)
{
	// TODO: Implement SFX volume change logic
}

void USettingsUW::HandleMusicVolumeChanged(float Value)
{
}

void USettingsUW::HandleBrightnessChanged(float Value)
{
}
