// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"


class AShooterCharacter;


UCLASS(minimalapi)
class AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	AShooterGameMode();

	/** Event called when target is hit. */
	void OnTargetHit(class ATarget* Target, class APawn* Instigator);

	/** Event called when final key is pressed. */
	void OnFinalKey(class AFinalButton* Button, class APawn* Instigator);

	void OnDamage(AShooterCharacter* Victim, AShooterCharacter* Instigator, float DamageAmount);

	void OnDie(AShooterCharacter* Victim);

	UFUNCTION(BlueprintCallable)
	void ServerTravel(const FString& URL);
	
protected:

	virtual void BeginPlay() override;

	/** Time to respawn character */
	UPROPERTY(EditDefaultsOnly, Category="Game Options")
	int32 TimeToRespawn;

};
