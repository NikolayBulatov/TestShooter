// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "PickupObject.h"
#include "Main.h"

#include "Engine/CollisionProfile.h"

#include "Main/ShooterCharacter.h"


//==============================================================================


// Sets default values
APickupObject::APickupObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	NetUpdateFrequency = 5.f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
}


// Called when the game starts or when spawned
// void APickupObject::BeginPlay()
// {
// 	Super::BeginPlay();
// }

// Called every frame
// void APickupObject::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }


void APickupObject::Use(APawn* InInstigator)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	AShooterCharacter* Character = Cast<AShooterCharacter>(InInstigator);
	if (Character == nullptr)
	{
		return;	
	}

	FActorSpawnParameters SpawnInfo;

	FVector Location = GetActorLocation();
	Location.Z += 50.f;

	AEquipment* Equipment = GetWorld()->SpawnActor<AEquipment>(EquipmentClass, Location, FRotator::ZeroRotator, SpawnInfo);
	if (Equipment == nullptr)
	{
		return;
	}
			
	Character->SetEquipment(Equipment);
	DeferredDestroy();
}


void APickupObject::DeferredDestroy()
{
	Super::DeferredDestroy();
	
	Mesh->BodyInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

//------------------------------------------------------------------------------
