// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "GSPawnBase.generated.h"

class UAbilitySystemComponent;
class UGSHealthSet;

UCLASS()
class GASFPS_API AGSPawnBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGSPawnBase();
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UGSHealthSet* GetHealthSet() const { return HealthSet; }

protected:	


	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UGSHealthSet> HealthSet;
};
