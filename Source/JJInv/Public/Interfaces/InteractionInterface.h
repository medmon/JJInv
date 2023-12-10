// Copyright © Jodiah Jensen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInterface.generated.h"

class AJJInvCharacter;

UENUM()
enum class EInteractableType : uint8
{
	Pickup,
	Converse,
	Activate,
	Operate,
	Open,
	Use,
	Examine,
	Harvest,
	Toggle,
	Click,
	Touch,
	Press,
	Turn,
	Pull,
	Twist,
	Push,
	Unlock
};

USTRUCT()
struct FInteractableData
{
	GENERATED_BODY()

	FInteractableData() :
		InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.f)
	{};

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText Action;

	//used only for pickups and maybe harvests
	UPROPERTY(EditInstanceOnly)
	int32 Quantity;

	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
	 
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class JJINV_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void OnBeginInteract();
	virtual void OnEndInteract();
	virtual void OnInteract(AJJInvCharacter* PlayerCharacter);

	FInteractableData InteractableData;
};
