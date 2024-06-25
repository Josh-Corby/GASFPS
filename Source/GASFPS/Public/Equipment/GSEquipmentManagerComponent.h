// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/GSAbilitySet.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Components/ActorComponent.h"

#include "GSEquipmentManagerComponent.generated.h"

class UGSEquipmentDefinition;
class UGSEquipmentInstance;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FGSAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGSAppliedEquipmentEntry()
	{
	}

	FString GetDebugString() const;

private:
	friend FGSEquipmentList;
	friend UGSEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UGSEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UGSEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FGSAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FGSEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGSEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FGSEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGSAppliedEquipmentEntry, FGSEquipmentList>(Entries, DeltaParms, *this);
	}

	UGSEquipmentInstance* AddEntry(TSubclassOf<UGSEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UGSEquipmentInstance* Instance);

private:
	UGSAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UGSEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FGSAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGSEquipmentList> : public TStructOpsTypeTraitsBase2<FGSEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class UGSEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UGSEquipmentInstance* EquipItem(TSubclassOf<UGSEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UGSEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGSEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UGSEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UGSEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UGSEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FGSEquipmentList EquipmentList;
};
