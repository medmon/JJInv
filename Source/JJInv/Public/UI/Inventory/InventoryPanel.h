// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class AJJInvCharacter;
class UTextBlock;
class UWrapBox;
/**
 * 
 */
UCLASS()
class JJINV_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void RefreshInventory();
	
	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryPanel;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeightInfo;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	AJJInvCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	
protected:

	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
};
