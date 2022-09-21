// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>
#include "SPlayerState.h"

void ASPlayerController::SetPlayerId(uint8 NewPlayerId)
{
	PlayerId = NewPlayerId;
	ASPlayerState* playerState = GetPlayerState<ASPlayerState>();
	playerState->SetPlayerId(PlayerId);
}

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	if (!IsLocalController())
	{
		return;
	}

	if (AudioComponent)
	{
		AudioComponent->Deactivate();
	}

	if (GameMainSound)
	{ 
		AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), GameMainSound);
	}
}