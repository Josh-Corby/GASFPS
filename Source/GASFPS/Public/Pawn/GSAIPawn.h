// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/GSPawnBase.h"
#include "GSAIPawn.generated.h"

class UBehaviorTree;
class AGSAIController;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSAIPawn : public AGSPawnBase
{
	GENERATED_BODY()
	
public:

	AGSAIPawn();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AGSAIController> GSAIController;
};
