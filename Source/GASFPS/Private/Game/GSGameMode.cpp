// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GSGameMode.h"

#include "Character/GSPawnData.h"
#include "Character/GSPawnExtensionComponent.h"
#include "Player/GSPlayerState.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GSGameMode)


AGSGameMode::AGSGameMode()
{
}

const UGSPawnData* AGSGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AGSPlayerState* GSPS = InController->GetPlayerState<AGSPlayerState>())
		{
			if (const UGSPawnData* PawnData = GSPS->GetPawnData<UGSPawnData>())
			{
				return PawnData;
			}
		}
	}

	return DefaultPawnData;
}

UClass* AGSGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UGSPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AGSGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UGSPawnExtensionComponent* PawnExtComp = UGSPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UGSPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}
