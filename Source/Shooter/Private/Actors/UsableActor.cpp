// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#include "UsableActor.h"
#include "Main.h"

//#include "Net/UnrealNetwork.h"

//------------------------------------------------------------------------------


AUsableActor::AUsableActor()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;
//	PrimaryActorTick.TickInterval = 0.2f;
//	bReplicates = true;
//	bNetUseOwnerRelevancy = true;
}


//------------------------------------------------------------------------------


void AUsableActor::DeferredDestroy()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&]()
	{
		Destroy();
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0, false);

	// Hide actor
	SetReplicatingMovement(false);
	TearOff();
	SetActorHiddenInGame(true);
}
