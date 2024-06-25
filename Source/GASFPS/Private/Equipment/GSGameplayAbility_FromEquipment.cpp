// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/GSGameplayAbility_FromEquipment.h"

#include "Equipment/GSEquipmentInstance.h"
#include "Inventory/GSInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSGameplayAbility_FromEquipment)

UGSGameplayAbility_FromEquipment::UGSGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGSEquipmentInstance* UGSGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UGSEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UGSInventoryItemInstance* UGSGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UGSEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UGSInventoryItemInstance>(Equipment->GetInstigator());
	}

	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UGSGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("GS", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif