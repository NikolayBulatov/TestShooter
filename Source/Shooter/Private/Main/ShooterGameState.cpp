// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "ShooterGameState.h"
#include "Main.h"

#include "Net/UnrealNetwork.h"

#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"

//------------------------------------------------------------------------------


AShooterGameState::AShooterGameState()
	: Super()
{
//	NetUpdateFrequency = 10.0;
	
}


void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, GameStatus);
	DOREPLIFETIME(AShooterGameState, WinnerPlayerState);
}


void AShooterGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PlayerController != nullptr)
	{
		PlayerController->OnGameStateInitialize(this);
	}
}


void AShooterGameState::NativeChanged()
{
	OnChanged.Broadcast(this);
}


void AShooterGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	OnPlayerSatateAdd.Broadcast(PlayerState);
}


void AShooterGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	OnPlayerSatateRemove.Broadcast(PlayerState);
}


//------------------------------------------------------------------------------


void AShooterGameState::SetGameStatus(EGameStatusEnum InGameStatus)
{
	GameStatus = InGameStatus;

	if (GetLocalRole() == ROLE_Authority)
	{
		NativeChanged();
	}
}


void AShooterGameState::SetWinnerPlayerState(AShooterPlayerState* PlayerState)
{
	WinnerPlayerState = PlayerState;

	if (GetLocalRole() == ROLE_Authority)
	{
		NativeChanged();
	}
}


void AShooterGameState::OnRep_GameStatus()
{
	NativeChanged();
}


void AShooterGameState::OnRep_WinnerPlayerState()
{
	NativeChanged();
}
