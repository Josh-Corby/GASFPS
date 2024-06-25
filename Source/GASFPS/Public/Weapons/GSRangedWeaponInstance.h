// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/GSWeaponInstance.h"
#include "AbilitySystem/GSAbilitySourceInterface.h"

#include "GSRangedWeaponInstance.generated.h"

/**
 * UGSRangedWeaponInstance
 *
 * A piece of equipment representing a ranged weapon spawned and applied to a pawn
 */
UCLASS()
class GASFPS_API UGSRangedWeaponInstance : public UGSWeaponInstance, public IGSAbilitySourceInterface
{
	GENERATED_BODY()
	
public:

	UGSRangedWeaponInstance(const FObjectInitializer& ObjectInitializer);


	//~UGSEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UGSEquipmentInstance interface
};
