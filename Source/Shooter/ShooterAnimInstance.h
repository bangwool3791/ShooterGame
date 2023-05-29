// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),
	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};
/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UShooterAnimInstance();
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationPropeties(float DeltaTime);
	virtual void NativeInitializeAnimation() override;

protected:
	/* Handle turning in place variables*/
	void TurnInPlace();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	class AShooterChracter* ShooterCharacter;

	/*The speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float Speed;
	/* Whether or not the chracter is in the air*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsInAir;
	/*Wheter or not the chracter is moving*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsAccelerating;

	/* Offset yaw used for strafing */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetYaw;

	/* Offset yaw the frame before we stopped moving*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	/* Yaw of the Character this frame*/
	float TIPCharacterYaw;

	/* Yaw of the Character the previous frame*/
	float TIPCharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trun In Place", meta = (AllowPrivateAccess = "true"));
	float RootYawOffset;

	/* Rotation curve value this frame*/
	float RotationCurve;
	/* Rotation curve value last frame*/
	float RotationCurveLastFrame;

	/* The ptich of the aim rotation, used for Aim Offset*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trun In Place", meta = (AllowPrivateAccess = "true"));
	float Pitch;

	/* True when reloading, used to prevent Aim Offset while Reloading*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trun In Place", meta = (AllowPrivateAccess = "true"));
	bool bReloading;

	/* Offset state used to determine which Aim Offset to use*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trun In Place", meta = (AllowPrivateAccess = "true"));
	EOffsetState OffsetState;
};
