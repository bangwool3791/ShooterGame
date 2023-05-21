   // Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "ShooterAnimInstance.h"
#include "ShooterChracter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationPropeties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterChracter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// Get the lateral speed of the chracter from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0.f;

		Speed = Velocity.Size();

		// Is the character in the air ?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is the chracter accerlating?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		
		FRotator MovementRotation =
			UKismetMathLibrary::MakeRotFromX(
				ShooterCharacter->GetVelocity());

			MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
				MovementRotation,
				AimRotation).Yaw;
		
		if(ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		bAiming = ShooterCharacter->GetAiming();
		//FString RotationMessage = 
		//	FString::Printf(
		//		TEXT("Base Aim Rotation: %f"), 
		//		AimRotation.Yaw);

		//FString RotationMessage =
		//	FString::Printf(
		//		TEXT("Movement Rotation: %f"),
		//		MovementRotation.Yaw);

		//FString OffsetMessage =
		//	FString::Printf(
		//		TEXT("Movement Offset Yaw: %f"),
		//		MovementOffsetYaw);

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
		//}
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterChracter>(TryGetPawnOwner());
}
