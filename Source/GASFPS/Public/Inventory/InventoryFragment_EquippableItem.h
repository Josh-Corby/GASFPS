
#pragma once

#include "GSInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UGSEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UGSInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=GS)
	TSubclassOf<UGSEquipmentDefinition> EquipmentDefinition;
};
