// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Pickup.generated.h"

class UItemBase;

UCLASS()
class JJINV_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:
	
	// constructor
	APickup();

	void InitializePickup(TSubclassOf<UItemBase> BaseClass, const int32 InQuantity );

	void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);

	FORCEINLINE UItemBase* GetItemData() { return ItemReference; };
	virtual void BeginFocus() override;
    virtual void EndFocus() override;

	
protected:
	UPROPERTY(VisibleAnywhere, Category="Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditInstanceOnly, Category="Pickup | Item Database")
	UDataTable* ItemDataTable;

	UPROPERTY(EditInstanceOnly, Category="Pickup | Item Database")
	FName DesiredItemID;

	UPROPERTY(VisibleAnywhere, Category="Pickup | Item Reference")
	UItemBase* ItemReference;

	UPROPERTY(EditInstanceOnly, Category="Pickup | Item Database")
	int32 ItemQuantity;

	UPROPERTY(EditInstanceOnly, Category="Pickup | Interaction")
	FInteractableData InstanceInteractableData;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnInteract(AJJInvCharacter* PlayerCharacter) override;
	void UpdateInteractableData();
	
	void TakePickup( const AJJInvCharacter* Taker);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
