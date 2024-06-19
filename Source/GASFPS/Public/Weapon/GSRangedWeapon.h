// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/GSWeapon.h"
#include "GameplayTagContainer.h"
#include "System/GameplayTagStack.h"
#include "GSRangedWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API AGSRangedWeapon : public AGSWeapon
{
	GENERATED_BODY()

public:
	AGSRangedWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnEquipped() override;

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, int32> InitialItemStats;

	int32 GetItemStatByTag(FGameplayTag Tag) const;

private:

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
};
