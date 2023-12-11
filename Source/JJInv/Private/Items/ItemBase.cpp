// Copyright © Jodiah Jensen. All rights reserved.


#include "Items/ItemBase.h"

#include "Components/InventoryComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ItemID = this->ItemID;
	ItemCopy->SetQuantity(this->Quantity);
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->ItemTextData = this->ItemTextData;
	ItemCopy->ItemNumericData = this->ItemNumericData;
	ItemCopy->ItemAssetData = this->ItemAssetData;
	ItemCopy->bIsCopy = true;
	
	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = NewQuantity;

		//if we want to clamp quantity changes to MaxStackSize, do it here
		// Quantity = FMath::Clamp(
		// 	NewQuantity,
		// 	0,
		// 	ItemNumericData.bIsStackable ? ItemNumericData.MaxStackSize : 1);
		//
		
		if (OwningInventory)
		{
				if (Quantity <= 0)
				{
					OwningInventory->RemoveSingleInstanceOfItem(this);
				}
		}
		
	}
}

void UItemBase::Use(AJJInvCharacter* Character)
{
	
}
