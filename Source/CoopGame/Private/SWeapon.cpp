// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Components/SkeletalMeshComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include "SProjectile.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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

	// Particle "Target" parameter
	FVector tracerEndPoint = traceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, eyeLocation, traceEnd, ECC_Visibility, queryParams))
	{
		// Blocking hit! Process damage

		AActor* HitActor = Hit.GetActor();
		UGameplayStatics::ApplyPointDamage(HitActor, 20.f, shotDirection, Hit, owner->GetInstigatorController(), this, DamageType);

		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}
		tracerEndPoint = Hit.ImpactPoint;
	}

	DrawDebugLine(GetWorld(), eyeLocation, traceEnd, FColor::White, false, 1.f, 0, 1.f);

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
			tracerComp->SetVectorParameter("Target", tracerEndPoint);
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

