// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include <Net/UnrealNetwork.h>
#include "SGameMode.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
	bIsDead = false;

	TeamNum = 255;

	SetIsReplicatedByDefault(true);
}


float USHealthComponent::GetHealth() const
{
	return Health;
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Friendly
		return true;
	}
		
	USHealthComponent* healthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* healthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if (healthCompA == nullptr || healthCompB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	return healthCompA->TeamNum == healthCompB->TeamNum;
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hool if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{ 
		AActor* myOwner = GetOwner();
		if (myOwner)
		{
			myOwner->OnTakeAnyDamage.AddUniqueDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
	Health = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
		return;

	if (DamagedActor != DamageCauser && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	//Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{ 
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed : %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}
