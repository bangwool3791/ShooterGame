// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EqupInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called when overlapping AreaSphere */
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OhterComp, 
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/* Called when End Overlapping AreaSphere*/
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappepComponent,
		AActor* OhterActor,
		UPrimitiveComponent* OhterComp,
		int32 OtherBodyIndex);

	/* Sets the ActiveStars array of bools based on rarity*/
	void SetActiveStars();

	/* Sets properties of the Item's components based on State*/
	void SetItemProperties(EItemState State);

	/* Called when ItemInterpTimer is finished*/
	void FinishInterping();

	/* Handles item interpolation when in the EquipInterping state */
	void ItemInterp(float DeltaTime);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/* Skeletal Mesh for the item*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	USkeletalMeshComponent* ItemMesh;

	/* Line trace collides with box to show HUB widgets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = ( AllowPrivateAccess = "true"));
	class UBoxComponent* CollisionBox;

	/* Popup widget for when the player looks at the item*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class UWidgetComponent* PickupWidget;

	/* Enables item tracing when overlapped*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class USphereComponent* AreaSphere;

	/* The name which appears on the Pickup Widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	FString ItemName;

	/* Item count (ammo, etc.)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	int32 ItemCount;

	/* Item rarity - determines number of start in Pickup Widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	TArray<bool> ActiveStars;

	/* State of the Item*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	EItemState ItemState;

	/* The curve asset to use for the item's Z location when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class UCurveFloat* ItemZCurve;

	/* Starting location when interping begins */
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	FVector ItemInterpStartLocation;

	/* Target interp lcoation in fornt of the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	FVector CameraTargetLocation;

	/* true when interping*/
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	bool bInterping;

	/* Play when we start interping*/
	FTimerHandle ItemInterpTimer;
	/* Duration of the curve and timer*/
	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	float ZCurveTime;

	/* Pointer to the character*/
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class AShooterChracter* Character;

	/* X and Y for the Item while interping in the EquipInterping state*/
	float ItemInterpX;
	float ItemInterpY;

	/* Initial Yaw offset between the caera and the interping item*/
	float InterpInitialYawOffset;

	/* Curve used to scale the item when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	UCurveFloat* ItemScaleCurve;
public:
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	void SetItemState(EItemState State);

	// Called from the AShooterCharacter class
	void StartItemCurve(AShooterChracter* Char);
};
