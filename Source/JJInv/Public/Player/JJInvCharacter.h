// Copyright © Jodiah Jensen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"


#include "JJInvCharacter.generated.h"

USTRUCT()
struct FInteractionData 
{
	GENERATED_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.f)
	{
		
	};
	
	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
	
};

class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class IInteractionInterface;

UCLASS()
class JJINV_API AJJInvCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D CrosshairOffset = FVector2d(0.f, 0.f);


	// Sets default values for this character's properties
	AJJInvCharacter();
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE bool bIsInteracting() const
	{
		return GetWorldTimerManager().IsTimerActive(TimerHandleInteraction);
	};

	// Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	UPROPERTY(VisibleAnywhere, Category="Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	float InteractionCheckFrequency;

	float InteractionCheckDistance;

	FTimerHandle TimerHandleInteraction;

	FInteractionData InteractionData;

	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

/*
 *	Enhanced Input System
 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* ShooterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* AimingAction;
	
	virtual void Jump() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void FireWeapon();
	bool GetCrosshairWorldPosition(FVector2d CrosshairLocation, FVector& CrosshairWorldPosition,
	                               FVector& CrosshairWorldDirection) const;

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamEndLocation) const;

	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

private:
	//camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess))
	USpringArmComponent* CameraBoom;

	//camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess))
	UCameraComponent* FollowCamera;

	// Base turn rate in degrees per second. Other scaling may affect final turn rate.
	//This is used for turning with the keyboard versus mouselook 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Camera", meta=(AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Combat", meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Combat", meta=(AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	
	
};
