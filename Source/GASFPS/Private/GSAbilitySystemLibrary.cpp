// Fill out your copyright notice in the Description page of Project Settings.


#include "GSAbilitySystemLibrary.h"
#include "GSAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GSGameplayTags.h"

FGameplayEffectContextHandle UGSAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	FGameplayEffectContextHandle ContextHandle = Params.SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(Params.SourceASC->GetAvatarActor());

	const FGameplayEffectSpecHandle SpecHandle = Params.SourceASC->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbiltyLevel, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamageType, Params.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGSGameplayTags::Get().Debuff_Duration, Params.DebuffDuration);

	Params.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return ContextHandle;
}

bool UGSAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FGSGameplayEffectContext* GSEffectContext = static_cast<const FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return GSEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

FGameplayTag UGSAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FGSGameplayEffectContext* GSEffectContext = static_cast<const FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (GSEffectContext->GetDamageType().IsValid())
		{
			return *GSEffectContext->GetDamageType();
		}
	}

	return FGameplayTag();
}

float UGSAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FGSGameplayEffectContext* GSEffectContext = static_cast<const FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return GSEffectContext->GetDebuffDuration();
	}
	return 0.0f;
}

void UGSAbilitySystemLibrary::SetIsSuccessfulDebuff(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff)
{
	if (FGSGameplayEffectContext* GSEffectContext = static_cast<FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GSEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UGSAbilitySystemLibrary::SetDamageType(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FGSGameplayEffectContext* GSEffectContext = static_cast<FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		GSEffectContext->SetDamageType(DamageType);
	}
}

void UGSAbilitySystemLibrary::SetDebuffDuration(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	if (FGSGameplayEffectContext* GSEffectContext = static_cast<FGSGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GSEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}
