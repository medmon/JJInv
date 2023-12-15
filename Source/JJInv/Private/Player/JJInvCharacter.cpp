// Copyright © Jodiah Jensen. All Rights Reserved.


#include "Player/JJInvCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interfaces/InteractionInterface.h"
#include "Particles/ParticleSystemComponent.h"
#include "UI/JJInvHUD.h"
#include "Components/InventoryComponent.h"
#include "World/Pickup.h"


// Constructor: sets default values
AJJInvCharacter::AJJInvCharacter() :
	BaseTurnRate(45.f),
	bAiming(false),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f)	
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create the camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;  //camera follows behind character at this distance
	CameraBoom->bUsePawnControlRotation = true;  // keep boom pointing forward relative to character
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	// Create a Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false;  // camera does not rotate relative to arm

	//don't rotate when the controller rotates.  Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f,0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	InteractionCheckFrequency = 0.1f;
	InteractionCheckDistance = 425.f;

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("PlayerInventory");
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.f);
}



// Called when the game starts or when this class is spawned
void AJJInvCharacter::BeginPlay()
{
	Super::BeginPlay();
//	UE_LOG(LogTemp,Warning,TEXT("BeginPlay() Called!"));

	HUD = Cast<AJJInvHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			 ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ShooterMappingContext, 0);
		}		
	}
}

void AJJInvCharacter::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
	
}

void AJJInvCharacter::ToggleMenu()
{
	HUD->ToggleMenu();
}

//=========================================================================================
//		InteractionInterface Functions
//=========================================================================================

void AJJInvCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector2d ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//get screen-space location of crosshair
	FVector2d CrosshairLocation(ViewportSize.X/2, ViewportSize.Y/2);
	CrosshairLocation.Y += CrosshairOffset.Y;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld;
	
	bScreenToWorld = GetCrosshairWorldPosition(
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (!bScreenToWorld) { return; }
	
	FVector TraceStart{CrosshairWorldPosition};
	FVector TraceEnd{TraceStart + (CrosshairWorldDirection * InteractionCheckDistance)};

	// DrawDebugLine(GetWorld(),
	// 	TraceStart,
	// 	TraceEnd,
	// 	FColor::Red,
	// 	false,
	// 	1.f,
	// 	0,
	// 	2.f
	// 	);
	//
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	
	FHitResult TraceHit;

	if (GetWorld()->LineTraceSingleByChannel(TraceHit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams))
	{
		if (TraceHit.GetActor()->GetClass()->
			ImplementsInterface(UInteractionInterface::StaticClass()))
		{
		
			if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
			{
				FoundInteractable(TraceHit.GetActor());
				return;
			}

			if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
			{
				return;
			}
		}
	}

	NoInteractableFound();
}


void AJJInvCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (bIsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	
	TargetInteractable->BeginFocus();
}

void AJJInvCharacter::NoInteractableFound()
{
	if (bIsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandleInteraction);
		
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		HUD->HideInteractionWidget();
		
		
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}

}

void AJJInvCharacter::BeginInteract()
{
	//verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->OnBeginInteract();

			if (FMath::IsNearlyZero(
				TargetInteractable->InteractableData.InteractionDuration,
				0.7f ))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(
					TimerHandleInteraction,
					this,
					&AJJInvCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration);
			}
		}
	}
}

void AJJInvCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandleInteraction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->OnEndInteract();
	}
	
}

void AJJInvCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandleInteraction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->OnInteract(this);
	}
}

void AJJInvCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AJJInvCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{GetActorLocation() + FVector(0.f,45.f,0.f) + (GetActorForwardVector()  * 235.f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop is null!"));
	}
}


