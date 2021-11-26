// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "Equipment.h"

#include "Main/ShooterCharacter.h"

//==============================================================================


// Sets default values
AEquipment::AEquipment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetOwnerNoSee(true);

	RootComponent = Mesh3P;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(Mesh3P);
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->SetOnlyOwnerSee(true);

}


// Called when the game starts or when spawned
// void AEquipment::BeginPlay()
// {
// 	Super::BeginPlay();
// }


//------------------------------------------------------------------------------
// Action
//------------------------------------------------------------------------------


// Owner
void AEquipment::Action()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerAction();
	}
	HandleAction();
}


void AEquipment::ServerAction_Implementation()
{
	HandleAction();
}


bool AEquipment::ServerAction_Validate()
{
	return true;
}


//------------------------------------------------------------------------------
// Equipment
//------------------------------------------------------------------------------


void AEquipment::Equip()
{
	AShooterCharacter* OwnerPawn = Cast<AShooterCharacter>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}

	UnEquip();
	
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);

	Mesh1P->AttachToComponent(OwnerPawn->GetMesh1P(), FAttachmentTransformRules::KeepRelativeTransform, EquipSocketName);
	Mesh1P->SetHiddenInGame(false);

	Mesh3P->AttachToComponent(OwnerPawn->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquipSocketName);
	Mesh3P->SetHiddenInGame(false);
}


void AEquipment::UnEquip()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
}


//------------------------------------------------------------------------------


AShooterCharacter* AEquipment::GetOwnerPawn() const
{
	return Cast<AShooterCharacter>(GetOwner());
}


bool AEquipment::IsFirstPerson() const
{
	AShooterCharacter* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn != nullptr)
	{
		return OwnerPawn->IsFirstPerson();
	}
	return false;
}


void AEquipment::DeferredDestroy()
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
}
