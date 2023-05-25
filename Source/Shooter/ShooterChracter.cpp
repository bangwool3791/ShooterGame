// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterChracter.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components\WidgetComponent.h"
#include "Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AShooterChracter::AShooterChracter()
// Base rates for turning/looking up
	:BaseTurnRate{ 45.f }
	, BaseLookUpRate{ 45.f }
	// Turn rates for aiming/not aiming
	, HipTurnRate(90.f)
	, HipLookUpRate(90.f)
	, AimingTurnRate(20.f)
	, AimingLookUpRate(20.f)
	// Mouse kook sensitivity scale factors
	, MouseHipTurnRate{ 1.f }
	, MouseHipLookUpRate{ 1.f }
	, MouseAimingTurnRate{ 0.2f }
	, MouseAimingLookUpRate{ 0.2f }
	// true when aiming the weapon
	, bAiming{ false }
	, CameraDefaultFOV{ 0.f }
	//Camera field of view values 
	//Your Game
	, CameraZoomedFOV{ 35.f }
	, CameraCurrentFOV{ 0.f }
	, ZoomInterpSpeed{ 20.f }
	// Crosshair spread factors
	, CrosshairSpreadMultiplier{ 0.f }
	, CrosshairVelocityFactor{ 0.f }
	, CrosshairInAirFactor{ 0.f }
	, CrosshairAimFactor{ 0.f }
	, CrosshairShootingFactor{ 0.f }
	// Bullert fire timer variables
	, ShootTimeDuration{ 0.05f }
	, bFiringBullet{ false }
	// AUtomatic fire variables
	, AutomaticFireRate{ 0.1f }
	, bShouldFire{ true }
	, bFireButtonPressed{ false }
	, bShouldTraceForItems{false}

	// Camera interp location valuable
	, CameraInterpDistance(250.f)
	, CameraInterpElevation(65.f)
	// Starting ammo amounts
	, Starting9mmAmmo(85)
	, StartingARAmmo(120)
	// Combat variables
	, CombatState(ECombatState::ECS_Unoccupied)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a camera boom (pulls in towards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; //The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true;// Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);
	//Create a follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; //Camera dose not rotate relative to arm

	// Don't rotate when the controller rotates. Let he controller only affect rhe camear.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;// Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterChracter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	// Spawn the default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());

	InitializeAmmoMap();
}

// Called every frame
void AShooterChracter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	* Handle interpolation for zoom when aiming
	*/
	CameraInterpZoom(DeltaTime);
	// Change look senstivity based on aiming
	SetLookRates();

	CalculateCrosshariSpread(DeltaTime);

	// CHeck OverlappedItemCount, then trace for items
	TraceForItems();
}


void AShooterChracter::MoveForward(float Value)
{
	if ((Controller != nullptr) && Value != 0)
	{
		// find out which way is forward
		const FRotator Rotatation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotatation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterChracter::MoveRight(float Value)
{
	if ((Controller != nullptr) && Value != 0)
	{
		// find out which way is forward
		const FRotator Rotatation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotatation.Yaw, 0 };
		UE_LOG(LogTemp, Warning, TEXT("Rot : %f"), Rotatation.Yaw);
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		UE_LOG(LogTemp, Warning, TEXT("X : %f, Y : %f Z : %f"), Direction.X, Direction.Y, Direction.Z);
		AddMovementInput(Direction, Value);
	}
}

void AShooterChracter::TurnAtRate(float Rate)
{
	// calculate delta for this frame form the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());// deg/sec * sec/frame
}

void AShooterChracter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());// deg/sec * sec/frame
}

void AShooterChracter::Turn(float Value)
{
	float TurnScaleFactor{};

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterChracter::LookUp(float Value)
{
	float LookUpScaleFactor{};

	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}

	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterChracter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;

	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();

		SendBullet();

		PlayGunFireMontage();
		// Subtract 1 from the Weapon's Ammo
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();
	}

	// Start bullet fire timer for crosshairs
	//StartCrosshairBulletFire();
}

bool AShooterChracter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{
	// Check for crosshair trace hit
	FHitResult CrosshariHitResult;
	bool bCrosshariHit = TraceUnderCrosshairs(CrosshariHitResult, OutBeamLocation);

	if (bCrosshariHit)
	{
		// Tentative beam location - still need to trae from gun
		OutBeamLocation = CrosshariHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End location for the line trace

	}

	// Perform a second trace, this time from the gun barrel
	FHitResult WeaponTraceHit;

	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartEnd * 1.25f };

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (WeaponTraceHit.bBlockingHit) //object between bareel and BeamEndPoint?
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

void AShooterChracter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterChracter::AimingButtonReleased()
{
	bAiming = false;
}

