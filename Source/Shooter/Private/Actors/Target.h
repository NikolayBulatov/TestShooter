// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"


UCLASS()
class ATarget : public AActor
{
	GENERATED_BODY()

public:

	ATarget();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;


	//------------------------------------------------------------------------------

public:

	UPROPERTY(EditAnywhere, Category="Target")
	bool Motion = true;

	UPROPERTY(EditAnywhere, Category="Target")
	float MotionSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category="Target")
	float MotionDistance = 100.f;

	/** Offset from -1 to 1. */
	UPROPERTY(EditAnywhere, meta=(ClampMin="-1", ClampMax="1"), Category="Target")
	float MotionOffset = 0.f;

	/** Called when hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	FVector RootLocation;
	FRotator RootRotation;

	bool TeleportUpdatedComponent(float Time);
};
