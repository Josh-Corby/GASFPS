// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GSAbilitySystemGlobals.h"

#include "GSAbilityTypes.h"

FGameplayEffectContext* UGSAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FGSGameplayEffectContext();
}
