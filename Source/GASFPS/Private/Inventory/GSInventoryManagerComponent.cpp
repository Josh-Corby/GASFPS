// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GSInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/GSInventoryItemDefinition.h"
#include "Inventory/GSInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSInventoryManagerComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GS_Inventory_Message_StackChanged, "GS.Inventory.Message.StackChanged");


FString FGSInventoryEntry::GetDebugString() const
{
	TSubclassOf<UGSInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}


//////////////////////////////////////////////////////////////////////
// FGSInventoryList

void FGSInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FGSInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FGSInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FGSInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FGSInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FGSInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FGSInventoryList::BroadcastChangeMessage(FGSInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FGSInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_GS_Inventory_Message_StackChanged, Message);
}

UGSInventoryItemInstance* FGSInventoryList::AddEntry(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UGSInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FGSInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UGSInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UGSInventoryItemFragment* Fragment : GetDefault<UGSInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	MarkItemDirty(NewEntry);

	return Result;
}

void FGSInventoryList::AddEntry(UGSInventoryItemInstance* Instance)
{
	unimplemented();
}

void FGSInventoryList::RemoveEntry(UGSInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGSInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UGSInventoryItemInstance*> FGSInventoryList::GetAllItems() const
{
	TArray<UGSInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FGSInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr)
		{
			Results.Add(Entry.Instance);
		}
	}

	return Results;
}

//////////////////////////////////////////////////////////////////////
// UGSInventoryManagerComponent

UGSInventoryManagerComponent::UGSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UGSInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UGSInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount)
{
	return true;
}

UGSInventoryItemInstance* UGSInventoryManagerComponent::AddItemDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UGSInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UGSInventoryManagerComponent::AddItemInstance(UGSInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UGSInventoryManagerComponent::RemoveItemInstance(UGSInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UGSInventoryItemInstance*> UGSInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UGSInventoryItemInstance* UGSInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef) const
{
	for (const FGSInventoryEntry& Entry : InventoryList.Entries)
	{
		UGSInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UGSInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;

	for (const FGSInventoryEntry& Entry : InventoryList.Entries)
	{
		UGSInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UGSInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UGSInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();

	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	int32 TotalConsumed = 0;

	while (TotalConsumed < NumToConsume)
	{
		if (UGSInventoryItemInstance* Instance = UGSInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UGSInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FGSInventoryEntry& Entry : InventoryList.Entries)
		{
			UGSInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UGSInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FGSInventoryEntry& Entry : InventoryList.Entries)
	{
		UGSInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}
