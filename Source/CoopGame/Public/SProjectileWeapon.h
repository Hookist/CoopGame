// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SProjectileWeapon.generated.h"

class ASProjectile;

UCLASS()
class COOPGAME_API ASProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()

protected:

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<ASProjectile> ProjectileClass;

public:

	ASProjectileWeapon();

	virtual void Fire_Implementation() override;

	virtual void ServerFire_Implementation() override;

protected:

	virtual void FireInitiated();

private:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySound();
};
