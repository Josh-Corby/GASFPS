// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GSPickupDefinition.generated.h"

class UGSInventoryItemDefinition;
class UNiagaraSystem;

/**
 * 
 */

UCLASS()
class GASFPS_API UGSPickupDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup|Equipment")
	TSubclassOf<UGSInventoryItemDefinition> InventoryItemDefinition;

	//Visual representation of the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup|Mesh")
	TObjectPtr<UStaticMesh> DisplayMesh;

	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup")
	int32 SpawnCoolDownSeconds;

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup")
	TObjectPtr<USoundBase> PickedUpSound;

	//Sound to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup")
	TObjectPtr<USoundBase> RespawnedSound;

	//Particle FX to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup")
	TObjectPtr<UNiagaraSystem> PickedUpEffect;

	//Particle FX to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup")
	TObjectPtr<UNiagaraSystem> RespawnedEffect;
};


UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "GS Weapon Pickup Data", ShortTooltip = "Data asset used to configure a weapon pickup."))
class GASFPS_API UGSWeaponPickupDefinition : public UGSPickupDefinition
{
	GENERATED_BODY()

public:

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup|Mesh")
	FVector WeaponMeshOffset;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GS|Pickup|Mesh")
	FVector WeaponMeshScale = FVector(1.0f, 1.0f, 1.0f);
};
