// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "Weapon.h"
#include "Main.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "Main/ShooterCharacter.h"
#include "Main/ShooterGameMode.h"

//==============================================================================


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MuzzleAttachPoint = TEXT("Muzzle");
	AmmunitionMax = 10;
	Ammunition = 10;
	DamageAmount = 50;
}


void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	DOREPLIFETIME_CONDITION(AWeapon, Ammunition, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, WeaponFire, COND_None);
}


// Called when the game starts or when spawned
// void AWeapon::BeginPlay()
// {
// 	Super::BeginPlay();
// }


// Server, Owner
void AWeapon::HandleAction()
{
	if (Ammunition <= 0)
	{
		return;
	}

	Ammunition--;
	OnRep_Ammunition();

	FHitResult Hit(ForceInit);
	WeaponFire.Explosion = GetActorInView(Hit, 5000);
	WeaponFire.ExplosionLocation = Hit.Location;
	WeaponFire.ExplosionNormal = Hit.Normal;
	WeaponFire.FireLocation = GetMuzzleLocation();

	if (GetLocalRole() == ROLE_Authority)
	{
		// Damage
		AShooterCharacter* Victim = Cast<AShooterCharacter>(Hit.Actor);
		if (Victim != nullptr)
		{
			AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
			if (GameMode != nullptr)
			{
				GameMode->OnDamage(Victim, GetOwnerPawn(), DamageAmount);
			}
			else
			{
				Victim->AddDamage(DamageAmount);
			}
		}

		// If Owner Listen Server
		if (IsFirstPerson())
		{
			OnLoc_WeaponFire(WeaponFire);
		}
		else
		{
			OnRep_WeaponFire();
		}
	}
	else
	{
		OnLoc_WeaponFire(WeaponFire);
	}
}


//------------------------------------------------------------------------------
//  Ammunition
//------------------------------------------------------------------------------


// Owner
void AWeapon::Reload()
{
	ServerReload();
	HandleReload();
}


void AWeapon::ServerReload_Implementation()
{
	HandleReload();
}


bool AWeapon::ServerReload_Validate()
{
	return true;
}

void AWeapon::HandleReload()
{
	Ammunition = AmmunitionMax;
	OnRep_Ammunition();
}


void AWeapon::OnRep_Ammunition()
{
	OnAmmunitionChanged.Broadcast(this);
}


//------------------------------------------------------------------------------
// Effects
//------------------------------------------------------------------------------


// All
void AWeapon::OnRep_WeaponFire()
{
	Debug_WeaponFire(WeaponFire, FColor::Purple);
	
	if (IsFirstPerson())
	{
		return;
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, WeaponFire.FireLocation);
	}

	if (WeaponFire.Explosion)
	{
		FVector Location = WeaponFire.ExplosionLocation + WeaponFire.ExplosionNormal * 5.f;

		if (ExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, Location, FRotator::ZeroRotator);
		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, Location);
		}
	}
}


// Owner
void AWeapon::OnLoc_WeaponFire(const FWeaponFire& WeaponFire)
{
	Debug_WeaponFire(WeaponFire, FColor::Green);

	AShooterCharacter* OwnerPawn = GetOwnerPawn();
	if (FireSound != nullptr && OwnerPawn != nullptr)
	{
		OwnerPawn->PlaySound(FireSound);
	}

	if (WeaponFire.Explosion)
	{
		FVector Location = WeaponFire.ExplosionLocation + WeaponFire.ExplosionNormal * 5.f;
	
		if (ExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, Location, FRotator::ZeroRotator);
		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, Location);
		}
	}
}


void AWeapon::Debug_WeaponFire(const FWeaponFire& WeaponFire, const FColor& Color) const
{
	AShooterCharacter* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn != nullptr && OwnerPawn->ShowWeaponLineTrace)
	{
		float PointSize = 15.f;
		float LineThickess = 1.f;
		float LifeTime = 10.f;
	
		DrawDebugLine(GetWorld(), WeaponFire.FireLocation, WeaponFire.ExplosionLocation, Color, false, LifeTime, 0, LineThickess);
		DrawDebugPoint(GetWorld(), WeaponFire.ExplosionLocation, PointSize, Color, false, LifeTime);
	}
}


//------------------------------------------------------------------------------


FVector AWeapon::GetMuzzleLocation() const
{
	if (IsFirstPerson())
	{
		return GetMesh1P()->GetSocketLocation(MuzzleAttachPoint);
	}
	return GetMesh3P()->GetSocketLocation(MuzzleAttachPoint);
}


bool AWeapon::GetActorInView(FHitResult& OutHit, float Distance) const
{
	AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return false;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->Controller);
	if (PlayerController == nullptr)
	{
		return false;
	}

	FVector CamLoc;
	FRotator CamRot;
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * Distance);

	FCollisionQueryParams QueryParams(TEXT("TraceActor"), true, this);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(OwnerPawn);

	bool res = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Projectile, QueryParams);
	if (!res)
	{
		FVector Normal = TraceStart - TraceEnd;
		Normal.Normalize();
		OutHit.Location = FVector_NetQuantize(TraceEnd);
		OutHit.Normal = FVector_NetQuantizeNormal(Normal);
	}	
	return res;
}
