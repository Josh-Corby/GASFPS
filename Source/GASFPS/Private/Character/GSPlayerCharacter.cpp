// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Player/GSPlayerState.h"
#include "Player/GSPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

AGSPlayerCharacter::AGSPlayerCharacter()
	: Super()
{
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(GetRootComponent());
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMesh");
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetVisibility(false, true);
}

void AGSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	SetupStartupPerspective();
}

USkeletalMeshComponent* AGSPlayerCharacter::GetFirstPersonMesh() const
{
	return FirstPersonMesh;
}

void AGSPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AGSPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	SetupStartupPerspective();
}

void AGSPlayerCharacter::InitAbilityActorInfo()
{
	AGSPlayerState* GSPlayerState = GetPlayerState<AGSPlayerState>();
	check(GSPlayerState);
	AbilitySystemComponent = GSPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(GSPlayerState, this);
	HealthSet = GSPlayerState->GetHealthSet();
}

void AGSPlayerCharacter::SetupStartupPerspective()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC && PC->IsLocalController())
		{
			bIsFirstPersonPerspective = true;
			SetPerspective(bIsFirstPersonPerspective);
		}
	}
}

void AGSPlayerCharacter::SetPerspective(bool bInIsFirstPersonPerspective)
{
	AGSPlayerController* PC = GetController<AGSPlayerController>();
	if (PC && PC->IsLocalPlayerController())
	{
		if (bInIsFirstPersonPerspective)
		{
			GetMesh()->SetVisibility(false, true);
			FirstPersonMesh->SetVisibility(true, true);
			GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() + FVector(-120.0f, 0.0f, 0.0f));
		}
	}
}
