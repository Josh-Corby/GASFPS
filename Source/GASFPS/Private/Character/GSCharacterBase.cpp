// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSCharacterBase.h"

#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Character/GSHealthComponent.h"
#include "Character/GSCharacterMovementComponent.h"
#include "Character/GSPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Game/GSGameMode.h"
#include "GSGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Player/GSPlayerController.h"
#include "Player/GSPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSCharacterBase)


static FName NAME_GSCharacterCollisionProfile_Capsule(TEXT("GSPawnCapsule"));
static FName NAME_GSCharacterCollisionProfile_Mesh(TEXT("GSPawnMesh"));

// Sets default values
AGSCharacterBase::AGSCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_GSCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionProfileName(NAME_GSCharacterCollisionProfile_Mesh);

	UGSCharacterMovementComponent* GSMoveComp = CastChecked<UGSCharacterMovementComponent>(GetCharacterMovement());
	GSMoveComp->GravityScale = 1.0f;
	GSMoveComp->MaxAcceleration = 2400.0f;
	GSMoveComp->BrakingFrictionFactor = 1.0f;
	GSMoveComp->BrakingFriction = 6.0f;
	GSMoveComp->GroundFriction = 8.0f;
	GSMoveComp->BrakingDecelerationWalking = 1400.0f;
	GSMoveComp->bUseControllerDesiredRotation = false;
	GSMoveComp->bOrientRotationToMovement = false;
	GSMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GSMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GSMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	GSMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	GSMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UGSPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));


	HealthComponent = CreateDefaultSubobject<UGSHealthComponent>(TEXT("HealthComponent"));
}

void AGSCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGSCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void AGSCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGSCharacterBase::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AGSCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGSCharacterBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

USkeletalMeshComponent* AGSCharacterBase::GetEquipMesh_Implementation() const
{
	return GetMesh();
}

AGSPlayerController* AGSCharacterBase::GetGSPlayerController() const
{
	return CastChecked<AGSPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AGSPlayerState* AGSCharacterBase::GetGSPlayerState() const
{
	return CastChecked<AGSPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);

}

UGSAbilitySystemComponent* AGSCharacterBase::GetGSAbilitySystemComponent() const
{
	return Cast<UGSAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AGSCharacterBase::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetGSAbilitySystemComponent();
}

void AGSCharacterBase::OnAbilitySystemInitialized()
{
	UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent();
	check(GSASC);

	HealthComponent->InitializeWithAbilitySystem(GSASC);

	InitializeGameplayTags();
}

void AGSCharacterBase::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void AGSCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AGSCharacterBase::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void AGSCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void AGSCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void AGSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AGSCharacterBase::InitializeGameplayTags()
{
}

void AGSCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		GSASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AGSCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		return GSASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AGSCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		return GSASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AGSCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		return GSASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AGSCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UGSCharacterMovementComponent* GSMoveComp = CastChecked<UGSCharacterMovementComponent>(GetCharacterMovement());
	GSMoveComp->StopMovementImmediately();
	GSMoveComp->DisableMovement();
}

void AGSCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		if (GSASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}