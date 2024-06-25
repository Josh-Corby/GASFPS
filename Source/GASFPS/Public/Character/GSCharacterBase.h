// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"

#include "GSCharacterBase.generated.h"

class AActor;
class AController;
class AGSPlayerController;
class AGSPlayerState;
class UAbilitySystemComponent;
class UGSAbilitySystemComponent;
class UGSHealthComponent;
class UGSPawnExtensionComponent;
class UObject;

UCLASS()
class GASFPS_API AGSCharacterBase : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	AGSCharacterBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "GS|Character")
	AGSPlayerController* GetGSPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "GS|Character")
	AGSPlayerState* GetGSPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "GS|Character")
	UGSAbilitySystemComponent* GetGSAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	void DisableMovementAndCollision();
	void UninitAndDestroy();

protected:

	UPROPERTY(VisibleAnywhere, Category = "GS|Character")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "GS|Character")
	TObjectPtr<UGSHealthComponent> HealthComponent;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GS|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGSPawnExtensionComponent> PawnExtComponent;
};
