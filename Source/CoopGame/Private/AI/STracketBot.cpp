// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STracketBot.h"
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <NavigationSystem.h>
#include <NavigationPath.h>
#include <DrawDebugHelpers.h>
#include "Components/SHealthComponent.h"
#include <Components/SphereComponent.h>
#include "SCharacter.h"
#include <Sound/SoundCue.h>
#include <Net/UnrealNetwork.h>
#include <AIController.h>

// Sets default values
ASTracketBot::ASTracketBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	AISphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AISphereComp"));
	AISphereComp->SetSphereRadius(1000);
	AISphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AISphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	AISphereComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	AISphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000.f;
	RequiredDistanceToTarget = 100.f;

	ExplosianDamage = 40.f;
	ExplosianRadius = 200.f;

	bStartedSelfDestruction = false;

	SelfDamageInterval = 0.25f;
	PowerLevel = 0;
	MaxPowerLevel = 3;
}

void ASTracketBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartedSelfDestruction || bExploded)
		return;

	ASCharacter* playerPawn = Cast<ASCharacter>(OtherActor);
	if (playerPawn)
	{
		// we overlapped with a player!

		if (GetLocalRole() == ROLE_Authority)
		{ 
			// Start self destruction sequence
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTracketBot::DamageSelf, SelfDamageInterval, true, 0.f);
		}

		bStartedSelfDestruction = true;

		UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
	}
}

// Called when the game starts or when spawned
void ASTracketBot::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddUniqueDynamic(this, &ASTracketBot::HandleTakeDamage);
	AISphereComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASTracketBot::AISphereBeginOverlap);
	AISphereComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ASTracketBot::AISphereEndOverlap);

	MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

	if (GetLocalRole() == ROLE_Authority)
	{ 
		// Find initial move-to
		NextPathPoint = GetNextPathPoint();

		TArray<AActor*> overlappingActors;
		AISphereComp->GetOverlappingActors(overlappingActors);

		for (AActor* actor : overlappingActors)
		{
			if (CheckIsAIControlled(actor))
			{
				SetPowerLevel(PowerLevel + 1);
			}
		}
	}
}

FVector ASTracketBot::GetNextPathPoint()
{
	// Hack, to get player location
	ACharacter* playerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!playerPawn)
		return GetActorLocation();

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), GetActorLocation(), playerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		// Return next point in the path
		return NavPath->PathPoints[1];
	}

	//Failed to find path
	return GetActorLocation();
}

void ASTracketBot::HandleTakeDamage(USHealthComponent* _HealtComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (Health <= 0.f)
	{
		SelfDestruct();
	}
}

void ASTracketBot::SelfDestruct()
{
	if (bExploded)
		return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetLocalRole() == ROLE_Authority)
	{ 
		TArray<AActor*> ignoredActors;
		ignoredActors.Add(this);

		float damageToApply = ExplosianDamage + (ExplosianDamage * PowerLevel);
		// Apply Damage!
		UGameplayStatics::ApplyRadialDamage(this, damageToApply, GetActorLocation(), ExplosianRadius, UDamageType::StaticClass(), ignoredActors, this, GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosianRadius, 12, FColor::Red, false, 2.f, 0.f, 1.f);

		HealthComp->OnHealthChanged.RemoveAll(this);
		SetLifeSpan(2.f);
	}
}

void ASTracketBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.f, GetInstigatorController(), this, nullptr);
}

void ASTracketBot::AISphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (!CheckIsAIControlled(OtherActor))
		return;
			
	PowerLevel += 1;

	// Call it for Server because OnRep_PowerLevel won't be called if it also Client
	OnRep_PowerLevel(PowerLevel - 1);
}

void ASTracketBot::AISphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (!CheckIsAIControlled(OtherActor))
		return;

	if (PowerLevel > 0)
	{
		PowerLevel -= 1;
		// Call it for Server because OnRep_PowerLevel won't be called if it also Client
		OnRep_PowerLevel(PowerLevel + 1);
	}
}

void ASTracketBot::OnRep_PowerLevel(int32 OldPowerLevel)
{
	if (OldPowerLevel != PowerLevel)
	{
		if (MatInst)
		{
			float powerLevelClamped = FMath::Clamp(PowerLevel, 0, MaxPowerLevel); 
			float alpha = powerLevelClamped > 0  ? (float)powerLevelClamped / (float)MaxPowerLevel : 0;
			MatInst->SetScalarParameterValue("PowerLevelAlpha", alpha);
		}
	}
}

bool ASTracketBot::CheckIsAIControlled(AActor* actor)
{
	if (!actor)
		return false;

	APawn* pawn = Cast<APawn>(actor);

	if (!pawn)
		return false;

	AController* controller = pawn->GetController();

	if (!controller)
		return false;

	AAIController* _AIcontroller = Cast<AAIController>(controller);

	if (!_AIcontroller)
		return false;

	return true;
}

void ASTracketBot::SetPowerLevel(int32 NewVal)
{
	int32 oldValue = PowerLevel;
	PowerLevel = NewVal;
	// Call it for Server because OnRep_PowerLevel won't be called if it also Client
	OnRep_PowerLevel(oldValue);
}

// Called every frame
void ASTracketBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		float distanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (distanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
		}
		else
		{
			// Keep moving towards next target
			FVector forceDirection = NextPathPoint - GetActorLocation();
			forceDirection.Normalize();
			forceDirection *= MovementForce;
			MeshComp->AddForce(forceDirection, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + forceDirection, 32, FColor::Yellow, false, 0.f, 0, 1.f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.f, 1.f);
	}
}

void ASTracketBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTracketBot, PowerLevel);
}



