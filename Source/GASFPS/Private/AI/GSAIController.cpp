// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/GSAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AGSAIController::AGSAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackBoard");
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviourTree");
	check(BehaviorTreeComponent);
}