//========================================================================================
//		Enhanced Input System
//========================================================================================
void AJJInvCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionValue = Value.Get<FVector2D>();

	if ((Controller != nullptr) && ((DirectionValue.X != 0.f) || DirectionValue.Y != 0.f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{0, Rotation.Yaw, 0 };
		
		//find which way is forward
		const FVector ForwardDirection{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		//move forward(backward)
		AddMovementInput(ForwardDirection, DirectionValue.X);

		//find which way is Right
		const FVector RightDirection{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		//move right(left)
		AddMovementInput(RightDirection, DirectionValue.Y);
	
	}
}

void AJJInvCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
	
}

void AJJInvCharacter::Jump()
{
	Super::Jump();
}

void AJJInvCharacter::FireWeapon() 
{
//	UE_LOG(LogTemp, Warning, TEXT("Firing weapon!"));

	//play fire weapon sound: FireSound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	//get socket at end of weapon barrel
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		//spawn muzzle flash particle system: MuzzleFlash
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		//get beam end location
		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			//spawn impact particle system: ImpactParticles
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

			//spawn beam particle system: BeamParticles
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					BeamParticles,
					SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}	
		
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	
}	//end FireWeapon()

bool AJJInvCharacter::GetCrosshairWorldPosition(FVector2d CrosshairLocation, FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection) const
{
	return UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(
			this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);
}

bool AJJInvCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamEndLocation) const
{
	FVector2d ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//get screen-space location of crosshair
	FVector2d CrosshairLocation(ViewportSize.X/2, ViewportSize.Y/2);
	CrosshairLocation.Y += CrosshairOffset.Y;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld;
	
	bScreenToWorld = GetCrosshairWorldPosition(
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End { Start + CrosshairWorldDirection * 50'000.f };

		//set beam end point to line trace end point
		OutBeamEndLocation = End;
		
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit,
			Start,
			End,
			ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit)
		{
			//update beam end point to hit location
			OutBeamEndLocation = ScreenTraceHit.Location;
			
			//Perform a second trace, this time from gun barrel socket
			FHitResult WeaponTraceHit;
			const FVector WeaponTraceStart{ MuzzleSocketLocation };
			const FVector WeaponTraceEnd{ OutBeamEndLocation };

			GetWorld()->LineTraceSingleByChannel(
				WeaponTraceHit,
				WeaponTraceStart,
				WeaponTraceEnd,
				ECollisionChannel::ECC_Visibility);

			if (WeaponTraceHit.bBlockingHit)
			{
				//update beam end point to second trace hit point location
				OutBeamEndLocation = WeaponTraceHit.Location;
			}

		}

		return true;
	}

	return false;
}

void AJJInvCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AJJInvCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AJJInvCharacter::InteractButtonPressed()
{
	bInteracting = true;
}

void AJJInvCharacter::InteractButtonReleased()
{
	bInteracting = false;
}


void AJJInvCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	
}


// Called to bind functionality to input
void AJJInvCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	
/*
 *	Old Input System method
 */
	
	// PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	// PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	// PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput );
	// PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

/*
 *	Enhanced Input System method
 */
	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction( MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::Move);

		EnhancedInputComponent->BindAction( LookAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::Look);

		EnhancedInputComponent->BindAction( JumpAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::Jump);

		EnhancedInputComponent->BindAction( JumpAction,
			ETriggerEvent::Completed,
			this,
			&ACharacter::StopJumping);

		
		EnhancedInputComponent->BindAction( FireAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::FireWeapon);
		
		EnhancedInputComponent->BindAction( AimingAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::AimingButtonPressed);
		
		EnhancedInputComponent->BindAction( AimingAction,
			ETriggerEvent::Completed,
			this,
			&AJJInvCharacter::AimingButtonReleased);

		EnhancedInputComponent->BindAction( InteractAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::BeginInteract);
		
		EnhancedInputComponent->BindAction( InteractAction,
			ETriggerEvent::Completed,
			this,
			&AJJInvCharacter::EndInteract);

		EnhancedInputComponent->BindAction( MenuAction,
			ETriggerEvent::Triggered,
			this,
			&AJJInvCharacter::ToggleMenu);
		
	}	


}
	
	

