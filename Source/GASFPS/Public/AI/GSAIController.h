// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GSAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class GASFPS_API AGSAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AGSAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
