// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GSAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UGSAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
