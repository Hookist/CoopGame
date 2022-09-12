// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class URadialForceComponent;
class UStaticMeshComponent;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing=OnRep_IsExploded)
	bool bIsExploded = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Impulse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LaunchCharacterMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SelfLaunchImpulseZ;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ExplosionParticle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	URadialForceComponent* RadialForceComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USHealthComponent* SHealtComp;
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleHealthChanged(USHealthComponent* HealtComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void Server_LaunchSelf();

	UFUNCTION()
	void LaunchSelf();

	UFUNCTION(NetMulticast, Reliable)
	void Server_SetExplodedMaterial();

	UFUNCTION()
	void OnRep_IsExploded();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
