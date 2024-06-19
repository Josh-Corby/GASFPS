// Fill out your copyright notice in the Description page of Project Settings.


#include "GSAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "GSGameplayTags.h"

UGSAssetManager& UGSAssetManager::Get()
{
	check(GEngine);

	UGSAssetManager* GSAssetManager = Cast<UGSAssetManager>(GEngine->AssetManager);
	return *GSAssetManager;
}

void UGSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FGSGameplayTags::InitializeNativeGameplayTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
