// Copyright © Jodiah Jensen. All Rights Reserved.


#include "UI/Interaction/InteractionWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interfaces/InteractionInterface.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressBar->PercentDelegate.BindUFunction(
		this,
		"UpdateInteractionProgress");

	
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KeyPressText->SetText(FText::FromString("Use"));
	CurrentInteractionDuration = 0.f;
	
}

void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData)
{
	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		ActionText->SetText(FText::FromString("examine"));
		
		KeyPressText->SetText(FText::FromString("Press"));

		InteractionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		if (InteractableData->Quantity == 1)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetText(
				FText::Format(NSLOCTEXT("InteractionWidget","QuantityText", "x{0}"),
					InteractableData->Quantity)
			);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		break;
	case EInteractableType::Converse:
		break;
	case EInteractableType::Activate:
		break;
	case EInteractableType::Operate:
		break;
	case EInteractableType::Open:
		break;
	case EInteractableType::Use:
		break;
	case EInteractableType::Examine:
		break;
	case EInteractableType::Harvest:
		break;
	case EInteractableType::Toggle:
		break;
	case EInteractableType::Click:
		break;
	case EInteractableType::Touch:
		break;
	case EInteractableType::Press:
		break;
	case EInteractableType::Turn:
		break;
	case EInteractableType::Pull:
		break;
	case EInteractableType::Twist:
		break;
	case EInteractableType::Push:
		break;
	case EInteractableType::Unlock:
		break;
	default: ;
	}

//	ActionText->SetText(InteractableData->Action);
	NameText->SetText(InteractableData->Name);
}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.f;
}
