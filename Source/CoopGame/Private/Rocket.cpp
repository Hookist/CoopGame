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

ARocket::ARocket()
{
	bReplicates = true;
	Damage = 50.f;
}

void ARocket::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddUniqueDynamic(this, &ARocket::HandleOnCollisonCompHit);
}

void ARocket::HandleOnCollisonCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//AddRadialImpulseToPhysicBodyComponents();
	//LaunchCharactersInRadius(Hit);
	Server_AddRadialImpulseToPhysicBodyComponents();
	Server_LaunchCharactersInRadius(Hit);
	
	if (GetLocalRole() == ROLE_Authority)
	{ 
		UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, Hit.Location, ExplosionRadius, UDamageType::StaticClass(), {}, this, GetInstigatorController(), true);\
	}
	Super::HandleOnCollisonCompHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
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

void ARocket::Server_AddRadialImpulseToPhysicBodyComponents_Implementation()
{
	AddRadialImpulseToPhysicBodyComponents();
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

void ARocket::Server_LaunchCharactersInRadius_Implementation(const FHitResult& Hit)
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

	//LaunchCharactersInRadius(Hit);
}
