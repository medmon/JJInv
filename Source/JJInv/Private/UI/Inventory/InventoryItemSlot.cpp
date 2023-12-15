// Copyright © Jodiah Jensen. All Rights Reserved.


#include "UI/Inventory/InventoryItemSlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/ItemBase.h"
#include "JJInv/JJInvColors.h"
#include "UI/Inventory/DragItemVisual.h"

#include "UI/Inventory/InventoryTooltip.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Util/ColorConstants.h"

class UItemBase;

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ToolTipClass)
	{
		UInventoryTooltip* ToolTip = CreateWidget<UInventoryTooltip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;

		SetToolTip(ToolTip);
	}
	
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		switch (ItemReference->ItemQuality)
		{
		case EItemQuality::Useless:
			ItemBorder->SetBrushColor(FLinearColor(0.1f, 0.1f,0.1f,1.f));
			break;
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor(0.1f, 0.1f,0.1f,1.f));
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
		case EItemQuality::Rare:
			ItemBorder->SetBrushColor(FLinearColor::Green);
			break;
		case EItemQuality::Epic:
//			ItemBorder->SetBrushColor(UE::Geometry::LinearColors::DarkCyan3b());
//			ItemBorder->SetBrushColor(FLinearColor(0.0f, 0.1f, 1.0f));
			ItemBorder->SetBrushColor(BLUE);
			break;
		case EItemQuality::Legendary:
			ItemBorder->SetBrushColor(UE::Geometry::LinearColors::Purple3f());
			break;
		case EItemQuality::Mythic:
//			ItemBorder->SetBrushColor(UE::Geometry::LinearColors::DarkOrange3b());
			ItemBorder->SetBrushColor(GOLD);
			break;
		case EItemQuality::Unique:
			//ItemBorder->SetBrushColor(UE::Geometry::LinearColors::DarkOrange3b());
			ItemBorder->SetBrushColor(ORANGE);
			break;
		default:
			ItemBorder->SetBrushColor(FLinearColor::White);
			;
		}
		
		ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
		
		if (ItemReference->ItemNumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	//TODO: sub menu on right click goes here
	
	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		
		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwningInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
		
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
