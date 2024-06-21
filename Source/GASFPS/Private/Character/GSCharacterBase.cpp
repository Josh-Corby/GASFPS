// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Attributes/GSHealthSet.h"
#include "Weapon/GSWeapon.h"
#include "GSGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/GSGameMode.h"
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

	GetWorldTimerManager().SetTimerForNextTick(this, &AGSCharacterBase::SpawnDefaultInventory);
}

void AGSCharacterBase::InitAbilityActorInfo()
{
}

bool AGSCharacterBase::AddWeaponToInventory_Implementation(AGSWeapon* NewWeapon, bool bEquipWeapon)
{
	if (DoesWeaponExistInInventory(NewWeapon))
	{
		NewWeapon->Destroy();

		return false;
	}

	Inventory.Weapons.Add(NewWeapon);
	NewWeapon->SetOwningActor(this);

	AddAbilities(NewWeapon, NewWeapon->GetAbilitiesToGrant());
	if (bEquipWeapon)
	{
		EquipWeapon(NewWeapon);
		ClientSyncCurrentWeapon(CurrentWeapon);
	}

	return true;
}

bool AGSCharacterBase::IsAvailableForInteraction_Implementation(AActor* InteractionActor) const
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown))
	{
		return true;
	}

	return IGSInteractableInterface::IsAvailableForInteraction_Implementation(InteractionActor);
}

float AGSCharacterBase::GetInteractionDuration_Implementation(AActor* InteractionActor) const
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown))
	{
		return ReviveDuration;
	}

	return IGSInteractableInterface::GetInteractionDuration_Implementation(InteractionActor);
}

void AGSCharacterBase::PreInteract_Implementation(AActor* InteractingActor, AActor* InteractionActor)
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown) && HasAuthority())
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGSGameplayTags::Get().Abilities_Revive));
	}
}

void AGSCharacterBase::PostInteract_Implementation(AActor* InteractingActor, AActor* InteractionActor)
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown) && HasAuthority())
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(ReviveEffect, 1.f, EffectContext);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void AGSCharacterBase::GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, AActor* InteractionActor) const
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown))
	{
		bShouldSync = true;
		Type = EAbilityTaskNetSyncType::OnlyClientWait;
		return;
	}

	IGSInteractableInterface::GetPreInteractSyncType_Implementation(bShouldSync, Type, InteractionActor);
}

void AGSCharacterBase::CancelInteraction_Implementation(AActor* InteractionActor)
{
	if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(FGSGameplayTags::Get().State_KnockedDown) && HasAuthority())
	{
		FGameplayTagContainer CancelTags(FGSGameplayTags::Get().Abilities_Revive);
		AbilitySystemComponent->CancelAbilities(&CancelTags);
	}
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

	float NumWeapons = Inventory.Weapons.Num();
	if (NumWeapons == 0)
	{
		return;
	}

	UnEquipCurrentWeapon();

	float radius = 50.0f;

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

float AGSCharacterBase::GetHealth() const
{
	return HealthSet->GetHealth();
}

FSimpleMulticastDelegate* AGSCharacterBase::GetTargetCancelInteractionDelegate(AActor* InteractionActor)
{
	return &InteractionCanceledDelegate;
}

void AGSCharacterBase::AddAbilities(AActor* AbilityOwner, const TArray<UGSAbilitySet*>& AbilitiesToGive) const
{
	UGSAbilitySystemComponent* GSASC = CastChecked<UGSAbilitySystemComponent>(GetAbilitySystemComponent());

	if (!HasAuthority())
	{
		return;
	}

	GSASC->AddAbilities(AbilityOwner, AbilitiesToGive);
}

void AGSCharacterBase::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1.f, EffectContext);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
}

void AGSCharacterBase::PlayKnockDownEffects()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
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
		CurrentWeapon->SetOwningActor(this);
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

void AGSCharacterBase::KnockDown()
{
	if (!HasAuthority())
	{
		return;
	}

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(KnockDownEffect, 1.f, ContextHandle);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	HealthSet->FullHeal();
}

void AGSCharacterBase::KnockDownTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (!IsValid(this))
	{
		return;
	}

	if (NewCount > 0)
	{
		PlayKnockDownEffects();
	}
}

void AGSCharacterBase::FinishDying()
{
	if (!HasAuthority())
	{
		return;
	}

	RemoveAllWeaponsFromInventory();

	if (AGSGameMode* GM = Cast<AGSGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->PlayerDied(GetController());
	}

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

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
		if (this->Implements<UGSInventoryInterface>())
		{
			IGSInventoryInterface::Execute_AddWeaponToInventory(this, NewWeapon, bEquipFirstWeapon);
		}
	}
}

void AGSCharacterBase::AddStartupAbilities()
{
	if (StartupAbilities.Num() == 0 || !HasAuthority())
	{
		return;
	}

	AddAbilities(this, StartupAbilities);
}

void AGSCharacterBase::HealthDepleted(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();
	if (IsValid(this) && !IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.State_Dead))
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.State_KnockedDown))
		{
			KnockDown();
		}
		else
		{
			FinishDying();
		}
	}
}