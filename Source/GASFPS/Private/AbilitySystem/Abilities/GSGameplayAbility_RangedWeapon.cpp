// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GSGameplayAbility_RangedWeapon.h"

#include "AbilitySystemComponent.h"

UGSGameplayAbility_RangedWeapon::UGSGameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGSGameplayAbility_RangedWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bResult)
	{
		// perform weapon check here
	}

	return bResult;
}

void UGSGameplayAbility_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	OnTargetDataReadyCallbackDelegateHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

	// can update weapon firing time here
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGSGameplayAbility_RangedWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
		}

		UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
		check(ASC);

		// when the abilty ends, consume target data and remove delegate
		ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UGSGameplayAbility_RangedWeapon::StartRangedWeaponTargetting()
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	AController* Controller = GetControllerFromActorInfo();
	check(Controller);
	FScopedPredictionWindow ScopedPrediction(ASC, CurrentActivationInfo.GetActivationPredictionKey());

	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(/*out*/ FoundHits);

	// fill out target data from hit results
	FGameplayAbilityTargetDataHandle TargetData;
	
	if (FoundHits.Num() > 0)
	{
		for (const FHitResult& FoundHit : FoundHits)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;

			TargetData.Add(NewTargetData);
		}
	}

	// process target data immediately
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UGSGameplayAbility_RangedWeapon::WeaponTrace(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits) const
{
	TArray<FHitResult> AllImpacts;
	FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, InputData.EndAim, AllImpacts);

	const AActor* HitActor = Impact.GetActor();

	if (HitActor)
	{
		//DrawDebugPoint(GetWorld(), Impact.ImpactPoint, 20.f, FColor::White, false, 10.f);

		if (AllImpacts.Num() > 0)
		{
			OutHits.Append(AllImpacts);
		}
	}

	if (OutHits.Num() == 0)
	{
		if (!Impact.bBlockingHit)
		{
			Impact.Location = InputData.EndAim;
			Impact.ImpactPoint = InputData.EndAim;
		}

		OutHits.Add(Impact);
	}
}

FHitResult UGSGameplayAbility_RangedWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, OUT TArray<FHitResult>& OutHits) const
{
	TArray<FHitResult> HitResults;

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);

	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		for (FHitResult& HitResult : HitResults)
		{
			auto Pred = [&HitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == HitResult.HitObjectHandle;
				};

			if (!OutHits.ContainsByPredicate(Pred))
			{
				OutHits.Add(HitResult);
			}
		}

		Hit = OutHits.Last();
	}

	return Hit;
}

void UGSGameplayAbility_RangedWeapon::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits)
{
	const APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (AvatarPawn && AvatarPawn->IsLocallyControlled())
	{
		FRangedWeaponFiringInput InputData;
		const FVector AvatarLocation = AvatarPawn->GetActorLocation();
		FVector TraceStart(AvatarLocation.X, AvatarLocation.Y, AvatarLocation.Z);

		FRotator ViewRot = TraceStart.Rotation();
		FVector ViewStart = TraceStart;

		if (APlayerController* PC = Cast<APlayerController>(GetAvatarActorFromActorInfo()->GetInstigatorController()))
		{		
			PC->GetPlayerViewPoint(ViewStart, ViewRot);
		}
		else
		{
			ViewRot = AvatarPawn->GetActorForwardVector().Rotation();
		}

		const FVector ViewDir = ViewRot.Vector();
		const float AimDistance = 2000.f;
		FVector ViewEnd = ViewStart + (ViewDir * AimDistance);

		InputData.StartTrace = ViewStart;
		InputData.EndAim = ViewEnd;

		DrawDebugLine(GetWorld(), ViewStart, ViewEnd, FColor::Yellow, false, 10.f, 0, 10.f);

		WeaponTrace(InputData, OutHits);
	}
}

void UGSGameplayAbility_RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow ScopedPrediction(ASC);

		// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, ASC->ScopedPredictionKey);
		}

		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			OnRangedWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			K2_EndAbility();
		}

		ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	}
}