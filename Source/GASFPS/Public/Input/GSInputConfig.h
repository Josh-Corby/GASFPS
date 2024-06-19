// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GSInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FGSInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};


/**
 * 
 */
UCLASS()
class GASFPS_API UGSInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UGSInputConfig(const FObjectInitializer& ObjectInitializer);

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGSInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGSInputAction> AbilityInputActions;
};
