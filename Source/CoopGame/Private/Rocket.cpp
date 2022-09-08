// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Components/SphereComponent.h>
#include "SCharacter.h"

void ARocket::PreEndPlay_Implementation()
{
	Super::PreEndPlay_Implementation();
}

void ARocket::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddUniqueDynamic(this, &ARocket::HandleOnCollisonCompHit);
}

void ARocket::HandleOnCollisonCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AddRadialImpulseToPhysicBodyComponents();
	LaunchCharactersInRadius(Hit);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 40.f, Hit.Location, ExplosionRadius, UDamageType::StaticClass(), {}, this, GetInstigatorController(), true);
	Destroy();
}

void ARocket::AddRadialImpulseToPhysicBodyComponents()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	TArray<UPrimitiveComponent*> outComponents;
	UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), GetActorLocation(), ExplosionRadius, objectTypes, UPrimitiveComponent::StaticClass(), {}, outComponents);

	for (UPrimitiveComponent* comp : outComponents)
	{
		comp->AddRadialImpulse(CollisionComp->GetComponentLocation(), ExplosionRadius, PhysicBodyImpulseStrength, ERadialImpulseFalloff::RIF_Linear, false);
	}
}

void ARocket::LaunchCharactersInRadius(const FHitResult& Hit)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> outActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Hit.Location, ExplosionRadius, objectTypes, ASCharacter::StaticClass(), {}, outActors);
	
	for (AActor* actor : outActors)
	{
		ASCharacter* character = CastChecked<ASCharacter>(actor);
		FVector launchVelocity = (character->GetActorLocation() - Hit.Location).GetSafeNormal() * LaunchVelosityMultiplier;
		character->LaunchCharacter(launchVelocity, true, true);
	}
}
