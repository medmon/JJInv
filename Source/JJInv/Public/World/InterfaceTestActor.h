// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "InterfaceTestActor.generated.h"

UCLASS()
class JJINV_API AInterfaceTestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInterfaceTestActor();
	
protected:

	UPROPERTY(EditAnywhere, Category="Test Actor")
	UStaticMeshComponent* Mesh;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void OnBeginInteract() override;
	virtual void OnEndInteract() override;
	virtual void OnInteract() override;

};
