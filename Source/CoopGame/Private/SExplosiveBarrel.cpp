// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include <Components/StaticMeshComponent.h>
#include <PhysicsEngine/RadialForceComponent.h>
#include "Components/SHealthComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "SCharacter.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	SetRootComponent(StaticMeshComp);
	StaticMeshComp->SetCollisionProfileName("BlockAllDynamic");
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Challenges/ExplosiveBarrel/SM_ExplosiveBarrel.SM_ExplosiveBarrel'"));

	if (meshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(meshAsset.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterialInterface> defaultMaterialAsset(TEXT("Material'/Game/Challenges/ExplosiveBarrel/M_ExplosiveBarrel.M_ExplosiveBarrel'"));

	if (defaultMaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, ExplodedMaterial);
	}

	ConstructorHelpers::FObjectFinder<UMaterialInterface> explodedMaterialAsset(TEXT("Material'/Game/Challenges/ExplosiveBarrel/M_ExplosiveBarrelExploded.M_ExplosiveBarrelExploded'"));

	if (explodedMaterialAsset.Succeeded())
	{
		ExplodedMaterial = explodedMaterialAsset.Object;
	}


	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(StaticMeshComp);
	RadialForceComp->SetAutoActivate(false);

	SHealtComp = CreateDefaultSubobject<USHealthComponent>("SHealthComp");

	SetReplicates(true);
	SetReplicatingMovement(true);

	Impulse = 3000.f;
	Radius = 700.f;
	LaunchCharacterMultiplier = 500.f;
	bIsExploded = false;
	SelfLaunchImpulseZ = 300.f;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	RadialForceComp->ImpulseStrength = Impulse;
	RadialForceComp->Radius = Radius;

	SHealtComp->OnHealthChanged.AddUniqueDynamic(this, &ASExplosiveBarrel::HandleHealthChanged);
	
}

void ASExplosiveBarrel::HandleHealthChanged(USHealthComponent* HealtComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsExploded)
		return;

	if (Health <= 0.f)
	{
		
		StaticMeshComp->SetSimulatePhysics(true);

		// radius impulse for objects to fly away
		RadialForceComp->FireImpulse();

		//Server_SetExplodedMaterial();
		
		// set explod to true so it won't 'explode' again
		bIsExploded = true;
		OnRep_IsExploded();

		LaunchSelf();

		// Get characters in radius of explosion and launch them away and apply radial damage
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = TArray<TEnumAsByte<EObjectTypeQuery>>();
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> outActors;
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), StaticMeshComp->GetComponentLocation(), Radius, objectTypes, ASCharacter::StaticClass(), {}, outActors);

		for (AActor* actor : outActors)
		{
			if (!IsValid(actor))
				continue;

			ASCharacter* character = CastChecked<ASCharacter>(actor);
			FVector direction = (GetActorLocation() - character->GetActorLocation()).GetSafeNormal();
			character->LaunchCharacter(direction * LaunchCharacterMultiplier, true, true);
			
			UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.f, GetActorLocation(), Radius, UDamageType::StaticClass(), {}, this);
		}
	}
}

void ASExplosiveBarrel::Server_LaunchSelf_Implementation()
{
	// Launch barrel in the air
	StaticMeshComp->AddImpulse(FVector(0.f, 0.f, SelfLaunchImpulseZ), NAME_None, true);
}

void ASExplosiveBarrel::LaunchSelf()
{
	// Launch barrel in the air
	StaticMeshComp->AddImpulse(FVector(0.f, 0.f, SelfLaunchImpulseZ), NAME_None, true);
}

void ASExplosiveBarrel::Server_SetExplodedMaterial_Implementation()
{
	// change barrel material to exploded one
	if (ExplodedMaterial)
	{
		StaticMeshComp->SetMaterial(0, ExplodedMaterial);
	}
	// Show explode effect
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ExplosionParticle, StaticMeshComp, NAME_None);
	}
}

void ASExplosiveBarrel::OnRep_IsExploded()
{
	// change barrel material to exploded one
	if (ExplodedMaterial)
	{
		StaticMeshComp->SetMaterial(0, ExplodedMaterial);
	}
	// Show explode effect
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ExplosionParticle, StaticMeshComp, NAME_None);
	}
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bIsExploded);
}


