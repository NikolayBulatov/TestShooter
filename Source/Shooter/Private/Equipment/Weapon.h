// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Weapon.generated.h"


USTRUCT()
struct FWeaponFire
{
	GENERATED_BODY()

	UPROPERTY()
	FVector FireLocation;

	UPROPERTY()
	bool Explosion;
	
	UPROPERTY()
	FVector ExplosionLocation;

	UPROPERTY()
	FVector_NetQuantizeNormal ExplosionNormal;
};


UCLASS(Abstract)
class AWeapon : public AEquipment
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponDelegate, AWeapon*, Weapon);

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:

	virtual void HandleAction() override;

	//------------------------------------------------------------------------------
	//  Ammunition
	//------------------------------------------------------------------------------

public:

	UPROPERTY(BlueprintAssignable)
	FWeaponDelegate OnAmmunitionChanged;

	UFUNCTION(BlueprintCallable)
	void Reload();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 AmmunitionMax;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing="OnRep_Ammunition")
	int32 Ammunition;

	UFUNCTION()
	void OnRep_Ammunition();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	void HandleReload();
	
	//------------------------------------------------------------------------------
	// Effects
	//------------------------------------------------------------------------------

protected:
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	class USoundBase* FireSound;

	/** Explosion FX */
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	class UParticleSystem* ExplosionFX;
	
	/** Explosion sound */
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	class USoundBase* ExplosionSound;
	
	UPROPERTY(ReplicatedUsing="OnRep_WeaponFire")
	FWeaponFire WeaponFire;

	/** From Server */
	UFUNCTION()
	void OnRep_WeaponFire();

	/** From Owner */
	void OnLoc_WeaponFire(const FWeaponFire& WeaponFire);

	void Debug_WeaponFire(const FWeaponFire& WeaponFire, const FColor& Color) const;
	
	//------------------------------------------------------------------------------

public:
	
	FVector GetMuzzleLocation() const;
	bool GetActorInView(FHitResult& OutHit, float Distance) const;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	float DamageAmount;
	
};
