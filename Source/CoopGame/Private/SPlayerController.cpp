// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>

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