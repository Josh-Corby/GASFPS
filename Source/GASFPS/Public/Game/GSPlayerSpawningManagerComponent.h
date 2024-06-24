// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GSPlayerSpawningManagerComponent.generated.h"

class AGSPlayerStart;
/**
 * 
 */
UCLASS()
class GASFPS_API UGSPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:

	UGSPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	virtual void InitializeComponent() override;
	/** ~UActorComponent */

private:

	void HandleOnActorSpawned(AActor* SpawnedActor);

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AGSPlayerStart>> CachedPlayerStarts;
};
