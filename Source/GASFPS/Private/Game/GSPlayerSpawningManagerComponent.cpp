// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GSPlayerSpawningManagerComponent.h"

#include "EngineUtils.h"

#include "Player/GSPlayerStart.h"

UGSPlayerSpawningManagerComponent::UGSPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(false);
	bAutoRegister = true;
	bAutoActivate = true;
	bWantsInitializeComponent = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UGSPlayerSpawningManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::HandleOnActorSpawned));

	for (TActorIterator<AGSPlayerStart> It(World); It; ++It)
	{
		if (AGSPlayerStart* PlayerStart = *It)
		{
			CachedPlayerStarts.Add(PlayerStart);
		}
	}
}

void UGSPlayerSpawningManagerComponent::HandleOnActorSpawned(AActor* SpawnedActor)
{
	if (AGSPlayerStart* PlayerStart = Cast<AGSPlayerStart>(SpawnedActor))
	{
		CachedPlayerStarts.Add(PlayerStart);
	}
}
