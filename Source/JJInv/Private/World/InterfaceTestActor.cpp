// Copyright © Jodiah Jensen. All Rights Reserved.


#include "World/InterfaceTestActor.h"

// Sets default values
AInterfaceTestActor::AInterfaceTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	
}

// Called when the game starts or when spawned
void AInterfaceTestActor::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstanceInteractableData;
	
}

// Called every frame
void AInterfaceTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInterfaceTestActor::BeginFocus()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void AInterfaceTestActor::EndFocus()
{
		Mesh->SetRenderCustomDepth(false);
}

void AInterfaceTestActor::OnBeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling OnBeginInteract override on interface test actor"));
}

void AInterfaceTestActor::OnEndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling OnEndInteract override on interface test actor"));

}

void AInterfaceTestActor::OnInteract(AJJInvCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling OnInteract override on interface test actor"));

//	Destroy();
}

