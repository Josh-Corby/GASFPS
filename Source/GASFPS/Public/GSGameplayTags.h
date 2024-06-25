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

	FGameplayTag Ability_ActivateFail_IsDead;
	FGameplayTag Ability_ActivateFail_ActivationGroup;

	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look;
	FGameplayTag InputTag_Interact;

	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_None;
	FGameplayTag Debuff_Stun;

	FGameplayTag Damage_Normal;
	FGameplayTag Damage_Electric;
	FGameplayTag Damage_InstaKill;

	FGameplayTag Effects_Stun;

	FGameplayTag State_Dead;
	FGameplayTag State_KnockedDown;
	FGameplayTag State_Interacting_Added;
	FGameplayTag State_Interacting_Removed;

	FGameplayTag Ability_Interaction_Start;
	FGameplayTag Ability_Interaction_End;
	FGameplayTag Ability_Interaction_Block;

	FGameplayTag Abilities_Revive;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

private:

	static FGSGameplayTags GameplayTags;
};