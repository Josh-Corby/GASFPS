// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "Interfaces/GSInventoryInterface.h"
#include "Interfaces/GSInteractableInterface.h"
#include "Interfaces/GSCombatInterface.h"
#include "GSCharacterBase.generated.h"

class AGSWeapon;
class UAbilitySystemComponent;
class UGSAbilitySet;
class UGSCombatSet;
class UGSHealthSet;
struct FOnAttributeChangeData;

USTRUCT()
struct GASFPS_API FGSCharacterInventory
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<TObjectPtr<AGSWeapon>> Weapons;
};

UCLASS()
class GASFPS_API AGSCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGSInventoryInterface, public IGSInteractableInterface, public IGSCombatInterface
{
	GENERATED_BODY()

public:

	AGSCharacterBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Inventory Interface
	virtual USkeletalMeshComponent* GetFirstPersonMesh_Implementation() const override { return nullptr; }
	virtual USkeletalMeshComponent* GetThirdPersonMesh_Implementation() const override { return GetMesh(); }
	virtual bool AddWeaponToInventory_Implementation(AGSWeapon* NewWeapon, bool bEquipWeapon = false) override;
	virtual FName GetWeaponAttachPoint_Implementation() const override { return WeaponAttachPoint; }
	virtual AGSWeapon* GetCurrentWeapon_Implementation() const override { return CurrentWeapon; }
	virtual bool IsInFirstPersonPerspective_Implementation() const override { return bIsFirstPersonPerspective; }
	// End Inventory Interface

	// Interactable Interface
	/**
	* We can Interact with other heroes when:
	* Knocked Down - to revive them
	*/
	virtual bool IsAvailableForInteraction_Implementation(AActor* InteractionActor) const override;

	/**
	* How long to interact with this player:
	* Knocked Down - to revive them
	*/
	virtual float GetInteractionDuration_Implementation(AActor* InteractionActor) const override;

	/**
	* Interaction:
	* Knocked Down - activate revive GA (plays animation)
	*/
	virtual void PreInteract_Implementation(AActor* InteractingActor, AActor* InteractionActor) override;

	/**
	* Interaction:
	* Knocked Down - apply revive GE
	*/
	virtual void PostInteract_Implementation(AActor* InteractingActor, AActor* InteractionActor) override;

	/**
	* Should we wait and who should wait to sync before calling PreInteract():
	* Knocked Down - Yes, client. This will sync the local player's Interact Duration Timer with the knocked down player's
	* revive animation. If we had a picking a player up animation, we could play it on the local player in PreInteract().
	*/
	virtual void GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, AActor* InteractionActor) const override;

	/**
	* Cancel interaction:
	* Knocked Down - cancel revive ability
	*/
	virtual void CancelInteraction_Implementation(AActor* InteractionActor) override;

	/**
	* Get the delegate for this Actor canceling interaction:
	* Knocked Down - cancel being revived if killed
	*/
	FSimpleMulticastDelegate* GetTargetCancelInteractionDelegate(AActor* InteractionActor) override;
	// End Interactable Interface


	UGSHealthSet* GetHealthSet() const { return HealthSet; }
	void AddAbilities(AActor* AbilityOwner, const TArray<UGSAbilitySet*>& AbilitiesToGive) const;
	void InitializeAttributes();

	virtual void PlayKnockDownEffects();

	UFUNCTION(BlueprintCallable)
	bool RemoveWeaponFromInventory(AGSWeapon* WeaponToRemove);

	UFUNCTION(BlueprintCallable)
	void RemoveAllWeaponsFromInventory();

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AGSWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(AGSWeapon* NewWeapon);
	void ServerEquipWeapon_Implementation(AGSWeapon* NewWeapon);
	bool ServerEquipWeapon_Validate(AGSWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float ReviveDuration;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	FGameplayTag WeaponAbilityTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName WeaponAttachPoint;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFirstPersonPerspective = false;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AGSWeapon> CurrentWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	FGSCharacterInventory Inventory;

	UPROPERTY()
	TObjectPtr<UGSHealthSet> HealthSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseWalkSpeed = 600.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<AGSWeapon>> DefaultInventoryWeaponClasses;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> KnockDownEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly)
	TArray<UGSAbilitySet*> StartupAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReviveEffect;

	FSimpleMulticastDelegate InteractionCanceledDelegate;


protected:
	void SetCurrentWeapon(AGSWeapon* NewWeapon, AGSWeapon* LastWeapon);
	void UnEquipWeapon(AGSWeapon* WeaponToUnEquip);
	void UnEquipCurrentWeapon();
	bool DoesWeaponExistInInventory(AGSWeapon* InWeapon) const;
	void SpawnDefaultInventory();
	void AddStartupAbilities();

	UFUNCTION()
	void OnRep_CurrentWeapon(AGSWeapon* LastWeapon);

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION(Server, Reliable)
	void ServerSyncCurrentWeapon();
	void ServerSyncCurrentWeapon_Implementation();
	bool ServerSyncCurrentWeapon_Validate();

	UFUNCTION(Client, Reliable)
	void ClientSyncCurrentWeapon(AGSWeapon* InWeapon);
	void ClientSyncCurrentWeapon_Implementation(AGSWeapon* InWeapon);
	bool ClientSyncCurrentWeapon_Validate(AGSWeapon* InWeapon);

	UFUNCTION(BlueprintCallable)
	virtual void FinishDying();

	bool IsAlive() const { return GetHealth() > 0.f; }
	virtual void HealthDepleted(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue);
	virtual void KnockDown();

	virtual void KnockDownTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
