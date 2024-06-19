// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GSWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Character/GSCharacterBase.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/GSAbilitySet.h"
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

	DOREPLIFETIME_CONDITION(AGSWeapon, OwningCharacter, COND_OwnerOnly);
}

// Called when the game starts or when spawned
void AGSWeapon::BeginPlay()
{
	if (!OwningCharacter && bSpawnWithCollision)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	Super::BeginPlay();
}

void AGSWeapon::PickUpOnTouch(AGSCharacterBase* InCharacter)
{
	if (InCharacter->GetAbilitySystemComponent())
	{
		OnEquipped();
		if (InCharacter->AddWeaponToInventory(this, true) && OwningCharacter->IsInFirstPersonPerspective())
		{
			WeaponMesh3P->CastShadow = false;
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
	}
}

void AGSWeapon::SetOwningCharacter(AGSCharacterBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		SetOwner(OwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (OwningCharacter->GetCurrentWeapon() != this)
		{
			WeaponMesh3P->bCastInsetShadow = false;
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
	}
	else
	{
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AGSWeapon::OnEquipped()
{
}

void AGSWeapon::Equip()
{
	if (!OwningCharacter)
	{
		return;
	}

	const FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();

	if (WeaponMesh1P)
	{
		if (USkeletalMeshComponent* FirstPersonMesh = OwningCharacter->GetFirstPersonMesh())
		{
			WeaponMesh1P->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
			WeaponMesh1P->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
			WeaponMesh1P->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (OwningCharacter->IsInFirstPersonPerspective())
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
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetThirdPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PEquippedRelativeLocation);
		WeaponMesh3P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;

		if (OwningCharacter->IsInFirstPersonPerspective())
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
	if (!OwningCharacter)
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
	AGSCharacterBase* OverlappingCharacter = Cast<AGSCharacterBase>(Other);
	if (GetLocalRole() == ROLE_Authority && OverlappingCharacter != nullptr)
	{
		PickUpOnTouch(OverlappingCharacter);
	}
}