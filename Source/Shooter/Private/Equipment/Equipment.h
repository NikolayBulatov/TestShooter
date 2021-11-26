// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"


class AShooterCharacter;


UCLASS(Abstract)
class AEquipment : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquipment();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const
	{
		return Mesh1P;
	}

	/** Returns Mesh3P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh3P() const
	{
		return Mesh3P;
	}

private:
	
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh3P;
	
	//------------------------------------------------------------------------------
	// Action
	//------------------------------------------------------------------------------

public:
	
	void Action();
	
protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAction();

	virtual void HandleAction() { }

	//------------------------------------------------------------------------------
	// Equipment
	//------------------------------------------------------------------------------

public:

	virtual void Equip();
	virtual void UnEquip();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	FName EquipSocketName;
	
	
	//------------------------------------------------------------------------------

public:

	AShooterCharacter* GetOwnerPawn() const;
	bool IsFirstPerson() const;

	/** Deferred destroy this actor */
	void DeferredDestroy();

};
