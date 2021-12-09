// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"


UCLASS()
class AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShooterPlayerStateDelegate, AShooterPlayerState*, ShooterPlayerState);
	
public:

	AShooterPlayerState();

	virtual void ClientInitialize(class AController* C) override;

	UPROPERTY(BlueprintAssignable)
	FShooterPlayerStateDelegate OnChanged;

protected:

	virtual void CopyProperties(APlayerState* NewPlayerState) override;
	virtual void NativeChanged();

	//------------------------------------------------------------------------------

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Score")
	int32 GetScoreHits() const
	{
		return ScoreHits;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Score")
	int32 GetScoreKills() const
	{
		return ScoreKills;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Score")
	int32 GetScoreDeaths() const
	{
		return ScoreDeaths;
	}

	void ScoreHitIncrement();
	void ScoreKillIncrement();
	void ScoreDeathIncrement();
	
protected:

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_Changed")
	int32 ScoreHits;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_Changed")
	int32 ScoreKills;

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_Changed")
	int32 ScoreDeaths;

	UFUNCTION()
	void OnRep_Changed();

};
