// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectile.h"
#include "Rocket.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ARocket : public ASProjectile
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rocket")
	float ExplosionRadius;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rocket")
	float LaunchVelosityMultiplier;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Rocket")
	float PhysicBodyImpulseStrength;

protected:

	virtual void BeginPlay() override;

	virtual void PreEndPlay_Implementation() override;

	virtual void HandleOnCollisonCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

	void AddRadialImpulseToPhysicBodyComponents();

	void LaunchCharactersInRadius(const FHitResult& Hit);
	
};
