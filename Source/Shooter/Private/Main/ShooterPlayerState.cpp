// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "ShooterPlayerState.h"
#include "Main.h"

#include "Net/UnrealNetwork.h"

#include "ShooterPlayerController.h"

//------------------------------------------------------------------------------


AShooterPlayerState::AShooterPlayerState()
	: Super()
{
	// NetUpdateFrequency = 1.0;
}


void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, ScoreHits);
	DOREPLIFETIME(AShooterPlayerState, ScoreKills);
	DOREPLIFETIME(AShooterPlayerState, ScoreDeaths);
}


void AShooterPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(C);
	if (PlayerController != nullptr)
	{
		PlayerController->OnPlayerStateClientInitialize(this);
	}
}


void AShooterPlayerState::NativeChanged()
{
	OnChanged.Broadcast(this);
}


//------------------------------------------------------------------------------


void AShooterPlayerState::ScoreHitIncrement()
{
	ScoreHits++;
	NativeChanged();
}


void AShooterPlayerState::ScoreKillIncrement()
{
	ScoreKills++;
	NativeChanged();
}


void AShooterPlayerState::ScoreDeathIncrement()
{
	ScoreDeaths++;
	NativeChanged();
}


void AShooterPlayerState::OnRep_Changed()
{
	NativeChanged();
}
