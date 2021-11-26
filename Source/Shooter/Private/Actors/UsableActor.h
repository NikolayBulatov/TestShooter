// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UsableActor.generated.h"


/**
 * Base class for all usable actors.
 */
UCLASS(Abstract)
class AUsableActor : public AActor
{
	GENERATED_BODY()

public:

	AUsableActor();

	//------------------------------------------------------------------------------

public:

	virtual void Use(APawn* InInstigator) { };

protected:

	/** Deferred destroy this actor */
	virtual void DeferredDestroy();
	
};
