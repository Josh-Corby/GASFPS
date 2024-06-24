// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSPlayerState.h"

#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "AbilitySystem/GSAbilitySet.h"
#include "Character/GSPawnData.h"
#include "Net/UnrealNetwork.h"
#include "Player/GSPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSPlayerState)


AGSPlayerState::AGSPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<UGSHealthSet>("HealthSet");

	NetUpdateFrequency = 100.f;
}

void AGSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}

AGSPlayerController* AGSPlayerState::GetGSPlayerController() const
{
	return Cast<AGSPlayerController>(GetOwner());
}

UAbilitySystemComponent* AGSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGSPlayerState::SetPawnData(const UGSPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UGSAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

void AGSPlayerState::OnRep_PawnData()
{
}
