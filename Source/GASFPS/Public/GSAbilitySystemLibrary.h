// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "GSAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
struct FDamageEffectParams;
/**
 * 
 */
UCLASS()
class GASFPS_API UGSAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "GSAbilitySystemLibrary")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& Params);

	// Effect Context Getters

	UFUNCTION(BlueprintPure, Category = "GSAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "GSAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	// Effect Context Setters

	UFUNCTION(BlueprintCallable, Category = "GSAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff);

	UFUNCTION(BlueprintCallable, Category = "GSAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category = "GSAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration);

};
