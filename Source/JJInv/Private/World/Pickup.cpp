// Copyright © Jodiah Jensen. All Rights Reserved.


#include "World/Pickup.h"

#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Player/JJInvCharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
	
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
	
}

void APickup::InitializePickup(TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemDataRow = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);
		
		ItemReference->ItemID = ItemDataRow->ItemID;
		ItemReference->ItemType = ItemDataRow->ItemType;
		ItemReference->ItemQuality = ItemDataRow->ItemQuality;
		ItemReference->ItemNumericData = ItemDataRow->ItemNumericData;
		ItemReference->ItemTextData = ItemDataRow->ItemTextData;
		ItemReference->ItemAssetData = ItemDataRow->ItemAssetData;
		ItemReference->ItemStatistics = ItemDataRow->ItemStatistics;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);		

		PickupMesh->SetStaticMesh(ItemDataRow->ItemAssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);		
	ItemReference->ItemNumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->ItemAssetData.Mesh);

	UpdateInteractableData();

}

void APickup::BeginFocus()
{
	IInteractionInterface::BeginFocus();
}

void APickup::EndFocus()
{
	IInteractionInterface::EndFocus();
}

void APickup::OnInteract(AJJInvCharacter* PlayerCharacter)
{
	IInteractionInterface::OnInteract(PlayerCharacter);

	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->ItemTextData.InteractText;
	InstanceInteractableData.Name = ItemReference->ItemTextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;

	InteractableData = InstanceInteractableData;
	
}

void APickup::TakePickup(const AJJInvCharacter* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					
					break;
				case EItemAddResult::IAR_AllItemsAdded:
//					Destroy();
					break;
				default: ;
				}
				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Inventory Component is null!"));
			}
		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was null!"))
		}
	}
}

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ?
		PropertyChangedEvent.Property->GetFName() :
		NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
	{
		if (ItemDataTable)
		{
			if (const FItemData* ItemDataRow = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickupMesh->SetStaticMesh(ItemDataRow->ItemAssetData.Mesh);
			}
		}
	}
}

