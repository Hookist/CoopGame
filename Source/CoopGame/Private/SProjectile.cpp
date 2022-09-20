// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectile::ASProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	SetRootComponent(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bReplicates = true;
	ProjectileMovement->SetIsReplicated(true);
}

void ASProjectile::HandleOnCollisonCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->GetCollisionObjectType() == ECC_Pawn) // OtherComp->IsSimulatingPhysics())
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			MakeNoise(1.f, GetInstigator());

			Destroy();
		}
	}
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddUniqueDynamic(this, &ASProjectile::HandleOnCollisonCompHit);	// set up a notification for when this component hits something blocking
}

void ASProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PreEndPlay();
	Super::EndPlay(EndPlayReason);
}

void ASProjectile::PreEndPlay_Implementation()
{
	TArray<AActor*> actorsToIgnore = TArray<AActor*>();
	actorsToIgnore.Add(this);

	if (ImpactEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorTransform());

	UGameplayStatics::ApplyRadialDamage(GetWorld(), 20.f, GetActorLocation(), DamageRadius, UDamageType::StaticClass(), actorsToIgnore);
}
