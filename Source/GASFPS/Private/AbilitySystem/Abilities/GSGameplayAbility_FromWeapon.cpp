// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GSGameplayAbility_FromWeapon.h"

#include "Interfaces/GSInventoryInterface.h"
#include "Weapon/GSRangedWeapon.h"

UGSGameplayAbility_FromWeapon::UGSGameplayAbility_FromWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AGSRangedWeapon* UGSGameplayAbility_FromWeapon::GetAssociatedWeapon() const
{
	const AActor* OwningActor = GetAvatarActorFromActorInfo();
	if (OwningActor->Implements<UGSInventoryInterface>())
	{
		AGSWeapon* Weapon = IGSInventoryInterface::Execute_GetCurrentWeapon(OwningActor);

		if (AGSRangedWeapon* RangedWeapon = Cast<AGSRangedWeapon>(Weapon))
		{
			return RangedWeapon;
		}
	}

	return nullptr;
}
