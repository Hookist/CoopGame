// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	PreparingNextWave,
	WaveInProgress,
	WaitingToComplete,
	WaveComplete,
	GameOver
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnRespawned, APlayerController*, ControllerOfSpawnedPawn);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewPlayerLogin, FString, UniqueId, FString, PlayerName);

public:

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnPawnRespawned OnPawnRespawned;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnNewPlayerLogin OnNewPlayerLogin;

protected:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public: 

	void SetWaveState(EWaveState NewState);

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState PreviousWaveState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);
};
