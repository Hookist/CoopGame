// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode_CoopFight.h"
#include "SGameState.h"
#include "Components/SHealthComponent.h"
#include <EngineUtils.h>
#include "GameFramework/PlayerStart.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/PlayerState.h"

ASGameMode_CoopFight::ASGameMode_CoopFight()
{
	NextPlayerId = 1;
}

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

FString ASGameMode_CoopFight::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString playerName = "";
	playerName = UGameplayStatics::ParseOption(Options, TEXT("name"));
	playerName = playerName == "" ? TEXT("NoName") : playerName;

	APlayerState* PS = NewPlayerController->PlayerState;
	auto GS = GetGameState<ASGameState>();

	if (GS)
	{
		GS->OnNewPlayerLogin.Broadcast(FString::FromInt(PS->GetPlayerId()), playerName);
	}

	NewPlayerController->PlayerState->SetPlayerName(playerName == "" ? TEXT("NoName") : playerName);

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
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
