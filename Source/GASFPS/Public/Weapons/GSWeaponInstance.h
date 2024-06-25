// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Equipment/GSEquipmentInstance.h"

#include "GSWeaponInstance.generated.h"

/**
 * UGSWeaponInstance
 *
 * A piece of equipment representing a weapon spawned and applied to a pawn
 */
UCLASS()
class GASFPS_API UGSWeaponInstance : public UGSEquipmentInstance
{
	GENERATED_BODY()
	
public:

	UGSWeaponInstance(const FObjectInitializer& ObjectInitializer);

	//~UGSEquipmentInstance interface
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
	//~End of UGSEquipmentInstance interface

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	// Returns how long it's been since the weapon was interacted with (fired or equipped)
	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

private:

	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;
};
