// Copyright © Jodiah Jensen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
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

	//TODO: uncomment this once we create the UJJInvComponent class	
	//	UPROPERTY()
	//	UJJInvComponent* OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category="Item Data")
	int32 Quantity;
	
	UPROPERTY(EditAnywhere, Category="Item Data")
	FName ItemID;
	
	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemType ItemType;
	
	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemAssetData ItemAssetData;

	//========================================================================================
	//		Functions
	//========================================================================================
	//constructor
	UItemBase();

	UFUNCTION(Category="Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * ItemNumericData.Weight; }
	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemSingleWeight() const { return ItemNumericData.Weight; }
	UFUNCTION(Category="Item")
	FORCEINLINE bool bIsFullItemStack() const { return Quantity == ItemNumericData.MaxStackSize; }

	UFUNCTION(Category="Item")
	void SetQuantity( const int32 NewQuantity);

	UFUNCTION(Category="Item")
	virtual void Use(AJJInvCharacter* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return ItemID == OtherID;
	}
};
