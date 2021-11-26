// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "ShooterCharacter.h"
#include "Main.h"

#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"

#include "Actors/UsableActor.h"
#include "ShooterGameMode.h"
#include "Equipment/Equipment.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AShooterCharacter

AShooterCharacter::AShooterCharacter()
{
	// Set size for collision capsule
	UCapsuleComponent* CapsuleComponent = GetCapsuleComponent();
	CapsuleComponent->InitCapsuleSize(34.f, 88.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(CapsuleComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	USkeletalMeshComponent* Mesh3P = GetMesh();
	Mesh3P->bOnlyOwnerSee = false;
	Mesh3P->bOwnerNoSee = true;

	MaxTargetDistance = 400.0;
	HealthMax = 100;
	Health = 100;
	RespawnTime = 5.f;
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	DOREPLIFETIME_CONDITION(AShooterCharacter, Equipment, COND_None);
	DOREPLIFETIME_CONDITION(AShooterCharacter, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, HealthMax, COND_OwnerOnly);
}

// void AShooterCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
// }

//////////////////////////////////////////////////////////////////////////
// Input

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::OnFire);

	// Use target event
	PlayerInputComponent->BindAction("UseTarget", IE_Pressed, this, &AShooterCharacter::UseTarget);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	// Bind rotation events
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

//------------------------------------------------------------------------------
// Sound
//------------------------------------------------------------------------------


void AShooterCharacter::PlaySound(USoundBase* SoundToPlay)
{
	if (SoundEnabled == false || GetNetMode() == NM_DedicatedServer || SoundToPlay == nullptr)
	{
		return;
	}

	USceneComponent* AttachToComponent = GetRootComponent();
	FName AttachPointName = NAME_None;
	FVector Location = FVector(ForceInit);
	FRotator Rotation = FRotator::ZeroRotator;
	EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset;
	bool bStopWhenAttachedToDestroyed = false;
	float VolumeMultiplier = 1.f;
	float PitchMultiplier = 1.f;
	float StartTime = 0.f;
	USoundAttenuation* Attenuation = nullptr;
	USoundConcurrency* Concurrency = nullptr;
	bool bAutoDestroy = true;

	UGameplayStatics::SpawnSoundAttached(SoundToPlay, AttachToComponent, AttachPointName, Location, Rotation, LocationType,
		bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, Attenuation, Concurrency, bAutoDestroy);
}


//------------------------------------------------------------------------------
// Fire
//------------------------------------------------------------------------------


void AShooterCharacter::OnFire()
{
	if (Equipment != nullptr)
	{
		Equipment->Action();
	}
}


//------------------------------------------------------------------------------
// Use target
//------------------------------------------------------------------------------


void AShooterCharacter::UseTarget()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerUseTarget();
	}

	FHitResult Hit(ForceInit);
	if (GetActorInView(Hit, MaxTargetDistance) == false)
	{
		return;
	}

	AUsableActor* UsableActor = Cast<AUsableActor>(Hit.GetActor());
	if (UsableActor == nullptr)
	{
		return;
	}

	UsableActor->Use(this);
}


void AShooterCharacter::ServerUseTarget_Implementation()
{
	UseTarget();
}


bool AShooterCharacter::ServerUseTarget_Validate()
{
	return true;
}


//------------------------------------------------------------------------------
// Equipment
//------------------------------------------------------------------------------


// Server
void AShooterCharacter::SetEquipment(AEquipment* InEquipment)
{
	auto OldEquipment = Equipment;
	Equipment = InEquipment;
	OnRep_Equipment(OldEquipment);

	if (OldEquipment != nullptr)
	{
		OldEquipment->DeferredDestroy();
	}
}


void AShooterCharacter::OnRep_Equipment(AEquipment* OldEquipment)
{
	if (OldEquipment != nullptr)
	{
		OldEquipment->UnEquip();
	}

	if (Equipment != nullptr)
	{
		Equipment->SetOwner(this);
		Equipment->Equip();

		OnEquipmentChanged.Broadcast();
	}
}


//------------------------------------------------------------------------------
// Health
//------------------------------------------------------------------------------


// Server
void AShooterCharacter::SetHealth(float Value)
{
	float OldHealth = Health;
	Health = FMath::Clamp<float>(Value, 0, HealthMax);

	OnHealthChanged.Broadcast(Health);
	if (OldHealth > Health)
	{
		OnDamage.Broadcast();
	}

	if (Health == 0)
	{
		AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
		if (GameMode != nullptr)
		{
			GameMode->OnDie(this);
		}
		else
		{
			DoDie();
		}
	}
}


// Server
void AShooterCharacter::AddDamage(float DamageAmount)
{
	float OldHealth = Health;
	Health = FMath::Clamp<float>(Health - DamageAmount, 0, HealthMax);

	OnHealthChanged.Broadcast(Health);
	if (OldHealth > Health)
	{
		OnDamage.Broadcast();
	}

	if (Health == 0)
	{
		AShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
		if (GameMode != nullptr)
		{
			GameMode->OnDie(this);
		}
		else
		{
			DoDie();
		}
	}
}


// Server
void AShooterCharacter::DoDie()
{
	if (Controller != nullptr)
	{
		Controller->UnPossess();
	}
	SetReplicateMovement(false);
	
	ClientDie();
}


void AShooterCharacter::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(Health);
	if (OldHealth > Health)
	{
		OnDamage.Broadcast();
	}
}


// All
void AShooterCharacter::ClientDie_Implementation()
{
	UCapsuleComponent* CapsuleComponent = GetCapsuleComponent();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	USkeletalMeshComponent* Mesh3P = GetMesh();
	Mesh3P->SetCollisionProfileName(TEXT("Ragdoll"));

	Mesh3P->SetAllBodiesSimulatePhysics(true);
	Mesh3P->SetSimulatePhysics(true);
	Mesh3P->bBlendPhysics = true;
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent != nullptr)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->SetMovementMode(MOVE_None);
		MovementComponent->SetComponentTickEnabled(false);
	}

	OnDie.Broadcast();
	DeferredDestroy();
}


//------------------------------------------------------------------------------


FRotator AShooterCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


bool AShooterCharacter::IsFirstPerson() const
{
	return Controller && Controller->IsLocalPlayerController();
}


USkeletalMeshComponent* AShooterCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}


bool AShooterCharacter::GetActorInView(FHitResult& OutHit, float Distance)
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
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

	return GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
}


void AShooterCharacter::DeferredDestroy()
{
	if (Equipment != nullptr)
	{
		Equipment->DeferredDestroy();
	}
	
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&]()
	{
		Destroy();
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, RespawnTime, false);

	// Hide actor
	SetReplicatingMovement(false);
	TearOff();
	// SetActorHiddenInGame(true);
}
