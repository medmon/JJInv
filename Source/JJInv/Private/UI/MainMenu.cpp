// Copyright © Jodiah Jensen. All Rights Reserved.


#include "UI/MainMenu.h"

#include "Player/JJInvCharacter.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AJJInvCharacter>(GetOwningPlayerPawn());
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//TODO: cast item to drag drop, ensure player is valid, call drop item on player
	
}
