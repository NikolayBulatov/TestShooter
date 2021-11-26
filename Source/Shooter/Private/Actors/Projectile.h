// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


UCLASS(config="Game")
class AProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	class UProjectileMovementComponent* ProjectileMovement;

public:

	AProjectile();

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const
	{
		return CollisionComp;
	}
	
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const
	{
		return ProjectileMovement;
	}

	//------------------------------------------------------------------------------

public:

	/** Called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	/** Explosion FX */
	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UParticleSystem* ExplosionFX;

	/** Explosion sound */
	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class USoundBase* ExplosionSound;

	/** Deferred destroy this actor */
	void DeferredDestroy();

	UPROPERTY(EditDefaultsOnly)
	float DamageAmount;

};
