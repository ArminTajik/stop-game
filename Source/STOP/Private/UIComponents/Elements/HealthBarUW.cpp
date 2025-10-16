

#include "UIComponents/Elements/HealthBarUW.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"


void UHealthBarUW::SetHealth(int32 NewHealth)
{
	if (!HealthBarBox) return;

	// Clamp the new health value between 0 and MaxHealth
	const int32 ClampedHealth = FMath::Clamp(NewHealth, 0, MaxHealth);

	// Update the visibility of each health widget based on the current health
	for (int32 i = 0; i < HealthBarBox->GetChildrenCount(); ++i)
	{
		if (UWidget* ChildWidget = HealthBarBox->GetChildAt(i))
		{
			ChildWidget->SetVisibility(i < ClampedHealth ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}
	}
}

void UHealthBarUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}


void UHealthBarUW::NativeConstruct()
{
	
	if (!HealthBarBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthBarBox is not bound!"));
		return;
	}

	// HealthBarBox->ClearChildren();
	// UUserWidget* HealthWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarWidgetClass);
	//
	// if (!HealthWidget)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("HealthBarWidgetClass is not set or failed to create widget!"));
	// 	return;
	// }
	// // Instantiate the health bar widget if the class is set
	// if (HealthBarWidgetClass)
	// {
	// 	for (int32 i = 0; i < MaxHealth; ++i)
	// 	{
	// 		if (UHorizontalBoxSlot* NewSlot = Cast<UHorizontalBoxSlot>(HealthBarBox->AddChild(HealthWidget)))
	// 		{
	// 			NewSlot->SetPadding(FMargin(-16.0f, 0.0f));
	// 		}
	// 	}
	// }
}