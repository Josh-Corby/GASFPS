// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GSCharacterBase.h"
#include "GSAICharacter.generated.h"

class UBehaviorTree;
class AGSAIController;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSAICharacter : public AGSCharacterBase
{
	GENERATED_BODY()
	
public:

	AGSAICharacter();
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void PossessedBy(AController* NewController) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AGSAIController> GSAIController;

	UFUNCTION(BlueprintCallable)
	void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	bool bStunned = false;
};
