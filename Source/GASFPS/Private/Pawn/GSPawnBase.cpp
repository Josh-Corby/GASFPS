// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/GSPawnBase.h"

#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "AbilitySystemComponent.h"


AGSPawnBase::AGSPawnBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	HealthSet = CreateDefaultSubobject<UGSHealthSet>("HealthSet");
}

void AGSPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
}

void AGSPawnBase::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AGSPawnBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}