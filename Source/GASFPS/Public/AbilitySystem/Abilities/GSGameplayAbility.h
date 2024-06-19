// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSAbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "GSGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EGSAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

class UGSAbilityCost;
/**
 * 
 */
UCLASS(Abstract)
class GASFPS_API UGSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGSGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr);

protected:

	AController* GetControllerFromActorInfo() const;

	// UGameplayAbilityInterface
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	//End UGameplayAbility interface

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	void GiveEffectTag(AActor* TargetActor, float InEffectDuration);

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Costs")
	TArray<TObjectPtr<UGSAbilityCost>> AdditionalCosts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGSAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;
};
