// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GSGameplayAbility.h"
#include "GSGameplayAbility_FromWeapon.generated.h"

class AGSRangedWeapon;
/**
 * 
 */
UCLASS()
class GASFPS_API UGSGameplayAbility_FromWeapon : public UGSGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGSGameplayAbility_FromWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	AGSRangedWeapon* GetAssociatedWeapon() const;
};
