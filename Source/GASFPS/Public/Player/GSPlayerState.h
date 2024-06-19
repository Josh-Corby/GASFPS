// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GSPlayerState.generated.h"

class UGSCombatSet;
class UGSHealthSet;

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	AGSPlayerState();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UGSHealthSet* GetHealthSet() const { return HealthSet; }
	UGSCombatSet* GetCombatSet() const { return CombatSet; }

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

private:

	UPROPERTY()
	TObjectPtr<UGSHealthSet> HealthSet;

	UPROPERTY()
	TObjectPtr<UGSCombatSet> CombatSet;
};
