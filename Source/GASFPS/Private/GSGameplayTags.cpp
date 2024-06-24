// Fill out your copyright notice in the Description page of Project Settings.


#include "GSGameplayTags.h"

#include "GameplayTagsManager.h"

FGSGameplayTags FGSGameplayTags::GameplayTags;;


void FGSGameplayTags::InitializeNativeGameplayTags()
{
		GameplayTags.InitState_Spawned = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InitState.Spawned"),
			FString("1: Actor/component has initially spawned and can be extended")
		);

		GameplayTags.InitState_DataAvailable = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InitState.DataAvailable"),
			FString("2: All required data has been loaded/replicated and is ready for initialization")
		);

		GameplayTags.InitState_DataInitialized = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InitState.DataInitialized"),
			FString("3: The available data has been initialized for this actor/component, but it is not ready for full gameplay")
		);

		GameplayTags.InitState_GameplayReady = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InitState.GameplayReady"),
			FString("4: The actor/component is fully ready for active gameplayd")
		);

		GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.Move"),
			FString("Move Input.")
		);

		GameplayTags.InputTag_Look = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.Look"),
			FString("Look Input.")
		);

		GameplayTags.InputTag_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag.Interact"),
			FString("Interact Input.")
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

		GameplayTags.Damage_InstaKill = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Damage.InstaKill"),
			FString("InstaKill Damage Tag.")
		);

		GameplayTags.Effects_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Effects.Stun"),
			FString("Stun Tag.")
		);

		GameplayTags.State_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("State.Dead"),
			FString("Dead Tag.")
		);

		GameplayTags.State_KnockedDown = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("State.KnockedDown"),
			FString("Knocked Down Tag.")
		);

		GameplayTags.State_Interacting_Added = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("State.Interacting.Added"),
			FString("Interacting Added Tag.")
		);

		GameplayTags.State_Interacting_Removed = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("State.Interacting.Removed"),
			FString("Interacting Removed Tag.")
		);

		GameplayTags.Ability_Interaction_Start = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Ability.Interaction.Start"),
			FString("Interaction Start Tag.")
		);

		GameplayTags.Ability_Interaction_End = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Ability.Interaction.End"),
			FString("Interaction End Tag.")
		);

		GameplayTags.Ability_Interaction_Block = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Ability.Interaction.Block"),
			FString("Interaction Block Tag.")
		);

		GameplayTags.Abilities_Revive = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Abilities.Revive"),
			FString("Revive Ability Tag.")
		);

		GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Normal, GameplayTags.Debuff_None);
		GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Electric, GameplayTags.Debuff_Stun);
		GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_InstaKill, GameplayTags.Debuff_None);
}
