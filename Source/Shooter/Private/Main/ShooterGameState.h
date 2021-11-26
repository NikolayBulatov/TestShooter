// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"


UENUM(BlueprintType)
enum class EGameStatusEnum : uint8
{
	GS_None 	UMETA(DisplayName="None"),
	GS_Game 	UMETA(DisplayName="Game"),
	GS_Final 	UMETA(DisplayName="Final")
};


UCLASS()
class AShooterGameState : public AGameStateBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameStateDelegate, AGameStateBase*, GameState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerStateDelegate, class APlayerState*, PlayerState);
	
public:

	AShooterGameState();

	virtual void PostInitializeComponents() override;
	virtual void AddPlayerState(class APlayerState* PlayerState) override;
	virtual void RemovePlayerState(class APlayerState* PlayerState) override;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FGameStateDelegate OnChanged;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FPlayerStateDelegate OnPlayerSatateAdd;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FPlayerStateDelegate OnPlayerSatateRemove;

protected:

	virtual void NativeChanged();

	
	//------------------------------------------------------------------------------

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GameState")
	EGameStatusEnum GetGameStatus() const
	{
		return GameStatus;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GameState")
	class AShooterPlayerState* GetWinnerPlayerState() const
	{
		return WinnerPlayerState;
	}

	void SetGameStatus(EGameStatusEnum InGameStatus);
	void SetWinnerPlayerState(AShooterPlayerState* PlayerState);

protected:

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_GameStatus")
	EGameStatusEnum GameStatus;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_WinnerPlayerState")
	class AShooterPlayerState* WinnerPlayerState;

	UFUNCTION()
	void OnRep_GameStatus();

	UFUNCTION()
	void OnRep_WinnerPlayerState();

};
