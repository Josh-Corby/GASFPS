// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Costs/GSAbilityCost_ItemTagStack.h"
#include "AbilitySystem/Abilities/GSGameplayAbility.h"

UGSAbilityCost_ItemTagStack::UGSAbilityCost_ItemTagStack()
{
	Quantity.SetValue(1.f);
}

bool UGSAbilityCost_ItemTagStack::CheckCost(const UGSGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
	const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
	const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
	const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

	return false;
}

void UGSAbilityCost_ItemTagStack::ApplyCost(const UGSGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
	}
}
