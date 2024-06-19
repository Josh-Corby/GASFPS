// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * 
 */
struct FGSGameplayTags
{
public:

	static const FGSGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look;

	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_None;
	FGameplayTag Debuff_Stun;

	FGameplayTag Damage_Normal;
	FGameplayTag Damage_Electric;

	FGameplayTag Effects_Stun;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

private:

	static FGSGameplayTags GameplayTags;
};