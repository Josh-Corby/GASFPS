// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GSWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Character/GSCharacterBase.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/GSAbilitySet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/GSInventoryInterface.h"
#include "GASFPS/GASFPS.h"

// Sets default values
AGSWeapon::AGSWeapon()
{
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>("CollisionComponent");
	CollisionComponent->InitCapsuleSize(40.f, 50.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComponent;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->SetupAttachment(CollisionComponent);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	WeaponMesh3PPickupRelativeLocation = FVector(0.f, -25.f, 0.f);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetupAttachment(CollisionComponent);
	WeaponMesh3P->SetRelativeLocation(WeaponMesh3PPickupRelativeLocation);
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
}

void AGSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGSWeapon, OwningActor, COND_OwnerOnly);
}

void AGSWeapon::OnDropped_Implementation(FVector NewLocation)
{
	SetOwningActor(nullptr);

	SetActorLocation(NewLocation);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(CollisionComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh1P->SetVisibility(false, true);
	}

	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(CollisionComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PPickupRelativeLocation);
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->SetVisibility(true, true);
	}
}

bool AGSWeapon::OnDropped_Validate(FVector NewLocation)
{
	return true;
}

void AGSWeapon::BeginPlay()
{
	if (!OwningActor && bSpawnWithCollision)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	Super::BeginPlay();
}

void AGSWeapon::PickUpOnTouch(AActor* InActor)
{
	if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor))
	{
		OnEquipped();
		if (InActor->Implements<UGSInventoryInterface>())
		{
			if (IGSInventoryInterface::Execute_AddWeaponToInventory(InActor, this, true) /*&& OwningActor->IsInFirstPersonPerspective()*/)
			{
				WeaponMesh3P->CastShadow = false;
				WeaponMesh3P->SetVisibility(true, true);
				WeaponMesh3P->SetVisibility(false, true);
			}
		}
	}
}

void AGSWeapon::SetOwningActor(AActor* InOwningActor)
{
	OwningActor = InOwningActor;
	if (!OwningActor || !OwningActor->Implements<UGSInventoryInterface>())
	{
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		return;
	}

	SetOwner(OwningActor);
	AttachToComponent(OwningActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IGSInventoryInterface::Execute_GetCurrentWeapon(OwningActor) != this)
	{
		WeaponMesh3P->bCastInsetShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}
}

void AGSWeapon::OnEquipped()
{
}

void AGSWeapon::Equip()
{
	if (!OwningActor || !OwningActor->Implements<UGSInventoryInterface>())
	{
		return;
	}

	const FName AttachPoint = IGSInventoryInterface::Execute_GetWeaponAttachPoint(OwningActor);

	if (WeaponMesh1P)
	{
		if (USkeletalMeshComponent* FirstPersonMesh = IGSInventoryInterface::Execute_GetFirstPersonMesh(OwningActor))
		{
			WeaponMesh1P->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
			WeaponMesh1P->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
			WeaponMesh1P->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (IGSInventoryInterface::Execute_IsInFirstPersonPerspective(OwningActor))
			{
				WeaponMesh1P->SetVisibility(true, true);
			}
			else
			{
				WeaponMesh1P->SetVisibility(false, true);
			}
		}
	}

	if (WeaponMesh3P)
	{
		if (USkeletalMeshComponent* ThirdPersonMesh = IGSInventoryInterface::Execute_GetThirdPersonMesh(OwningActor))
		{
			WeaponMesh3P->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
			WeaponMesh3P->SetRelativeLocation(WeaponMesh3PEquippedRelativeLocation);
			WeaponMesh3P->SetRelativeRotation(FRotator(0, 0, -90.0f));
			WeaponMesh3P->CastShadow = true;
			WeaponMesh3P->bCastHiddenShadow = true;
		}	

		if (IGSInventoryInterface::Execute_IsInFirstPersonPerspective(OwningActor))
		{
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
		}
	}
}

void AGSWeapon::UnEquip()
{
	if (!OwningActor)
	{
		return;
	}

	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);

	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
	WeaponMesh3P->SetVisibility(true, true); // Without this, the unequipped weapon's 3p shadow hangs around
	WeaponMesh3P->SetVisibility(false, true);
}

void AGSWeapon::NotifyActorBeginOverlap(AActor* Other)
{
	if (GetLocalRole() == ROLE_Authority && Other->Implements<UGSInventoryInterface>())
	{
		PickUpOnTouch(Other);
	}
}