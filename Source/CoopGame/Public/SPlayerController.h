// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class USoundCue* GameMainSound;

	UPROPERTY()
	class UAudioComponent* AudioComponent;

	UPROPERTY()
	uint8 PlayerId;

public:

	UFUNCTION()
	void SetPlayerId(uint8 NewPlayerId);

protected:

	virtual void BeginPlayingState() override;
};
