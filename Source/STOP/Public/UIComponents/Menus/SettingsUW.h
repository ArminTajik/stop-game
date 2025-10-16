
#pragma once

#include "CoreMinimal.h"
#include "MenuScreenUW.h"
#include "SettingsUW.generated.h"

class UButton;
class USlider;
/**
 * 
 */
UCLASS()
class STOP_API USettingsUW : public UMenuScreenUW
{
	GENERATED_BODY()

protected:
	// UI Elements ==========================================================================

	UPROPERTY(meta=(BindWidget))
	UButtonUW* ResetButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	USlider* SfxVolumeSlider = nullptr;

	UPROPERTY(meta=(BindWidget))
	USlider* MusicVolumeSlider = nullptr;

	UPROPERTY(meta=(BindWidget))
	USlider* BrightnessSlider = nullptr;


	// Functions ==========================================================================
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleReset();

	UFUNCTION()
	void HandleSfxVolumeChanged(float Value);

	UFUNCTION()
	void HandleMusicVolumeChanged(float Value);

	UFUNCTION()
	void HandleBrightnessChanged(float Value);
};
