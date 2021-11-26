// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"


/**
 * 
 */
UCLASS()
class UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual void NativeConstruct() override;
//	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	//------------------------------------------------------------------------------

public:

	UFUNCTION(BlueprintCallable)
	void SetPlayerState(class AShooterPlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	void SetGameState(class AShooterGameState* GameState);

protected:

	UFUNCTION()
	void NativeGameStateChanged(class AGameStateBase* GameState);

	UFUNCTION()
	void NativePlayerListAdd(class APlayerState* PlayerState);

	UFUNCTION()
	void NativePlayerListRemove(class APlayerState* PlayerState);

	UFUNCTION()
	void NativePlayerListChange(class AShooterPlayerState* PlayerState);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnGameStateChanged(class AShooterGameState* GameState);

	/** Called when added PlayerState to the PlayerArray. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnPlayerListAdd(class AShooterPlayerState* PlayerState);

	/** Called when removed PlayerState from the PlayerArray. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnPlayerListRemove(class AShooterPlayerState* PlayerState);

	/** Called when changed PlayerState. For all players. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnPlayerListChange(class AShooterPlayerState* PlayerState);

	/** Called when changed PlayerState. For local player. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnLocalPlayerChange(class AShooterPlayerState* PlayerState);

};
