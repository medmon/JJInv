// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemVisual.generated.h"

class UTextBlock;
class UImage;
class UBorder;
/**
 * 
 */
UCLASS()
class JJINV_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UTextBlock* ItemQuantity;

	
	
};
