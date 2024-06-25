// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GSInventoryItemDefinition.generated.h"

class UGSInventoryItemInstance;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class GASFPS_API UGSInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UGSInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UGSInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class GASFPS_API UGSInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:

	UGSInventoryItemDefinition(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
	TArray<TObjectPtr<UGSInventoryItemFragment>> Fragments;

public:
	const UGSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UGSInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UGSInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
	static const UGSInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UGSInventoryItemDefinition> ItemDef, TSubclassOf<UGSInventoryItemFragment> FragmentClass);
};
