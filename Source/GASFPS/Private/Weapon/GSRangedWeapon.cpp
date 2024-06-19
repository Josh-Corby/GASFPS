// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GSRangedWeapon.h"
#include "Net/UnrealNetwork.h"

AGSRangedWeapon::AGSRangedWeapon()
{

}

void AGSRangedWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGSRangedWeapon, StatTags);
}

void AGSRangedWeapon::OnEquipped()
{
	for (const auto KVP : InitialItemStats)
	{
		AddStatTagStack(KVP.Key, KVP.Value);
	}
}

void AGSRangedWeapon::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AGSRangedWeapon::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AGSRangedWeapon::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);

}

bool AGSRangedWeapon::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

int32 AGSRangedWeapon::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
