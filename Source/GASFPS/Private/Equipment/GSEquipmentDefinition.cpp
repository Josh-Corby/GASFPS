// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/GSEquipmentDefinition.h"

#include "Equipment/GSEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSEquipmentDefinition)

UGSEquipmentDefinition::UGSEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UGSEquipmentInstance::StaticClass();
}

