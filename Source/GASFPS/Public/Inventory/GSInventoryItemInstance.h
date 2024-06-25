// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"
#include "GSInventoryItemInstance.generated.h"

class UGSInventoryItemDefinition;
class UGSInventoryItemFragment;
/**
 * 
 */
UCLASS()
class GASFPS_API UGSInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:

	UGSInventoryItemInstance(const FObjectInitializer& ObjectInitializer);

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

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

	TSubclassOf<UGSInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UGSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UGSInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	void SetItemDef(TSubclassOf<UGSInventoryItemDefinition> InDef);

	friend struct FGSInventoryList;

private:

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<UGSInventoryItemDefinition> ItemDef;
};
