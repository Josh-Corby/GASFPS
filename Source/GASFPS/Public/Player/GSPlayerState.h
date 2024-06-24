// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GSPlayerState.generated.h"

class UGSCombatSet;
class UGSHealthSet;
class UAbilitySystemComponent;
class UGSAbilitySystemComponent;
class UGSPawnData;
class AGSPlayerController;
/**
 * 
 */
UCLASS()
class GASFPS_API AGSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	AGSPlayerState();

	UFUNCTION(BlueprintCallable, Category = "GS|PlayerState")
	AGSPlayerController* GetGSPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "GS|PlayerState")
	UGSAbilitySystemComponent* GetGSAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UGSPawnData* InPawnData);


protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UGSPawnData> PawnData;

private:

	UPROPERTY(VisibleAnywhere, Category = "GS|PlayerState")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UGSHealthSet> HealthSet;
};
