// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "FinalButton.h"
#include "Main.h"

//#include "Net/UnrealNetwork.h"

#include "Main/ShooterGameMode.h"

//------------------------------------------------------------------------------


AFinalButton::AFinalButton()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;
//	PrimaryActorTick.TickInterval = 0.2f;
//	bReplicates = true;
//	bNetUseOwnerRelevancy = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
}


//------------------------------------------------------------------------------


void AFinalButton::Use(APawn* InInstigator)
{
	if (GetNetMode() < NM_Client)
	{
		AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode != nullptr)
		{
			GameMode->OnFinalKey(this, InInstigator);
		}
	}
}
