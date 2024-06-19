// Fill out your copyright notice in the Description page of Project Settings.


#include "GSGameplayTags.h"

#include "GameplayTagsManager.h"

FGSGameplayTags FGSGameplayTags::GameplayTags;;


void FGSGameplayTags::InitializeNativeGameplayTags()
{
		GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.Move"),
			FString("Move Input.")
		);

		GameplayTags.InputTag_Look = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.Look"),
			FString("Look Input.")
		);

		GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Debuff.Duration"),
			FString("Debuff Duration.")
		);

		GameplayTags.Debuff_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Debuff.None"),
			FString("None Debuff Tag.")
		);

		GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Debuff.Stun"),
			FString("Stun Debuff Tag.")
		);

		GameplayTags.Damage_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Normal"),
			FString("Basic Damage Tag.")
		);

		GameplayTags.Damage_Electric = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.Electric"),
			FString("Electric Damage Tag.")
		);

		GameplayTags.Effects_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Effects.Stun"),
			FString("Stun Tag.")
		);

		GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Normal, GameplayTags.Debuff_None);
		GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Electric, GameplayTags.Debuff_Stun);
}
