// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GSGameplayAbility.h"
#include "GSGameplayAbility_RangedWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSGameplayAbility_RangedWeapon : public UGSGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGSGameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UGameplayAbility Interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	// End UGameplayAbility Interface

protected:
	
	struct FRangedWeaponFiringInput
	{
		FVector StartTrace;
		FVector EndAim;
		FVector AimDir;

		FRangedWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
			, AimDir(ForceInitToZero)
		{
		}
	};

	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);

	UFUNCTION(BlueprintCallable) // ability logic starts from this blueprint call
	void StartRangedWeaponTargetting();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	void WeaponTrace(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits) const;

	FHitResult DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, OUT TArray<FHitResult>& OutHits) const;

private:
	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
