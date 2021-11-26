// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "Target.h"
#include "Main.h"

//#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameStateBase.h"

#include "Main/ShooterGameMode.h"

//------------------------------------------------------------------------------


ATarget::ATarget()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
//	PrimaryActorTick.TickInterval = 0.2f;
//	bReplicates = true;
//	bNetUseOwnerRelevancy = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->OnComponentHit.AddDynamic(this, &ATarget::OnHit);

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Projectile, ECR_Block);
}


void ATarget::PostInitializeComponents()
{
	Super::PostInitializeComponents();

 	RootLocation = RootComponent->GetComponentLocation();
 	RootRotation = RootComponent->GetComponentRotation();
}


void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AGameStateBase* GameState = GetWorld()->GetGameState();
	TeleportUpdatedComponent(GameState->GetServerWorldTimeSeconds());
}


//------------------------------------------------------------------------------


bool ATarget::TeleportUpdatedComponent(float Time)
{
	if (Motion == true)
	{
		float Distance1 = Time * MotionSpeed / MotionDistance + MotionOffset;
		float Distance2 = FMath::Asin(FMath::Sin(Distance1 * PI)) / PI;
		FVector NewLocation = RootLocation + RootRotation.Vector() * Distance2 * MotionDistance;

		bool Sweep = false;
		EMoveComponentFlags MoveComponentFlags = MOVECOMP_NoFlags;
		FVector MoveDelta = NewLocation - RootComponent->GetComponentLocation();

		return RootComponent->MoveComponent(MoveDelta, RootRotation.Quaternion(), Sweep, nullptr, MoveComponentFlags, TeleportFlagToEnum(true));
	}
	return false;
}


void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr && OtherActor != nullptr)
	{
		GameMode->OnTargetHit(this, OtherActor->GetInstigator());
	}
}
