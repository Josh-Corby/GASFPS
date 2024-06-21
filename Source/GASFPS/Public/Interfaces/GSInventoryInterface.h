// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSInventoryInterface.generated.h"

class AGSWeapon;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGSInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASFPS_API IGSInventoryInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	bool AddWeaponToInventory(AGSWeapon* NewWeapon, bool bEquipWeapon = false);

	UFUNCTION(BlueprintNativeEvent)
	AGSWeapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintNativeEvent)
	FName GetWeaponAttachPoint() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetFirstPersonMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetThirdPersonMesh() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsInFirstPersonPerspective() const;
};
