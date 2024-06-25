// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ControllerComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "GSInventoryManagerComponent.generated.h"

class UGSInventoryItemDefinition;
class UGSInventoryItemInstance;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FGSInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UGSInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FGSInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGSInventoryEntry()
	{
	}

	FString GetDebugString() const;

private:
	friend FGSInventoryList;
	friend UGSInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UGSInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FGSInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGSInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FGSInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UGSInventoryItemInstance*> GetAllItems() const;

public:

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGSInventoryEntry, FGSInventoryList>(Entries, DeltaParms, *this);
	}

	UGSInventoryItemInstance* AddEntry(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount);
	void AddEntry(UGSInventoryItemInstance* Instance);

	void RemoveEntry(UGSInventoryItemInstance* Instance);

private:

	void BroadcastChangeMessage(FGSInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:

	friend UGSInventoryManagerComponent;

private:

	// Replicated list of items
	UPROPERTY()
	TArray<FGSInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGSInventoryList> : public TStructOpsTypeTraitsBase2<FGSInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};



/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class GASFPS_API UGSInventoryManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UGSInventoryItemInstance* AddItemDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddItemInstance(UGSInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UGSInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
	TArray<UGSInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	UGSInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:

	UPROPERTY(Replicated)
	FGSInventoryList InventoryList;

};