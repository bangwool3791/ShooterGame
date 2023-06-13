// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	AAmmo();

	virtual void Tick(float DeltaTime)override;
protected:
	virtual void BeginPlay() override;
	virtual void SetItemProperties(EItemState State) override;

	/* Called when overlapping AreaSphere */
	UFUNCTION()
		void AmmoSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OhterComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess));
	UStaticMeshComponent* AmmoMesh;
	
	/* Ammo type for the ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess));
	EAmmoType AmmoType;

	/* The texture for the Ammo icon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess));
	UTexture2D* AmmoIconTexture;

	/* Overlap spehre for picking up the Ammo*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess));
	class USphereComponent* AmmoCollisionSphere;
public :
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const {return AmmoMesh;}
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	
};
