// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

#include "ShooterChracter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem()
	:ItemName(FString("Default"))
	, ItemCount(0)
	, ItemRarity(EItemRarity::EIR_Common)
	, ItemState(EItemState::EIS_Pickup)
	// Item interp variables
	, ZCurveTime(0.7f)
	, ItemInterpStartLocation(FVector(0.f))
	, CameraTargetLocation(FVector(0.f))
	, bInterping(false)
	, ItemInterpX(0.f)
	, ItemInterpY(0.f)
	, InterpInitialYawOffset(0.f)
	, ItemType(EItemType::EIT_MAX)
	, InterpLocIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));

	/*
	* The component that defines the transform (location, rotation, scale) of this Actor in the world, all other components must be attached to this one somehow
	*/
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	//CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//CollisionBox->SetCollisionResponseToChannel(
	//	ECollisionChannel::ECC_Visibility, 
	//	ECollisionResponse::ECR_Block);

	PickupWidget = CreateAbstractDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent> (TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	// Hide Pickup Widget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	// Sets ActiveStars array based on Item Rarity
	SetActiveStars();
	// Setup overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Item properties based on ItemState
	SetItemProperties(ItemState);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Item Interping when in the Equiping state
	ItemInterp(DeltaTime);
}

void AItem::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OhterComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterChracter* ShooterChracter = dynamic_cast<AShooterChracter*>(OtherActor);

		if (ShooterChracter)
		{
			ShooterChracter->IncreamentOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappepComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OhterComp,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterChracter* ShooterChracter = dynamic_cast<AShooterChracter*>(OtherActor);

		if (ShooterChracter)
		{
			ShooterChracter->IncreamentOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	// The 0 element isn't used
	for (int32 i = 0; i <= 5; ++i)
	{
		ActiveStars.Add(false);
	}

	switch(ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_UnCommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}
}


void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

		/*The Second means that it generates an Overlap event which you must handle if you want it to do something e.g.
		BeginOverlap and EndOverlap. 
		Use Overlap when you want to do something like a motion detector or proximity mine or breaking of glass as a bullet passes through it. */
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		/*
		*  NoCollision	Will not create any representation in the physics engine.
			QueryOnly	Only used for spatial queries (raycasts, sweeps, and overlaps).
			PhysicsOnly	Only used only for physics simulation (rigid body, constraints).
			QueryAndPhysics	Can be used for both spatial queries (raycasts, sweeps, overlaps) and simulation (rigid body, constraints).
		*/
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);

		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		// Set Sphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		// Set Sphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		//Mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);

		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

		// Set Sphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterChracter* Char)
{
	// Store a handle th the Character
	Character = Char;

	// Get array index in interpLcations with the lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();
	// Add 1 to the Item Count for this interp location struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);
	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);
	}

	// Store initial location of the Item
	ItemInterpStartLocation = GetActorLocation();
	//
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&AItem::FinishInterping,
		ZCurveTime);

	// Get initial Yaw of the Camera
	const double CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw};
	// Get initial Yaw of the Item
	const double ItemRotationYaw{ GetActorRotation().Yaw };
	// INitial Yaw offset between Camera and Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

void AItem::FinishInterping()
{
	bInterping = false;

	if (Character)
	{
		// Subtract 1 from the Item Count of the interp location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickUpItem(this);
	}
	// Set scale back to normal
	SetActorScale3D(FVector(1.f));
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping)return;

	if (Character && ItemZCurve)
	{
		// Elaped time sice we started ItemInterpTimer
		const float ElapedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// Get curve value corresponding to ElapedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapedTime);

		// Get the item's initial when the curve started
		FVector ItemLocation = ItemInterpStartLocation;

		//Get location in front of the camera
		const FVector CameraInterpLocation{GetInterpLocation()};
		// Vector from Item to Camera Interp Location, X and Y are zeroed out
		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		// Interpolated X value
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X, 
			CameraInterpLocation.X, 
			DeltaTime, 
			30.0);
		// Interpolated Y value
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.0);

		// Set X and Y of ItemLocation to Interped values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		// Adding curve value to the Z component of the Inital Location (scaled by DeltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// Camera rotation this frame
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		// Camera roation plus initial Yaw Offset
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };

		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
		/*
		* In the context of Unreal Engine, "sweep" refers to a collision detection mechanism known as "sweeping" or "swept collision."
		It allows you to check for collisions between a moving object and other objects in the environment, 
		taking into account the object's movement over a specified distance or time interval.
		*/
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);

	switch(ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}
	return FVector();
}
