// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GSCharacterBase.generated.h"

class AGSWeapon;
class UAbilitySystemComponent;
class UGSAbilitySet;
class UGSCombatSet;
class UGSHealthSet;

USTRUCT()
struct GASFPS_API FGSCharacterInventory
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<TObjectPtr<AGSWeapon>> Weapons;
};

UCLASS()
class GASFPS_API AGSCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AGSCharacterBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	AGSWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	FName GetWeaponAttachPoint() const { return WeaponAttachPoint; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual USkeletalMeshComponent* GetFirstPersonMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

	UGSHealthSet* GetHealthSet() const { return HealthSet; }
	UGSCombatSet* GetCombatSet() const { return CombatSet; }

	bool IsInFirstPersonPerspective() const { return bIsFirstPersonPerspective; }
	void AddAbilities(AActor* AbilityOwner, const TArray<const UGSAbilitySet*> AbilitiesToGive) const;

	UFUNCTION(BlueprintCallable)
	bool AddWeaponToInventory(AGSWeapon* NewWeapon, bool bEquipWeapon = false);

	UFUNCTION(BlueprintCallable)
	bool RemoveWeaponFromInventory(AGSWeapon* WeaponToRemove);

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AGSWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(AGSWeapon* NewWeapon);
	void ServerEquipWeapon_Implementation(AGSWeapon* NewWeapon);
	bool ServerEquipWeapon_Validate(AGSWeapon* NewWeapon);

protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	FGameplayTag WeaponAbilityTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName WeaponAttachPoint;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFirstPersonPerspective;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AGSWeapon> CurrentWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	FGSCharacterInventory Inventory;

	UPROPERTY()
	TObjectPtr<UGSCombatSet> CombatSet;

	UPROPERTY()
	TObjectPtr<UGSHealthSet> HealthSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseWalkSpeed = 600.f;

protected:
	void SetCurrentWeapon(AGSWeapon* NewWeapon, AGSWeapon* LastWeapon);
	void UnEquipWeapon(AGSWeapon* WeaponToUnEquip);
	void UnEquipCurrentWeapon();
	bool DoesWeaponExistInInventory(AGSWeapon* InWeapon) const;

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

	void Die();
};
