// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GSGameplayAbility_FromWeapon.h"

#include "Character/GSCharacterBase.h"
#include "Weapon/GSRangedWeapon.h"

UGSGameplayAbility_FromWeapon::UGSGameplayAbility_FromWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AGSRangedWeapon* UGSGameplayAbility_FromWeapon::GetAssociatedWeapon() const
{
	AGSWeapon* Weapon = Cast<AGSCharacterBase>(GetAvatarActorFromActorInfo())->GetCurrentWeapon();
	if (AGSRangedWeapon* RangedWeapon = Cast<AGSRangedWeapon>(Weapon))
	{
		return RangedWeapon;
	}

	return nullptr;
}
