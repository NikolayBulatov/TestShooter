// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UsableActor.h"
#include "FinalButton.generated.h"


UCLASS()
class AFinalButton : public AUsableActor
{
	GENERATED_BODY()

public:

	AFinalButton();

private:

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;


	//------------------------------------------------------------------------------

public:

	virtual void Use(APawn* InInstigator) override;


};
