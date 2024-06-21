// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/GSAsyncTask_TagAddedRemoved.h"

#include "AbilitySystemComponent.h"

UGSAsyncTask_TagAddedRemoved* UGSAsyncTask_TagAddedRemoved::ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InTags)
{
	UGSAsyncTask_TagAddedRemoved* ListenForGameplayTagAddedRemoved = NewObject<UGSAsyncTask_TagAddedRemoved>();
	ListenForGameplayTagAddedRemoved->ASC = InAbilitySystemComponent;
	ListenForGameplayTagAddedRemoved->Tags = InTags;

	if (!IsValid(InAbilitySystemComponent) || InTags.Num() < 1)
	{
		ListenForGameplayTagAddedRemoved->EndTask();
		return nullptr;
	}

	TArray<FGameplayTag> TagArray;
	InTags.GetGameplayTagArray(TagArray);

	for (const FGameplayTag& Tag : TagArray)
	{
		InAbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForGameplayTagAddedRemoved, &UGSAsyncTask_TagAddedRemoved::TagChanged);
	}

	return ListenForGameplayTagAddedRemoved;
}

void UGSAsyncTask_TagAddedRemoved::EndTask()
{
	if (IsValid(ASC))
	{
		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		for (FGameplayTag Tag : TagArray)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UGSAsyncTask_TagAddedRemoved::TagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnTagAdded.Broadcast(Tag);
	}
	else
	{
		OnTagRemoved.Broadcast(Tag);
	}
}
