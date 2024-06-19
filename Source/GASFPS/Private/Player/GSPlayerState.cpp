// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSPlayerState.h"

#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"

AGSPlayerState::AGSPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthSet = CreateDefaultSubobject<UGSHealthSet>("HealthSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AGSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
