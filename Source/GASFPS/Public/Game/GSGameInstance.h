// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "GSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASFPS_API UGSGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
public:

	UGSGameInstance(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void Init() override;
};
