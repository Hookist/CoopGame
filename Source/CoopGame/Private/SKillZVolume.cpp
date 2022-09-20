// Fill out your copyright notice in the Description page of Project Settings.


#include "SKillZVolume.h"
#include "SGameMode_CoopFight.h"
#include "Kismet/GameplayStatics.h"

void ASKillZVolume::ActorEnteredVolume(class AActor* Other)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ASGameMode_CoopFight* GM = Cast<ASGameMode_CoopFight>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{ 
			auto ctrl = Other->GetInstigatorController();
			GM->OnActorKilled.Broadcast(Other, ctrl, this, nullptr);
		}
	}
	Super::ActorEnteredVolume(Other);
}
