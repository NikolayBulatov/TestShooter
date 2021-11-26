// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "Projectile.h"
#include "Main.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Main/ShooterCharacter.h"

//------------------------------------------------------------------------------


AProjectile::AProjectile()
{
	bReplicates = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName(Projectile_ProfileName);
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	DamageAmount = 50;
}


//------------------------------------------------------------------------------


void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->AddDamage(DamageAmount);
	}

	DeferredDestroy();
}


void AProjectile::DeferredDestroy()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&]()
	{
		Destroy();
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0, false);

	// Hide actor
	SetReplicatingMovement(false);
	TearOff();
	SetActorHiddenInGame(true);
	CollisionComp->OnComponentHit.RemoveDynamic(this, &AProjectile::OnHit);
	CollisionComp->BodyInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}
