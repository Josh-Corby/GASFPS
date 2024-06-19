// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/GSAIPawn.h"

#include "AI/GSAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "AbilitySystemComponent.h"
#include "Perception/AIPerceptionComponent.h"

AGSAIPawn::AGSAIPawn()
{

}

void AGSAIPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AGSAIPawn::PossessedBy(AController* NewController)
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
