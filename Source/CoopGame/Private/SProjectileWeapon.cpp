// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "SProjectile.h"

void ASProjectileWeapon::Fire_Implementation()
{
	LastFiredTime = GetWorld()->TimeSeconds;
	if (!ProjectileClass)
		return;

	if (APawn* instigator = GetInstigator())
	{
		FVector outEyeViewLocation;
		FRotator outEyeViewRotation;
		instigator->GetActorEyesViewPoint(outEyeViewLocation, outEyeViewRotation);
		FActorSpawnParameters actorSpawnParameters = FActorSpawnParameters();
		actorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		actorSpawnParameters.Owner = this;
		actorSpawnParameters.Instigator = GetInstigator();
		GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, MeshComp->GetSocketLocation("MuzzleSocket"), outEyeViewRotation, actorSpawnParameters);
	}
}