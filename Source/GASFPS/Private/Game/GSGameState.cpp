// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GSGameState.h"

#include "Game/GSPlayerSpawningManagerComponent.h"

AGSGameState::AGSGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawningComponent = CreateDefaultSubobject<UGSPlayerSpawningManagerComponent>("SpawningComponent");
}
