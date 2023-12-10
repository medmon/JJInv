// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"


class UProgressBar;
class AJJInvCharacter;
struct FInteractableData;
class UTextBlock;

/**
 * 
 */
UCLASS()
class JJINV_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Player Reference")
	AJJInvCharacter* PlayerReference;

	void UpdateWidget(const FInteractableData* InteractableData);
protected:
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* NameText;
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* ActionText;
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* QuantityText;
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	UProgressBar* InteractionProgressBar;

	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Interactable Data", meta = (BindWidget))
	float CurrentInteractionDuration;

	UFUNCTION(Category="Interaction Widget | Interactable Data")
	float UpdateInteractionProgress();
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
