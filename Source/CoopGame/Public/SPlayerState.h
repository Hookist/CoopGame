// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNameChanged, FString, NewName);

public:

	UPROPERTY(BlueprintAssignable)
	FOnNameChanged OnNameChanged;

protected:

	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> ScoreAddedSounds;
	
public:

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	virtual void OnRep_Score() override;

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "PlayerState")
	void BP_SetPlayerName(const FString& S);

	virtual void OnRep_PlayerName() override;

private:

	void PlayRandomSound(TArray<USoundBase*> Sounds);
};
