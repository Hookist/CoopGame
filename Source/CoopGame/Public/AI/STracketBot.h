// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracketBot.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTracketBot : public APawn
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* AISphereComp;

	//Next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosiveEffect;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosianRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosianDamage;

	FTimerHandle TimerHandle_SelfDamage;

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamageInterval;

	FTimerHandle TimerHandle_CheckNearbyAI;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_PowerLevel, Category = "TrackerBot")
	int32 PowerLevel;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "TrackerBot")
	int32 MaxPowerLevel;

	FTimerHandle TimerHandle_RefreshPath;

public:
	// Sets default values for this pawn's properties
	ASTracketBot();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPathPoint();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* _HealtComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void SelfDestruct();

	void DamageSelf();

	UFUNCTION()
	void AISphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AISphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_PowerLevel(int32 OldPowerLevel);

	bool CheckIsAIControlled(AActor* actor);

	void SetPowerLevel(int32 NewVal);

	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
