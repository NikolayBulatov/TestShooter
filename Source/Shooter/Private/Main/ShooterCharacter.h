// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


class AEquipment;


UCLASS(config="Game")
class AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthDelegate, float, Health);

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	AShooterCharacter();

protected:
	// virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float BaseLookUpRate;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	class UAnimMontage* FireAnimation;

protected:
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

protected:
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const
	{
		return Mesh1P;
	}
	
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const
	{
		return FirstPersonCameraComponent;
	}

	//------------------------------------------------------------------------------
	// Sound
	//------------------------------------------------------------------------------

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sound")
	bool SoundEnabled = true;

	void PlaySound(class USoundBase* SoundToPlay);


	//------------------------------------------------------------------------------
	// Fire
	//------------------------------------------------------------------------------

protected:

	/** Fires a projectile. */
	void OnFire();

	//------------------------------------------------------------------------------
	// Use target
	//------------------------------------------------------------------------------

protected:

	UPROPERTY(EditDefaultsOnly, Category="Pawn|Character")
	float MaxTargetDistance;

	void UseTarget();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseTarget();
	void ServerUseTarget_Implementation();
	bool ServerUseTarget_Validate();

	//------------------------------------------------------------------------------
	// Equipment
	//------------------------------------------------------------------------------

public:
	UPROPERTY(BlueprintAssignable)
	FSimpleDelegate OnEquipmentChanged;

	UPROPERTY(BlueprintReadWrite, Category="Debug")
	bool ShowWeaponLineTrace;
	
	void SetEquipment(AEquipment* InEquipment);	
	
protected:

	UPROPERTY(ReplicatedUsing="OnRep_Equipment", BlueprintReadOnly)
	AEquipment* Equipment;

	UFUNCTION()
	void OnRep_Equipment(AEquipment* OldEquipment);

	//------------------------------------------------------------------------------
	// Health
	//------------------------------------------------------------------------------

public:
	
	UPROPERTY(BlueprintAssignable)
	FSimpleDelegate OnDie;

	UPROPERTY(BlueprintAssignable)
	FHealthDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FSimpleDelegate OnDamage;

	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetHealth(float Value);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const
	{
		return Health;
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddDamage(float DamageAmount);

	bool IsDeath() const
	{
		return Health <= 0;
	}

	void DoDie();

protected:

	UPROPERTY(ReplicatedUsing="OnRep_Health", EditDefaultsOnly, Category="Health")
	float Health;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="Health")
	float HealthMax;
	
	UFUNCTION(NetMulticast, Reliable)
	void ClientDie();

private:

	UFUNCTION()
	void OnRep_Health(float OldHealth);


	//------------------------------------------------------------------------------
	// Respawn
	//------------------------------------------------------------------------------

protected:

	UPROPERTY(EditDefaultsOnly, Category="Health")
	float RespawnTime;

	//------------------------------------------------------------------------------
	
public:

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category="Targeting")
	FRotator GetAimOffsets() const;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category="Mesh")
	virtual bool IsFirstPerson() const;

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	bool GetActorInView(FHitResult& OutHit, float Distance);

	/** Deferred destroy this actor */
	void DeferredDestroy();

};

