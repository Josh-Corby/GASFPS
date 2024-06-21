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

	void SetOwningActor(AActor* InOwningActor);
	virtual void OnEquipped();

	virtual void Equip();
	virtual void UnEquip();

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	TArray<UGSAbilitySet*> GetAbilitiesToGrant() const { return AbilitySetsToGrant; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual USkeletalMeshComponent* GetWeaponMesh1P() const { return WeaponMesh1P; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual USkeletalMeshComponent* GetWeaponMesh3P() const { return WeaponMesh3P; }

	UFUNCTION(NetMulticast, Reliable)
	void OnDropped(FVector NewLocation);
	virtual void OnDropped_Implementation(FVector NewLocation);
	virtual bool OnDropped_Validate(FVector NewLocation);

	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision = true;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh1P;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh3P;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<AActor> OwningActor;
	
	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh1PEquippedRelativeLocation;

	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh3PEquippedRelativeLocation;

	UPROPERTY(EditDefaultsOnly);
	FVector WeaponMesh3PPickupRelativeLocation;

	virtual void PickUpOnTouch(AActor* InActor);

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TObjectPtr<UGSAbilitySet>> AbilitySetsToGrant;
};
