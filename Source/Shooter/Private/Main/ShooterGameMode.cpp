// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "ShooterGameMode.h"
#include "Main.h"

#include "UObject/ConstructorHelpers.h"
#include "Math/UnrealMathUtility.h"

#include "ShooterGameState.h"
#include "ShooterHUD.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerState.h"
#include "ShooterPlayerController.h"
#include "Actors/Target.h"
#include "Actors/FinalButton.h"

//------------------------------------------------------------------------------


AShooterGameMode::AShooterGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Main/Game/BP_Game_Character"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	GameStateClass = AShooterGameState::StaticClass();
	HUDClass = AShooterHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	PlayerStateClass = AShooterPlayerState::StaticClass();

	bUseSeamlessTravel = true;
	TimeToRespawn = 5.f;
}


void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	AShooterGameState* GameState = GetGameState<AShooterGameState>();
	if (GameState != nullptr)
	{
		GameState->SetGameStatus(EGameStatusEnum::GS_Game);
	}
}


//------------------------------------------------------------------------------


void AShooterGameMode::OnTargetHit(ATarget* Target, APawn* Instigator)
{
	if (Instigator == nullptr)
	{
		return;
	}

	AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(Instigator->GetPlayerState());
	if (PlayerState != nullptr)
	{
		PlayerState->ScoreHitIncrement();
	}
}


void AShooterGameMode::OnFinalKey(AFinalButton* Button, APawn* Instigator)
{
	AShooterGameState* GameState = GetGameState<AShooterGameState>();
	if (GameState != nullptr)
	{
		GameState->SetGameStatus(EGameStatusEnum::GS_Final);

		if (Instigator != nullptr)
		{
			GameState->SetWinnerPlayerState(Instigator->GetPlayerState<AShooterPlayerState>());
		}
	}
}


void AShooterGameMode::OnDamage(AShooterCharacter* Victim, AShooterCharacter* Instigator, float DamageAmount)
{
	AShooterPlayerState* VictimPS = Victim->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* InstigatorPS = Instigator->GetPlayerState<AShooterPlayerState>();

	if (InstigatorPS != nullptr)
	{
		InstigatorPS->ScoreHitIncrement();
	}
	
	Victim->AddDamage(DamageAmount);
	
	if (Victim->IsDeath())
	{
		if (VictimPS != nullptr)
		{
			VictimPS->ScoreDeathIncrement();
		}
		
		if (InstigatorPS != nullptr)
		{
			InstigatorPS->ScoreKillIncrement();
		}
	}
}


void AShooterGameMode::OnDie(AShooterCharacter* Victim)
{
	AShooterPlayerController* VictimPC = Victim->GetController<AShooterPlayerController>();
	Victim->DoDie();

	// Respawn
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&, VictimPC]()
	{
		RestartPlayer(VictimPC);
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, TimeToRespawn, false);
}


void AShooterGameMode::ServerTravel(const FString& URL)
{
	GetWorld()->ServerTravel(URL, true);
}
