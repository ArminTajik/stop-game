

#include "UIComponents/Elements/ButtonUW.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UButtonUW::HandleClick()
{
	OnClicked.Broadcast();
}

void UButtonUW::HandlePress()
{
	OnPressed.Broadcast();
}

void UButtonUW::HandleRelease()
{
	OnReleased.Broadcast();
}

void UButtonUW::SetButtonText(const FText& NewText) const
{
	if (ButtonText)
	{
		ButtonText->SetText(NewText);
	}
}

void UButtonUW::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UButtonUW::HandleClick);
		Button->OnPressed.AddDynamic(this, &UButtonUW::HandlePress);
		Button->OnReleased.AddDynamic(this, &UButtonUW::HandleRelease);
	}
}
