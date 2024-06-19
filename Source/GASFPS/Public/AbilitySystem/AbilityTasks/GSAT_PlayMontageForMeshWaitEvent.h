// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GSAT_PlayMontageForMeshWaitEvent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGSPlayMontageForMeshAndWaitForEventDelegate);

class UGSAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class GASFPS_API UGSAT_PlayMontageForMeshWaitEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FGSPlayMontageForMeshAndWaitForEventDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FGSPlayMontageForMeshAndWaitForEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FGSPlayMontageForMeshAndWaitForEventDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FGSPlayMontageForMeshAndWaitForEventDelegate OnCancelled;


	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAbilityCancelled();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGSAT_PlayMontageForMeshWaitEvent* PlayMontageForMeshAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		USkeletalMeshComponent* InMesh,
		UAnimMontage* MontageToPlay,
		float Rate = 1.f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f,
		float StartTimeSeconds = 0.f,
		bool bReplicateMontage = true,
		bool bAllowInterruptAfterBlendOut = false);


	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;

protected:
	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	/** Returns our ability system component */
	UGSAbilitySystemComponent* GetTargetASC() const;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	// Mesh that the Montage is playing on. Must be owned by the AvatarActor.
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	/** Montage that is playing */
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bReplicateMontage;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	UPROPERTY()
	bool bAllowInterruptAfterBlendOut;
};
