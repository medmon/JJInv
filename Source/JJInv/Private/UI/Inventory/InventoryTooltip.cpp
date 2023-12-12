// Copyright © Jodiah Jensen. All Rights Reserved.


#include "UI/Inventory/InventoryTooltip.h"

#include "Components/TextBlock.h"
#include "UI/Inventory/InventoryItemSlot.h"
#include "Items/ItemBase.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	switch (ItemBeingHovered->ItemType) {
	case EItemType::Junk:
		ItemType->SetText(FText::FromString("Junk"));
		break;
	case EItemType::Tool:
		ItemType->SetText(FText::FromString("Tool"));
		break;
	case EItemType::Armor:
		ItemType->SetText(FText::FromString("Armor"));
		break;
	case EItemType::Weapon:
		ItemType->SetText(FText::FromString("Weapon"));
		DamageValue->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Shield:
		ItemType->SetText(FText::FromString("Shield"));
		break;
	case EItemType::Spell:
		ItemType->SetText(FText::FromString("Spell"));
		break;
	case EItemType::Ammo:
		ItemType->SetText(FText::FromString("Ammo"));
		break;
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString("Consumable"));
		
		break;
	case EItemType::Quest:
		ItemType->SetText(FText::FromString("Quest"));
		break;
	case EItemType::Mundane:
		ItemType->SetText(FText::FromString("Mundane"));
		break;
	default: ;
	}

	ItemName->SetText(ItemBeingHovered->ItemTextData.Name);
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
	ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
	UsageText->SetText(ItemBeingHovered->ItemTextData.UsageText);
	SellValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.SellValue));
	ItemDescription->SetText(ItemBeingHovered->ItemTextData.Description);
	StackWeightValue->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));

	if (ItemBeingHovered->ItemNumericData.bIsStackable)
	{
		StackSizeText->SetText(FText::AsNumber(ItemBeingHovered->ItemNumericData.MaxStackSize));

	}
	else
	{
		StackSizeText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
