
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarUW.generated.h"


class UHorizontalBox;

/**
 * 
 */
UCLASS()
class STOP_API UHealthBarUW : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SetHealth(int32 NewHealth);

	UPROPERTY(BlueprintReadWrite, Category="Health")
	int32 MaxHealth = 5;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HealthBarBox = nullptr;

	UPROPERTY(EditAnywhere, Category="Health")
	TSubclassOf<UUserWidget> HealthBarWidgetClass = nullptr;

	
};
