// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GSGameMode.generated.h"

class AGSPlayerCharacter;
class AGSAICharacter;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGSGameMode();

	void PlayerDied(AController* Controller);

protected:

	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 5.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGSPlayerCharacter> PlayerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGSAICharacter> AIClass;

	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);
};
