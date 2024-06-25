// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Player/GSPlayerState.h"


AGSPlayerController::AGSPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGSPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

void AGSPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UGSAbilitySystemComponent* GSASC = GetGSAbilitySystemComponent())
	{
		GSASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AGSPlayerState* AGSPlayerController::GetGSPlayerState() const
{
	return CastChecked<AGSPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UGSAbilitySystemComponent* AGSPlayerController::GetGSAbilitySystemComponent() const
{
	const AGSPlayerState* GSPS = GetGSPlayerState();
	return (GSPS ? GSPS->GetGSAbilitySystemComponent() : nullptr);
}