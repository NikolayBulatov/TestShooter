// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/Equipment.h"
#include "UsableActor.h"
#include "PickupObject.generated.h"


UCLASS(Abstract)
class APickupObject : public AUsableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickupObject();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	virtual void Use(APawn* InInstigator) override;

protected:

	virtual void DeferredDestroy() override;

//------------------------------------------------------------------------------

public:
	
	UPROPERTY(EditAnywhere, Category="Pickup")
	TSubclassOf<AEquipment> EquipmentClass;

};
