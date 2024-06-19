// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "GSAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
