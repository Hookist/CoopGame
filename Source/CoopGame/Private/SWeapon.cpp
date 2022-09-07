// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Components/SkeletalMeshComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include "SProjectile.h"
#include <Camera/CameraShakeBase.h>
#include "CoopGame/CoopGame.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <TimerManager.h>

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.f;

	RateOfFire = 600.f;
}

void ASWeapon::Fire_Implementation()
{
	// Trace the world, from pawn eyes to crosshair location

	AActor* owner = GetOwner();

	if (!owner)
		return;

	FVector eyeLocation;
	FRotator eyeRotation;
	owner->GetActorEyesViewPoint(eyeLocation, eyeRotation);

	FVector shotDirection = eyeRotation.Vector();

	FVector traceEnd = eyeLocation + (shotDirection * 10000);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(owner);
	queryParams.AddIgnoredActor(this);
	queryParams.bTraceComplex = true;
	queryParams.bReturnPhysicalMaterial = true;

	// Particle "Target" parameter
	FVector tracerEndPoint = traceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, eyeLocation, traceEnd, COLLISION_WEAPON, queryParams))
	{
		// Blocking hit! Process damage

		AActor* HitActor = Hit.GetActor();
		EPhysicalSurface surfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		float actualDamage = BaseDamage;
		if (surfaceType == SURFACE_FLESHVULNERABLE)
		{
			actualDamage *= 4.f;
		}

		UGameplayStatics::ApplyPointDamage(HitActor, actualDamage, shotDirection, Hit, owner->GetInstigatorController(), this, DamageType);

		UParticleSystem* selectedEffect = nullptr;
		switch (surfaceType)
		{
		case SURFACE_FLESHDEFAULT:
			selectedEffect = FleshImpactEffect;
			break;
		case SURFACE_FLESHVULNERABLE:
			selectedEffect = FleshImpactEffect;
			break;
		default:
			selectedEffect = DefaultImpactEffect;
			break;
		}

		if (selectedEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), selectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		tracerEndPoint = Hit.ImpactPoint;
	}

	if (DebugWeaponDrawing > 0)
	{ 
		DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::White, false, 1.f, 0, 1.f);
	}

	PlayFireEffects(tracerEndPoint);

	LastFiredTime = GetWorld()->TimeSeconds;
}

void ASWeapon::StartFire()
{
	float firstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, firstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / RateOfFire;
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector muzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* tracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, muzzleLocation);
		if (tracerComp)
		{
			tracerComp->SetVectorParameter("Target", TracerEndPoint);
		}
	}

	APawn* myOwner = Cast<APawn>(GetOwner());
	if (myOwner)
	{
		APlayerController* PC = Cast<APlayerController>(myOwner->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}
