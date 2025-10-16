
#pragma once

#include "CoreMinimal.h"
#include "MenuScreenUW.h"
#include "StoryUW.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStoryMenu, Log, All);

class UCardUW;
class UStoryData;
class UStoryDataList;
class UStoryCardUW;
class UButton;
/**
 * 
 */
UCLASS()
class STOP_API UStoryUW : public UMenuScreenUW
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStoryCardUW> StoryCardClass;

protected:
	// UI Elements ==========================================================================
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* StoryBox = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButtonUW* BuyButton = nullptr;

	UPROPERTY()
	TObjectPtr<UStoryCardUW> SelectedCard;

	UPROPERTY(EditDefaultsOnly, Category = "Story Assets")
	UStoryDataList* StoryList;


	// Functions ==========================================================================
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;


	void BuildCards();
	void CacheUniformMetrics();

	UFUNCTION()
	void HandleCardSelected(UCardUW* Card);

	UFUNCTION()
	void HandleBuy();
	
	UFUNCTION()
	void HandleScroll(float CurrentOffset);

	UFUNCTION()
	void SnapToNearestCard(UCardUW* Card);
	
	int32 IndexFromScroll() const;

	UPROPERTY()
	TArray<UStoryCardUW*> Cards;

	bool  bIsSnapping = false;
	int32 LastSnappedIndex = INDEX_NONE;
	
	FTimerHandle SnapIdleTimer;
	float SnapIdleDelay = 0.12f;

	FTimerHandle SnapAnimGuardTimer;

	// Uniform layout params
	float ItemW   = 0.f;              
	float Gap     = 0.f;              
	float Pitch   = 0.f;              
	float FirstCenter = 0.f;

	UPROPERTY(EditAnywhere, Category="Layout")
	float DefaultPadding = 32.f;
};
