// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "GSPawnData.generated.h"

class APawn;
class UGSAbilitySet;
class UGSAbilityTagRelationshipMapping;
class UGSInputConfig;
class UObject;
/**
 * UGSPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Gas Shooter Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class GASFPS_API UGSPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UGSPawnData(const FObjectInitializer& ObjectInitializer);

	// Class to instantiate for this pawn (should usually derive from AGSPawn or AGSCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Abilities")
	TArray<TObjectPtr<UGSAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Abilities")
	TObjectPtr<UGSAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Input")
	TObjectPtr<UGSInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Camera")
	//TSubclassOf<UGSCameraMode> DefaultCameraMode;
};
