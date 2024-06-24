// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularGameMode.h"

#include "GSGameMode.generated.h"

class UGSPawnData;
/**
 * 
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class GASFPS_API AGSGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
	
public:

	AGSGameMode();

	UFUNCTION(BlueprintCallable, Category = "GS|Pawn")
	const UGSPawnData* GetPawnDataForController(const AController* InController) const;

	//~AGameModeBase interface
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	//~End of AGameModeBase interface


protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSPawnData> DefaultPawnData;
};