void AShooterChracter::CameraInterpZoom(float DeltaTime)
{
	// Set current camera field of view
	if (bAiming)
	{
		// Interpolate to zoomed FOV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		// Interpolate to default FOV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}
void AShooterChracter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}
void AShooterChracter::CalculateCrosshariSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	// Calculate crosshari velocity factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	/* Calculate crosshair in air factor */
	if (GetCharacterMovement()->IsFalling()) // is in air?
	{
		// Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else // Character is on the ground
	{
		// Shrink the crossharis rapidly while one the ground
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	/* Calculate crosshair aim factor */
	if (bAiming) // Are we aiming
	{
		// Shrink crossharis a small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else // Not aiming
	{
		// Spread corssharis back to normal very quickly
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	// True 0.05 second after fireing
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.3f,
			DeltaTime,
			60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			60.f);
	}

	CrosshairSpreadMultiplier =
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;

}
void AShooterChracter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterChracter::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AShooterChracter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterChracter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterChracter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(
		AutoFireTimer,
		this,
		&AShooterChracter::AutoFireReset,
		AutomaticFireRate);
}

void AShooterChracter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		// Reload Weapon
		ReloadWeapon();
	}
}

bool AShooterChracter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		// Tentative beam location - still need to trace from gun
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	else // no crosshair trace hit
	{

	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshariWorldDirection;

	//Get screen space location of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshariWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshariWorldDirection * 50'000 };

		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AShooterChracter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterChracter::FinishReloading()
{
	// Update the COmbat State
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon == nullptr) return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };
	//Update the AmmoMap
	if (AmmoMap.Contains(AmmoType))
	{
		// Amount of ammo the Character is carrying of the EquippedWeapon type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// Space left in the magazine of EquippedWeapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo) 
		{
			// Reload the magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// file the ammo
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

float AShooterChracter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterChracter::IncreamentOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterChracter::TraceForItems()
{
	if (bShouldTraceForItems)
	{

		FHitResult ItemTraceResult;
		FVector HitLocation;
		if (TraceUnderCrosshairs(ItemTraceResult, HitLocation))
		{
			if (ItemTraceResult.bBlockingHit)
			{
				//AActor* pActor = ItemTraceResult.GetActor();
				//FString ActorName{};
				//pActor->GetName(ActorName);
				//UE_LOG(LogTemp, Warning, TEXT("Here %s"), *ItemTraceResult.GetActor()->GetActorNameOrLabel());
				//UE_LOG(LogTemp, Warning, TEXT("Here 2 %s"), *ActorName);

				TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
				FString str;

				if (TraceHitItem)
				{
					TraceHitItem->GetName(str);
					UE_LOG(LogTemp, Warning, TEXT("Here %s"), *str);
				}

				if (TraceHitItem && TraceHitItem->GetPickupWidget())
				{

					UE_LOG(LogTemp, Warning, TEXT("Here 1"));

					TraceHitItem->GetPickupWidget()->SetVisibility(true);
				}

				if (TraceHitItemLastFrame != TraceHitItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("Here 2"));

					if (TraceHitItemLastFrame && TraceHitItemLastFrame->GetPickupWidget())
					{

						UE_LOG(LogTemp, Warning, TEXT("Here 3"));

						TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);

					}
				}
				TraceHitItemLastFrame = TraceHitItem;
			}
		}
	}
	else 
	{
		if (TraceHitItemLastFrame && TraceHitItemLastFrame->GetPickupWidget())
		{
			TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		}
	}
}

AWeapon* AShooterChracter::SpawnDefaultWeapon()
{
	// Check the TSubclassOf variable
	if (DefaultWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);

		
		// Get the Hand Socket
		//const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(
		//	FName("RightHandSocket"));

		//if (HandSocket)
		//{
		//	// Attach the Weapon to the hand socket
		//	HandSocket->AttachActor(DefaultWeapon, GetMesh());
		//}

		//EquippedWeapon = DefaultWeapon;
	}
	return nullptr;
}

void AShooterChracter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(
			FName("RightHandSocket"));

		if (HandSocket)
		{
			// Attach the Weapon to the hand socket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterChracter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);

		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterChracter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
	}
}


void AShooterChracter::SelectButtonReleased()
{

}

void AShooterChracter::SwapWeapon(AWeapon* WeaponeToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponeToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterChracter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterChracter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr)
		return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterChracter::PlayFireSound()
{
	// Play Fire sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterChracter::SendBullet()
{

	// Send bullet
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(
			EquippedWeapon->GetItemMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;

		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

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
}

void AShooterChracter::PlayGunFireMontage()
{
	// Play Hip Fire Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterChracter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterChracter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon == nullptr) return;
	// Do we have ammo of the correct type ?
	// TODO: Create bool CarryingAmmo()

	if (CarryingAmmo()) // replace with CarryingAmmo()
	{
		CombatState = ECombatState::ECS_Reloading;

		UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstace)
		{
			AnimInstace->Montage_Play(ReloadMontage);
			AnimInstace->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool AShooterChracter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

// Called to bind functionality to input
void AShooterChracter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterChracter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterChracter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterChracter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterChracter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterChracter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterChracter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterChracter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterChracter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this,
		&AShooterChracter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this,
		&AShooterChracter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed,
		this, &AShooterChracter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released,
		this, &AShooterChracter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed,
		this, &AShooterChracter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released,
		this, &AShooterChracter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed,
		this, &AShooterChracter::ReloadButtonPressed);
}


FVector AShooterChracter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };
	// Desired = CameraWolrdLocation + Forward * A + Up * B
	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}

void AShooterChracter::GetPickUpItem(AItem* Item)
{
	auto Weapon = Cast<AWeapon>(Item);

	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
}