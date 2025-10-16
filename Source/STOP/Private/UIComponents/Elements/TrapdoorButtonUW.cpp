

#include "UIComponents/Elements/TrapdoorButtonUW.h"

void UTrapdoorButtonUW::HandleClick()
{
	Super::HandleClick();
	OnTrapdoorButtonClicked.Broadcast(TrapdoorActor);
}

void UTrapdoorButtonUW::HandlePress()
{
	Super::HandlePress();
	OnTrapdoorButtonPressed.Broadcast(TrapdoorActor);
}

void UTrapdoorButtonUW::HandleRelease()
{
	Super::HandleRelease();
	OnTrapdoorButtonReleased.Broadcast(TrapdoorActor);
}
