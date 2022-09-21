// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
#include "SProjectile.h"
#include "Kismet/GameplayStatics.h"

void ASProjectileWeapon::Fire_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		ServerFire();
		return;
	}
	FireInitiated();
}

ASProjectileWeapon::ASProjectileWeapon()
{
	bReplicates = true;
}

void ASProjectileWeapon::FireInitiated()
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

	Multicast_PlaySound();
}

void ASProjectileWeapon::Multicast_PlaySound_Implementation()
{
	UGameplayStatics::SpawnSoundAttached(ShootSound, GetRootComponent(), TEXT("MuzzleSocket"));
}

void ASProjectileWeapon::ServerFire_Implementation()
{
	FireInitiated();
}
