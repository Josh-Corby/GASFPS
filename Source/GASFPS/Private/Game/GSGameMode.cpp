// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GSGameMode.h"

#include "GameFramework/SpectatorPawn.h"
#include "Character/GSPlayerCharacter.h"
#include "Character/GSAICharacter.h"
#include "Player/GSPlayerController.h"

AGSGameMode::AGSGameMode()
{
}

void AGSGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AGSGameMode::PlayerDied(AController* Controller)
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(Controller);
	if(!PC)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParams);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGSGameMode::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void AGSGameMode::RespawnPlayer(AController* Controller)
{
	if (!IsValid(Controller))
	{
		return;
	}

	if (Controller->IsPlayerController())
	{
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGSPlayerCharacter* Player = GetWorld()->SpawnActor<AGSPlayerCharacter>(PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Player);

		AGSPlayerController* PC = Cast<AGSPlayerController>(Controller);
		if (PC)
		{
			PC->ClientSetControlRotation(PlayerStart->GetActorRotation());
		}
	}
}
