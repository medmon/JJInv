// Copyright © Jodiah Jensen. All Rights Reserved.


#include "JJInvAnimInstance.h"

#include "JJInvCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UJJInvAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (JJInvCharacter == nullptr)
	{
		JJInvCharacter = Cast<AJJInvCharacter>(TryGetPawnOwner());
	}

	if (JJInvCharacter)
	{
		FVector Velocity{JJInvCharacter->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = JJInvCharacter->GetCharacterMovement()->IsFalling();

		bIsAccelerating = JJInvCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0.f? true : false;

		FRotator AimRotation = JJInvCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(JJInvCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (JJInvCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		
		bAiming = JJInvCharacter->GetAiming();
	}
}

void UJJInvAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	JJInvCharacter = Cast<AJJInvCharacter>(TryGetPawnOwner());
	
}
