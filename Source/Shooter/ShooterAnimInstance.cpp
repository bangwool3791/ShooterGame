   // Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "ShooterAnimInstance.h"
#include "ShooterChracter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance()
	: Speed(0.f)
	, bIsInAir(false)
	, bIsAccelerating(false)
	, MovementOffsetYaw(0.f)
	, LastMovementOffsetYaw(0.f)
	, bAiming(false)
	, TIPCharacterYaw(0.f)
	, TIPCharacterYawLastFrame(0.f)
	, RootYawOffset(0.f)
	, Pitch(0.f)
	, bReloading(false)
	, OffsetState(EOffsetState::EOS_Hip)
{
}

void UShooterAnimInstance::UpdateAnimationPropeties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterChracter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

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

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if(bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
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

	TurnInPlace();
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterChracter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		// Don't want to turn i place Character is moving
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		// Root Yaw Offset, updated and claped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);
		
		const float Turning{ GetCurveValue(TEXT("Turning")) };

		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset > 0, -> Turning Left. RootYawOffset < 0, -> Turning Right
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };

				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess; 
			}
		}
	}

}
