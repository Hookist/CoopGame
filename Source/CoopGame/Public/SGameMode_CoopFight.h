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

	// AActor*, VictimActor, AController*, VictimController, AActor*, KillerActor, AController*, KillerController
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:

	UPROPERTY()
	uint8 NextPlayerId;

private:

	TArray<APlayerStart*> PlayerStarts;

public:

	ASGameMode_CoopFight();

	virtual void StartPlay() override;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

private:
	
	UFUNCTION()
	void HandleActorKilled(AActor* VictimActor, AController* VictimController, AActor* KillerActor, AController* KillerController);

	UFUNCTION()
	void RespawnPlayer(APlayerController* ControllerOfRespawnedPawn);
	
};
