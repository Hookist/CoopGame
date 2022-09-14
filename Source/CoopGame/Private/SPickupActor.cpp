// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include <Components/SphereComponent.h>
#include <Components/DecalComponent.h>
#include "SPowerupActor.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(75.f);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(SphereComp);
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetRelativeRotation(FRotator(90, 0.f, 0.f));

	CooldownDuration = 10.f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Powerup is nullptr int %s. Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform transform = GetTransform();
	transform.AddToTranslation(FVector(0, 0, 50));

	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerUpClass, transform, spawnParams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// #TODO: Grant a powerup to player if available
	if (GetLocalRole() == ROLE_Authority && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup(OtherActor);
		PowerupInstance = nullptr;

		// Set Timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CooldownDuration);
	}

}
