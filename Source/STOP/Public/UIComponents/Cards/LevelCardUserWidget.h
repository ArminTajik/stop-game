
#pragma once

#include "CoreMinimal.h"
#include "CardUW.h"
#include "DataAssets/LevelDataAsset.h"
#include "LevelCardUserWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class STOP_API ULevelCardUserWidget : public UCardUW
{
	GENERATED_BODY()

public:
	// Delegate to notify selection


	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	TObjectPtr<ULevelDataAsset> LevelInfo;

	UPROPERTY(BlueprintReadOnly)
	float BestTimeSeconds = 0.f;

	UFUNCTION(BlueprintCallable)
	void InitFromInfo(ULevelDataAsset* InInfo, float InBestTime);

	virtual void SetSelected(bool bIsSelected) override;


protected:
	// UI Elements ==========================================================================
	UPROPERTY(meta=(BindWidget))
	UImage* Bg_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UImage* ThumbnailImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BestTimeText;


	// Functions ==========================================================================
	
	virtual void ApplyLockedVisuals(bool bLocked);

	
};
