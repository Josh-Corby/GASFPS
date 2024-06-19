// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GSGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GSAbilitySet.generated.h"

class UGSAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FGSAbiltySet_GameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGSGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

UCLASS()
class GASFPS_API UGSAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UGSAbilitySet(const FObjectInitializer& ObjectInitializer);

	void GiveToAbilitySystem(UAbilitySystemComponent* ASC, UObject* SourceObject = nullptr) const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TArray<FGSAbiltySet_GameplayAbility> GrantedAbilities;
};
