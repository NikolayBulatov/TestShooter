// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "HUDWidget.h"
#include "Main.h"

#include "GameFramework/PlayerState.h"

#include "Main/ShooterPlayerController.h"
#include "Main/ShooterGameState.h"
#include "Main/ShooterPlayerState.h"

//=============================================================================


UHUDWidget::UHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		AShooterPlayerState* PlayerState = PlayerController->GetPlayerState<AShooterPlayerState>();
		if (PlayerState != nullptr)
		{
			SetPlayerState(PlayerState);
		}
	}

	AShooterGameState* GameState = GetWorld()->GetGameState<AShooterGameState>();
	if (GameState != nullptr)
	{
		SetGameState(GameState);
	}
}


// void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
// {
// 	Super::NativeTick(MyGeometry, InDeltaTime);
// }


//=============================================================================


// Called from: NativeConstruct, PlayerController
void UHUDWidget::SetPlayerState(AShooterPlayerState* PlayerState)
{
	if (PlayerState == nullptr)
	{
		return;
	}

	PlayerState->OnChanged.AddDynamic(this, &UHUDWidget::OnLocalPlayerChange);
	OnLocalPlayerChange(PlayerState);

	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState != nullptr)
	{
		NativePlayerListChange(ShooterPlayerState);
	}
}


// Called from: NativeConstruct, PlayerController
void UHUDWidget::SetGameState(AShooterGameState* GameState)
{
	if (GameState == nullptr)
	{
		return;
	}

	GameState->OnChanged.AddDynamic(this, &UHUDWidget::NativeGameStateChanged);
	OnGameStateChanged(GameState);

	GameState->OnPlayerSatateAdd.AddDynamic(this, &UHUDWidget::NativePlayerListAdd);
	GameState->OnPlayerSatateRemove.AddDynamic(this, &UHUDWidget::NativePlayerListRemove);

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		NativePlayerListAdd(PlayerState);
	}
}


void UHUDWidget::NativeGameStateChanged(AGameStateBase* GameState)
{
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	if (ShooterGameState != nullptr)
	{
		OnGameStateChanged(ShooterGameState);
	}
}


void UHUDWidget::NativePlayerListAdd(APlayerState* PlayerState)
{
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState != nullptr)
	{
		ShooterPlayerState->OnChanged.AddDynamic(this, &UHUDWidget::NativePlayerListChange);
		NativePlayerListChange(ShooterPlayerState);

		OnPlayerListAdd(ShooterPlayerState);
	}
}


void UHUDWidget::NativePlayerListRemove(APlayerState* PlayerState)
{
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState != nullptr)
	{
		ShooterPlayerState->OnChanged.RemoveDynamic(this, &UHUDWidget::NativePlayerListChange);

		OnPlayerListRemove(ShooterPlayerState);
	}
}


void UHUDWidget::NativePlayerListChange(AShooterPlayerState* PlayerState)
{
	OnPlayerListChange(PlayerState);
}
