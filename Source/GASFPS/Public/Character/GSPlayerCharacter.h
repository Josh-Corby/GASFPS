// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GSCharacterBase.h"
#include "GSPlayerCharacter.generated.h"

class UCameraComponent;
class AGSWeapon;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSPlayerCharacter : public AGSCharacterBase
{
	GENERATED_BODY()
	
public:

	AGSPlayerCharacter();

	// Only called on server.
	virtual void PossessedBy(AController* NewController) override;
	virtual USkeletalMeshComponent* GetFirstPersonMesh() const override;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USkeletalMeshComponent* FirstPersonMesh;

	virtual void InitAbilityActorInfo() override;

	void SetupStartupPerspective();

	void SetPerspective(bool bInIsFirstPersonPerspective);

	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
};
