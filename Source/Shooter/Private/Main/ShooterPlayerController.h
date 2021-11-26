// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"


UCLASS()
class AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AShooterPlayerController();

	// virtual void PostInitializeComponents() override;
	virtual void SetPawn(APawn* InPawn) override;

protected:

	// virtual void BeginPlay() override;
	// virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPawn(APawn* NewPawn);

private:

	TWeakObjectPtr<APawn> PrevPawn;
	
	//------------------------------------------------------------------------------

public:

	/** Called when PlayerState is initially replicated. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerStateClientInitialize(class AShooterPlayerState* ShooterPlayerState);

	/** Called when GameState is initialize. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameStateInitialize(class AShooterGameState* ShooterGameState);


};
