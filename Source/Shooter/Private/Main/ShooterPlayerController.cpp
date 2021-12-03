// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "ShooterPlayerController.h"
#include "Main.h"

// #include "Net/UnrealNetwork.h"

#include "ShooterGameMode.h"
#include "ShooterGameState.h"

//------------------------------------------------------------------------------


AShooterPlayerController::AShooterPlayerController()
	: Super()
{

}


// void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	//DOREPLIFETIME(AShooterPlayerController, ReplicatedVariable1);
// }


void AShooterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsLocalPlayerController())
	{
		AShooterGameState* GameState = GetWorld()->GetGameState<AShooterGameState>();
		if (GameState != nullptr)
		{
			OnGameStateInitialize(GameState);
		}
	}
}


void AShooterPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (InPawn != PrevPawn)
	{
		OnPawn(InPawn);
		PrevPawn = InPawn;
	}
}


// void AShooterPlayerController::BeginPlay()
// {
// 	Super::BeginPlay();
// }


// void AShooterPlayerController::SetupInputComponent()
// {
// 	Super::SetupInputComponent();
// 
// 	InputComponent->BindAction("Action1", IE_Pressed, this, &AShooterPlayerController::StartAction1);
// 	InputComponent->BindAction("Action1", IE_Released, this, &AShooterPlayerController::StopAction1);
// }


//------------------------------------------------------------------------------
