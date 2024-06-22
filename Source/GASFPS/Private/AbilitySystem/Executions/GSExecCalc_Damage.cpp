// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/GSExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "GSGameplayTags.h"
#include "GSAbilitySystemLibrary.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGSHealthSet, Damage, Target, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UGSExecCalc_Damage::UGSExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UGSExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();

	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	DetermineDebuff(Spec, ExecutionParams, EvaluateParameters);

	float Damage = 0.0f;

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		if (DamageTypeValue <= 0)
		{
			continue;
		}

		if (DamageTypeTag.MatchesTagExact(GameplayTags.Damage_InstaKill))
		{
			DamageTypeValue = 1000;
		}

		Damage += DamageTypeValue;
	}

	if (Damage > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, Damage));
	}
}

void UGSExecCalc_Damage::DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvalParams) const
{
	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();

	for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;

		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > -.5f) // .5 padding for floating point imprecision
		{
			FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

			UGSAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
			UGSAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

			const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);

			UGSAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
		}
	}
}
