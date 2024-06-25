// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/GSCharacterBase.h"
#include "GSPlayerCharacter.generated.h"

class UGSHeroComponent;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSPlayerCharacter : public AGSCharacterBase
{
	GENERATED_BODY()

public:

	AGSPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

protected:

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

	void SetPerspective(bool InIsFirstPersonPerspective);
	void SetupStartupPerspective();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

private:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSHeroComponent> HeroComponent;
};
