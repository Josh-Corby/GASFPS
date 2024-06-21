// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/GSInteractableInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Add default functionality here for any IGSInteractableInterface functions that are not pure virtual.

bool IGSInteractableInterface::IsAvailableForInteraction_Implementation(AActor* InteractionActor) const
{
	return false;
}

float IGSInteractableInterface::GetInteractionDuration_Implementation(AActor* InteractionActor) const
{
	return 0.0f;
}

void IGSInteractableInterface::GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, AActor* InteractionActor) const
{
	bShouldSync = false;
	Type = EAbilityTaskNetSyncType::OnlyServerWait;
}

void IGSInteractableInterface::GetPostInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, AActor* InteractionActor) const
{
	bShouldSync = false;
	Type = EAbilityTaskNetSyncType::OnlyServerWait;
}

FSimpleMulticastDelegate* IGSInteractableInterface::GetTargetCancelInteractionDelegate(AActor* InteractionActor)
{
	return nullptr;
}

void IGSInteractableInterface::RegisterInteracter_Implementation(AActor* InteractionActor, AActor* InteractingActor)
{
	if (Interacters.Contains(InteractionActor))
	{
		TArray<AActor*>& InteractingActors = Interacters[InteractionActor];
		if (!InteractingActors.Contains(InteractingActor))
		{
			InteractingActors.Add(InteractingActor);
		}
	}
	else
	{
		TArray<AActor*> InteractingActors;
		InteractingActors.Add(InteractingActor);
		Interacters.Add(InteractionActor, InteractingActors);
	}
}

void IGSInteractableInterface::UnregisterInteracter_Implementation(AActor* InteractionActor, AActor* InteractingActor)
{
	if (Interacters.Contains(InteractionActor))
	{
		TArray<AActor*>& InteractingActors = Interacters[InteractionActor];
		InteractingActors.Remove(InteractingActor);
	}
}

void IGSInteractableInterface::InteractableCancelInteraction_Implementation(AActor* InteractionActor)
{
	if (Interacters.Contains(InteractionActor))
	{
		FGameplayTagContainer InteractAbilityTagContainer;
		InteractAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag("Ability.Interaction"));

		TArray<AActor*>& InteractingActors = Interacters[InteractionActor];
		for (AActor* InteractingActor : InteractingActors)
		{
			UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractingActor);

			if (ASC)
			{
				ASC->CancelAbilities(&InteractAbilityTagContainer);
			}
		}

		InteractingActors.Empty();
	}
}
