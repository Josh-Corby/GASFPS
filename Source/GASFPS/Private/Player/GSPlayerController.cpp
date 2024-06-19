// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSPlayerController.h"

#include "Input/GSInputComponent.h"
#include "GSGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "InputActionValue.h"

AGSPlayerController::AGSPlayerController()
{
	bReplicates = true;
}

void AGSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputContext);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AGSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UGSInputComponent* GSInputComponent = CastChecked<UGSInputComponent>(InputComponent);


	GSInputComponent->BindNativeAction(InputConfig, FGSGameplayTags::Get().InputTag_Move, ETriggerEvent::Triggered, this, &AGSPlayerController::Input_Move, false);
	GSInputComponent->BindNativeAction(InputConfig, FGSGameplayTags::Get().InputTag_Look, ETriggerEvent::Triggered, this, &AGSPlayerController::Input_Look, false);
	GSInputComponent->BindAbilityActions(InputConfig, this, &AGSPlayerController::AbilityInputTagPressed, &AGSPlayerController::AbilityInputTagReleased);
}

void AGSPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UGSAbilitySystemComponent* GSASC = Cast<UGSAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn())))
	{
		GSASC->AbilityInputTagPressed(InputTag);
	}
}

void AGSPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UGSAbilitySystemComponent* GSASC = Cast<UGSAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn())))
	{
		GSASC->AbilityInputTagReleased(InputTag);
	}
}

void AGSPlayerController::Input_Move(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();

	if (!ControlledPawn)
	{
		return;
	}

	FVector2D InputVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	if (InputVector.X != 0.f)
	{
		const FVector ForwardVector = YawRotation.RotateVector(FVector::RightVector);
		ControlledPawn->AddMovementInput(ForwardVector, InputVector.X);
	}

	if (InputVector.Y != 0.f)
	{
		const FVector RightVector = YawRotation.RotateVector(FVector::ForwardVector);
		ControlledPawn->AddMovementInput(RightVector, InputVector.Y);
	}
}

void AGSPlayerController::Input_Look(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();

	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D InputValue = Value.Get<FVector2D>();

	if (InputValue.X != 0.f)
	{
		ControlledPawn->AddControllerYawInput(InputValue.X);
	}

	if (InputValue.Y != 0.f)
	{
		ControlledPawn->AddControllerPitchInput(InputValue.Y);
	}
}
