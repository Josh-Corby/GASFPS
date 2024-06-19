// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GSAbilitySet.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"

UGSAbilitySet::UGSAbilitySet(const FObjectInitializer& ObjectInitializer)
{
}

void UGSAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* ASC, UObject* SourceObject) const
{
	check(ASC);
	
	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedAbilities.Num(); AbilityIndex++)
	{
		const FGSAbiltySet_GameplayAbility& AbilityToGrant = GrantedAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UGSGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGSGameplayAbility>();
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	}
}
