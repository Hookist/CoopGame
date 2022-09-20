// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/KillZVolume.h"
#include "SKillZVolume.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASKillZVolume : public AKillZVolume
{
	GENERATED_BODY()

	//Begin PhysicsVolume Interface
	virtual void ActorEnteredVolume(class AActor* Other) override;
	//End PhysicsVolume Interface
	
};
