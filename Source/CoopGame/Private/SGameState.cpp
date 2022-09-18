// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include <Net/UnrealNetwork.h>

void ASGameState::SetWaveState(EWaveState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EWaveState oldState = WaveState;
		WaveState = NewState;
		// Call on server
		OnRep_WaveState(oldState);
	}
}

void ASGameState::OnRep_WaveState(EWaveState PreviousWaveState)
{
	WaveStateChanged(WaveState, PreviousWaveState);
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}

