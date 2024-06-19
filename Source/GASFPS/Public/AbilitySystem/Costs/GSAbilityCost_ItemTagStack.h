// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Costs/GSAbilityCost.h"
#include "GSAbilityCost_ItemTagStack.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Item Tag Stack"))
class GASFPS_API UGSAbilityCost_ItemTagStack : public UGSAbilityCost
{
	GENERATED_BODY()
	
public:

	UGSAbilityCost_ItemTagStack();

	// UGSAbilityCost interface
	virtual bool CheckCost(const UGSGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UGSGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	// End UGSAbilityCost interface

protected:

	/** How much of the tag to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FScalableFloat Quantity;

	/** Which tag to spend some of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag Tag;

};
