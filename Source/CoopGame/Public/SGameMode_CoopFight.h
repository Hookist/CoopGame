// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode_CoopFight.generated.h"

class APlayerStart;

/**
 * Players can shoot each other
 * Each new player will have unique team Id
 * if player kill someone then he got scores
 * if player was killed then after some time he will respawn on new place
 * Match ends after one of the player reached the Max score number that set on server
 */
UCLASS()
class COOPGAME_API ASGameMode_CoopFight : public AGameModeBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnActorKilled, AActor*, VictimActor, AController*, VictimController, AActor*, KillerActor, AController*, KillerController);

public:

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

private:

	TArray<APlayerStart*> PlayerStarts;

public:

	virtual void StartPlay() override;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

private:
	
	UFUNCTION()
	void HandleActorKilled(AActor* VictimActor, AController* VictimController, AActor* KillerActor, AController* KillerController);

	UFUNCTION()
	void RespawnPlayer(APlayerController* ControllerOfRespawnedPawn);
	
};