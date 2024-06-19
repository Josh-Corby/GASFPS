// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSAICharacter.h"

#include "AbilitySystem/Attributes/GSCombatSet.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "AI/GSAIController.h"
#include "GSGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/GSAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"

AGSAICharacter::AGSAICharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	HealthSet = CreateDefaultSubobject<UGSHealthSet>("HealthSet");
	CombatSet = CreateDefaultSubobject<UGSCombatSet>("CombatSet");
}

void AGSAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	GSAIController = Cast<AGSAIController>(NewController);

	GSAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	GSAIController->RunBehaviorTree(BehaviorTree);
}


void AGSAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(FGSGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AGSAICharacter::StunTagChanged);
	}
}

void AGSAICharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bStunned ? 0.f : BaseWalkSpeed;
	if (GSAIController && GSAIController->GetBlackboardComponent())
	{
		GSAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bStunned);
	}
}

void AGSAICharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}