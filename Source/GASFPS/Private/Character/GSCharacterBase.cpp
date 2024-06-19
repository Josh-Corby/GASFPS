// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "Weapon/GSWeapon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/GSRangedWeapon.h"

// Sets default values
AGSCharacterBase::AGSCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->bCastHiddenShadow = true;
	GetMesh()->bReceivesDecals = false;

	bAlwaysRelevant = true;
}

void AGSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGSCharacterBase, Inventory);

	DOREPLIFETIME_CONDITION(AGSCharacterBase, CurrentWeapon, COND_SimulatedOnly);
}

void AGSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.AddLambda([this](AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/)
		{
			Die();
		});
	}

	GetWorldTimerManager().SetTimerForNextTick(this, &AGSCharacterBase::SpawnDefaultInventory);
}

void AGSCharacterBase::InitAbilityActorInfo()
{
}

USkeletalMeshComponent* AGSCharacterBase::GetFirstPersonMesh() const
{
	return nullptr;
}

bool AGSCharacterBase::AddWeaponToInventory(AGSWeapon* NewWeapon, bool bEquipWeapon)
{
	if (DoesWeaponExistInInventory(NewWeapon))
	{
		NewWeapon->Destroy();

		return false;
	}

	Inventory.Weapons.Add(NewWeapon);
	NewWeapon->SetOwningCharacter(this);

	AddAbilities(NewWeapon, NewWeapon->GetAbilitiesToGrant());
	if (bEquipWeapon)
	{
		EquipWeapon(NewWeapon);
		ClientSyncCurrentWeapon(CurrentWeapon);
	}

	return true;
}

bool AGSCharacterBase::RemoveWeaponFromInventory(AGSWeapon* WeaponToRemove)
{
	if (!DoesWeaponExistInInventory(WeaponToRemove))
	{
		return false;
	}

	if (WeaponToRemove == CurrentWeapon)
	{
		UnEquipCurrentWeapon();
		return true;
	}

	return false;
}

void AGSCharacterBase::RemoveAllWeaponsFromInventory()
{
	if (!HasAuthority())
	{
		return;
	}

	UnEquipCurrentWeapon();


	float radius = 50.0f;
	float NumWeapons = Inventory.Weapons.Num();

	if (NumWeapons > 1)
	{
		for (int32 i = NumWeapons - 1; i >= 0; i--)
		{
			AGSWeapon* Weapon = Inventory.Weapons[i];
			RemoveWeaponFromInventory(Weapon);

			// Set the weapon up as a pickup

			float OffsetX = radius * FMath::Cos((i / NumWeapons) * 2.0f * PI);
			float OffsetY = radius * FMath::Sin((i / NumWeapons) * 2.0f * PI);
			Weapon->OnDropped(GetActorLocation() + FVector(OffsetX, OffsetY, 0.0f));
		}
	}
	else
	{
		AGSWeapon* Weapon = Inventory.Weapons[0];
		RemoveWeaponFromInventory(Weapon);
		Weapon->OnDropped(GetActorLocation());
	}
}

void AGSCharacterBase::EquipWeapon(AGSWeapon* NewWeapon)
{
	if (!HasAuthority() && IsLocallyControlled())
	{
		ServerEquipWeapon(NewWeapon);
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
	}
	else if(HasAuthority())
	{
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
	}
}

void AGSCharacterBase::ServerEquipWeapon_Implementation(AGSWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool AGSCharacterBase::ServerEquipWeapon_Validate(AGSWeapon* NewWeapon)
{
	return true;
}

void AGSCharacterBase::AddAbilities(AActor* AbilityOwner, const TArray<const UGSAbilitySet*> AbilitiesToGive) const
{
	UGSAbilitySystemComponent* GSASC = CastChecked<UGSAbilitySystemComponent>(GetAbilitySystemComponent());

	if (!HasAuthority())
	{
		return;
	}

	GSASC->AddAbilities(AbilityOwner, AbilitiesToGive);
}

UAbilitySystemComponent* AGSCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGSCharacterBase::OnRep_CurrentWeapon(AGSWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AGSCharacterBase::SetCurrentWeapon(AGSWeapon* NewWeapon, AGSWeapon* LastWeapon)
{
	if (NewWeapon == LastWeapon)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AbilitiesToCancel = FGameplayTagContainer(WeaponAbilityTag);
		AbilitySystemComponent->CancelAbilities(&AbilitiesToCancel);
	}

	UnEquipWeapon(LastWeapon);

	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->Equip();
	}
	else
	{
		UnEquipCurrentWeapon();
	}
}

void AGSCharacterBase::UnEquipWeapon(AGSWeapon* WeaponToUnEquip)
{
	if (!WeaponToUnEquip)
	{
		return;
	}

	WeaponToUnEquip->UnEquip();
}

void AGSCharacterBase::UnEquipCurrentWeapon()
{
	UnEquipWeapon(CurrentWeapon);
	CurrentWeapon = nullptr;
}

bool AGSCharacterBase::DoesWeaponExistInInventory(AGSWeapon* InWeapon) const
{
	for (AGSWeapon* Weapon : Inventory.Weapons)
	{
		if (Weapon && InWeapon && Weapon->GetClass() == InWeapon->GetClass())
		{
			return true;
		}
	}

	return false;
}

void AGSCharacterBase::OnRep_Inventory()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && Inventory.Weapons.Num() > 0 && !CurrentWeapon)
	{
		ServerSyncCurrentWeapon();
	}
}

void AGSCharacterBase::ServerSyncCurrentWeapon_Implementation()
{
	ClientSyncCurrentWeapon(CurrentWeapon);
}

bool AGSCharacterBase::ServerSyncCurrentWeapon_Validate()
{
	return true;
}

void AGSCharacterBase::ClientSyncCurrentWeapon_Implementation(AGSWeapon* InWeapon)
{
	AGSWeapon* LastWeapon = CurrentWeapon;
	CurrentWeapon = InWeapon;
	OnRep_CurrentWeapon(LastWeapon);
}

bool AGSCharacterBase::ClientSyncCurrentWeapon_Validate(AGSWeapon* InWeapon)
{
	return true;
}

void AGSCharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	FinishDying();
}

void AGSCharacterBase::FinishDying()
{
	if (!HasAuthority())
	{
		return;
	}

	RemoveAllWeaponsFromInventory();

	Destroy();
}

void AGSCharacterBase::SpawnDefaultInventory()
{
	if (!HasAuthority())
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (!DefaultInventoryWeaponClasses[i])
		{
			// An empty item was added to the Array in blueprint
			continue;
		}

		AGSWeapon* NewWeapon = GetWorld()->SpawnActorDeferred<AGSWeapon>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			NewWeapon->bSpawnWithCollision = false;
			NewWeapon->FinishSpawning(FTransform::Identity);

			bool bEquipFirstWeapon = i == 0;
			AddWeaponToInventory(NewWeapon, bEquipFirstWeapon);
	}
}
