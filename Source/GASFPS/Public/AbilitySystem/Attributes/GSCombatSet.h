// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GSAttributeSet.h"
#include "GSCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSCombatSet : public UGSAttributeSet
{
	GENERATED_BODY()
	
public:

	UGSCombatSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
	ATTRIBUTE_ACCESSORS(UGSCombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);
};
