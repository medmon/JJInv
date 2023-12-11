// Copyright © Jodiah Jensen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InventoryComponent.h"
#include "Data/ItemDataStructs.h"

#include "ItemBase.generated.h"

class AJJInvCharacter;
/**
 * 
 */
UCLASS()
class JJINV_API UItemBase : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	UInventoryComponent* OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category="Item")
	int32 Quantity;
	
	UPROPERTY(EditAnywhere, Category="Item")
	FName ItemID;
	
	UPROPERTY(EditAnywhere, Category="Item")
	EItemType ItemType;
	
	UPROPERTY(EditAnywhere, Category="Item")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category="Item")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category="Item")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category="Item")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category="Item")
	FItemAssetData ItemAssetData;

	bool bIsCopy;
	bool bIsPickup;
	
	
	//========================================================================================
	//		Functions
	//========================================================================================
	//constructor
	UItemBase();

	void ResetItemFlags();
	
	UFUNCTION(Category="Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * ItemNumericData.Weight; }
	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemSingleWeight() const { return ItemNumericData.Weight; }
	UFUNCTION(Category="Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == ItemNumericData.MaxStackSize; }

	UFUNCTION(Category="Item")
	void SetQuantity( const int32 NewQuantity);

	UFUNCTION(Category="Item")
	virtual void Use(AJJInvCharacter* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ItemID == OtherID;
	}
};
