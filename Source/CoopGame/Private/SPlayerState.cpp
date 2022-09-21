// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include <Kismet/GameplayStatics.h>

void ASPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
	if (GetLocalRole() == ROLE_Authority)
	{
		PlayRandomSound(ScoreAddedSounds);
	}
}

void ASPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	if (HasLocalNetOwner())
	{
		PlayRandomSound(ScoreAddedSounds);
	}
}

void ASPlayerState::BP_SetPlayerName_Implementation(const FString& S)
{
	SetPlayerName(S);
}

//void ASPlayerState::BP_SetPlayerName(const FString& S)
//{
//	SetPlayerName(S);
//}

void ASPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnNameChanged.Broadcast(GetPlayerName());
}

void ASPlayerState::PlayRandomSound(TArray<USoundBase*> Sounds)
{
	if (Sounds.Num() > 0)
	{
		USoundBase* sound = Sounds[FMath::RandRange(0, Sounds.Num() - 1)];
		if (sound)
			UGameplayStatics::PlaySound2D(this, sound);
	}
}
