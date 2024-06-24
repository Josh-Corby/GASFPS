// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularGameState.h"

#include "GSGameState.generated.h"

class UGSPlayerSpawningManagerComponent;
/**
 * 
 */
UCLASS(Config = Game)
class GASFPS_API AGSGameState : public AModularGameStateBase
{
	GENERATED_BODY()

public:

	AGSGameState(const FObjectInitializer& ObjectInitializer);

private:
	TObjectPtr<UGSPlayerSpawningManagerComponent> SpawningComponent;
};
