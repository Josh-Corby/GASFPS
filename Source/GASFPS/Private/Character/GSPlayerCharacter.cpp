// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSPlayerCharacter.h"

#include "Character/GSHeroComponent.h"

AGSPlayerCharacter::AGSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeroComponent = CreateDefaultSubobject<UGSHeroComponent>("HeroComponent");

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstPersonMesh");
	FirstPersonMesh->SetupAttachment(GetRootComponent());
	FirstPersonMesh->SetVisibility(false, true);
}

void AGSPlayerCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AGSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void AGSPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGSPlayerCharacter::Reset()
{
	Super::Reset();
}

void AGSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGSPlayerCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void AGSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupStartupPerspective();
}

void AGSPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	SetupStartupPerspective();
}

void AGSPlayerCharacter::SetupStartupPerspective()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && PC->IsLocalController())
	{
		SetPerspective(true);
	}
}

void AGSPlayerCharacter::SetPerspective(bool InIsFirstPersonPerspective)
{
	if(IsLocallyControlled())
	{
		if (InIsFirstPersonPerspective)
		{
			GetMesh()->SetVisibility(false, false);
			FirstPersonMesh->SetVisibility(true, false);
		}
		else
		{
			FirstPersonMesh->SetVisibility(false, false);
			GetMesh()->SetVisibility(true, false);
		}
	}
}