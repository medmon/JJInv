// Copyright © Jodiah Jensen. All Rights Reserved.


#include "JJInvCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

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
}

// Called when the game starts or when this class is spawned
void AJJInvCharacter::BeginPlay()
{
	Super::BeginPlay();
//	UE_LOG(LogTemp,Warning,TEXT("BeginPlay() Called!"));

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

/*
 *	Enhanced Input System
 */
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
	CrosshairLocation.Y += -50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;		

	//get world position and direction of crosshair
	bool bScreenToWorld =
	UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(
			this, 0),
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


// Called every frame
void AJJInvCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	
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
		
	}	


}
	
	

