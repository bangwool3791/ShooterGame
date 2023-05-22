// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterChracter.generated.h"

UCLASS()
class SHOOTER_API AShooterChracter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterChracter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called for forwards/backwards input*/
	void MoveForward(float Value);

	/* Called for side to side input*/
	void MoveRight(float Value);

	/*
	* Called via input to turn at a given rate.
	* @param Rate This is a normalize rate, i.e. 10.0 meas 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/*
	* Called via input to look up/down at a given rate.
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired rate
	*/
	void LookUpAtRate(float Rate);

	/*
	* Rotate controller based on mouse x movement
	* @param Rate The input value form mouse movement
	*/
	void Turn(float Value);

	/*
	* Rotate controller based on mouse y movement
	* @param Rate The input value form mouse movement
	*/
	void LookUp(float Value);
	/*
	* Called when the Fire Button is pressed
	*/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& uzzleSocketLocation, FVector& OutBeamLocation);

	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float);

	/* Set BaseTurnRate and BaseLookUpRate based on aiming*/
	void SetLookRates();

	void CalculateCrosshariSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
		void AutoFireReset();

	/* Line trace for items under the crossharis*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/* Trace for items if OverlappedItemCount > 0 */
	void TraceForItems();
	UFUNCTION()
	void FinishCrosshairBulletFire();

	/* Spawns a default weapon and squips it*/
	class AWeapon* SpawnDefaultWeapon();

	/* Takes a weapon and attaches it to the mesh*/
	void EquipWeapon(class AWeapon* WeaponToEquip);

	/* Detach weapon an let it fall to the ground*/
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	/* Drops currently equipped Weapon and Equips TraceHitItem*/
	void SwapWeapon(AWeapon* WeaponeToSwap);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/* Camera boom positioning the camera beghin the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	/* Camera taht follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;

	/* Base turn rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;
	/*Base look up/down rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	/* Turn rate while note aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float HipTurnRate;

	/* Look up rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float HipLookUpRate;

	/* Turn rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float AimingTurnRate;

	/* Look up rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float AimingLookUpRate;

	/* Scale factor for mouse look sensitivity. Turn rate when not aiming.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseHipTurnRate;
	/* Scale factor for mouse look sensitivity. Look up rate when not aiming.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseHipLookUpRate;

	/* Scale factor for mouse look sensitivity. Turn rate when aiming.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseAimingTurnRate;
	/* Scale factor for mouse look sensitivity. Look up rate when aiming.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseAimingLookUpRate;

	/*Rndomized gunshot sound cue*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	/* Flahs spawnd at BarrelSOcket*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	/*Montage for firing the weapon.*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	/* Particles spawnd upon bullet impact*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	class AItem* TraceHitItemLastFrame;

	/* Distance outward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Items, meta = (AllowPrivateAccess = "true"));
	float CameraInterpDistance;

	/* Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	float CameraInterpElevation;

	/* Currently equipped Weapon*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	AWeapon* EquippedWeapon;

	/*
	* TSubclassOf is a template type that represents a class or a blueprint in the object hierarchy.
	  It is used to define a variable or a parameter that can hold a reference to a subclass or derived class of a specific base class.
	*/
	/* Set this in Blueprints for the default weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/* True when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	/* Default camera field of view value */
	float CameraDefaultFOV;

	/* Field of view value for when zoom in */
	float CameraZoomedFOV;

	/* Current field of view this frame*/
	float CameraCurrentFOV;

	/* Interp speed for zomming when aming*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomInterpSpeed;

	/* Determines the spread of the corssharis */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	float CrosshairSpreadMultiplier;

	/* Velocity component for crossharis spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	float CrosshairVelocityFactor;

	/* In air component for crossharis spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	float CrosshairInAirFactor;

	/* Aim component for crossharis spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	float CrosshairAimFactor;

	/* Shooting component for crossharis spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	float CrosshairShootingFactor;

	/* The item currently hit by out trace in TraceForItems (coud be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crossharis, meta = (AllowPrivateAccess = "true"));
	AItem* TraceHitItem;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/* Left mouse button or right console trigger pressed */
	bool bFireButtonPressed;

	/* True when we can fire. False when waiting for the timer*/
	bool bShouldFire;

	/* Rate of automatic gun fire*/
	float AutomaticFireRate;

	/* Sets a timer between gunshots*/
	FTimerHandle AutoFireTimer;

	/* True if we should trace every frame for items*/
	bool bShouldTraceForItems;

	/* Number of overlapped AItems*/
	int8 OverlappedItemCount;
public:
	/* Returns CameraBoom subobject*/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/*Returns FollowCamera subobject*/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming()const { return bAiming; }

	UFUNCTION(BlueprintCallable)
		float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/* Adds/subtracts to/from OverlappedItemCOunt and updates bShouldTraceForItems*/
	void IncreamentOverlappedItemCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickUpItem(AItem* Item);
};
