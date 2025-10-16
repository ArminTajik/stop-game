
#pragma once

#include "CoreMinimal.h"
#include "CardUW.h"
#include "StoryCardUW.generated.h"

class URichTextBlock;
class UStoryData;
class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class STOP_API UStoryCardUW : public UCardUW
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn=true))
	TObjectPtr<UStoryData> Info;

	UFUNCTION(BlueprintCallable)
	void InitFromInfo(UStoryData* InInfo);

	virtual void SetSelected(bool bIsSelected) override;
	UFUNCTION(BlueprintCallable)
	UStoryData* GetStoryInfo() const { return Info; }

protected:
	// UI Elements ==========================================================================
	UPROPERTY(meta=(BindWidget))
	UImage* Bg_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;

	UPROPERTY(meta = (BindWidget))
	UImage* ThumbnailImage;
	

	// Functions ==========================================================================
	
	void ApplyLockedVisuals(bool bLocked);
};
