// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GSExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UGSExecCalc_Damage();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	void DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvalParams) const;

};
