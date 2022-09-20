// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode_CoopFight.h"
#include "SGameState.h"
#include "Components/SHealthComponent.h"
#include <EngineUtils.h>
#include "GameFramework/PlayerStart.h"

void ASGameMode_CoopFight::StartPlay()
{
	Super::StartPlay();

	OnActorKilled.AddUniqueDynamic(this, &ASGameMode_CoopFight::HandleActorKilled);
}

AActor* ASGameMode_CoopFight::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	TArray<APlayerStart*> playerStarts = TArray<APlayerStart*>();
	for (TActorIterator<APlayerStart> iterator = TActorIterator<APlayerStart>(GetWorld()); iterator; ++iterator)
	{
		playerStarts.Add(*iterator);
	}

	int32 min = 0;
	int32 max = playerStarts.Num() - 1;
	return playerStarts[FMath::RandRange(min, max)];
}

void ASGameMode_CoopFight::HandleActorKilled(AActor* VictimActor, AController* VictimController, AActor* KillerActor, AController* KillerController)
{
	FTimerHandle TimerHandle_RespawnPlayer;
	FTimerDelegate TimerDelegate_RespawnPlayer;
	TimerDelegate_RespawnPlayer.BindUFunction(this, "RespawnPlayer", VictimController);
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnPlayer, TimerDelegate_RespawnPlayer, 5.f, false);
}

void ASGameMode_CoopFight::RespawnPlayer(APlayerController* playerController)
{
	RestartPlayer(playerController);
	
	ASGameState* GS = GetGameState<ASGameState>();
	if (GS)
	{
		GS->OnPawnRespawned.Broadcast(playerController);
	}
}
