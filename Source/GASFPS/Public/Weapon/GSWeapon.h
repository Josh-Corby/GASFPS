// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSWeapon.generated.h"

class AGSCharacterBase;
class UCapsuleComponent;
class UGSAbilitySet;

UCLASS(Blueprintable, BlueprintType)
class GASFPS_API AGSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AGSWeapon();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOwningCharacter(AGSCharacterBase* InOwningCharacter);
	virtual void OnEquipped();

	virtual void Equip();
	virtual void UnEquip();

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	const TArray<const UGSAbilitySet*> GetAbilitiesToGrant() const { return AbilitySetsToGrant; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual USkeletalMeshComponent* GetWeaponMesh1P() const { return WeaponMesh1P; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual USkeletalMeshComponent* GetWeaponMesh3P() const { return WeaponMesh3P; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh1P;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh3P;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<AGSCharacterBase> OwningCharacter;
	
	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh1PEquippedRelativeLocation;

	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh3PEquippedRelativeLocation;

	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh3PPickupRelativeLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision = true;

	virtual void PickUpOnTouch(AGSCharacterBase* InCharacter);

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TObjectPtr<const UGSAbilitySet>> AbilitySetsToGrant;
};
