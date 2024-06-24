// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSHeroComponent.h"

#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "Character/GSCharacterBase.h"
#include "Character/GSPawnData.h"
#include "Character/GSPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "GSGameplayTags.h"
#include "Input/GSInputComponent.h"
#include "Input/GSInputConfig.h"
#include "InputMappingContext.h"
#include "Player/GSLocalPlayer.h"
#include "Player/GSPlayerController.h"
#include "Player/GSPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GSHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

const FName UGSHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UGSHeroComponent::NAME_ActorFeatureName("Hero");

UGSHeroComponent::UGSHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void UGSHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogTemp, Error, TEXT("[UGSHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("GSHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("GSHeroComponent");

			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UGSHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == GameplayTags.InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == GameplayTags.InitState_Spawned && DesiredState == GameplayTags.InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AGSPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			AGSPlayerController* GSPC = GetController<AGSPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !GSPC || !GSPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == GameplayTags.InitState_DataAvailable && DesiredState == GameplayTags.InitState_DataInitialized)
	{
		// Wait for player state and extension component
		AGSPlayerState* GSPS = GetPlayerState<AGSPlayerState>();

		return GSPS && Manager->HasFeatureReachedInitState(Pawn, UGSPawnExtensionComponent::NAME_ActorFeatureName, GameplayTags.InitState_DataInitialized);
	}
	else if (CurrentState == GameplayTags.InitState_DataInitialized && DesiredState == GameplayTags.InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UGSHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();

	if (CurrentState == GameplayTags.InitState_DataAvailable && DesiredState == GameplayTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AGSPlayerState* GSPS = GetPlayerState<AGSPlayerState>();
		if (!ensure(Pawn && GSPS))
		{
			return;
		}

		const UGSPawnData* PawnData = nullptr;

		if (UGSPawnExtensionComponent* PawnExtComp = UGSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UGSPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(GSPS->GetGSAbilitySystemComponent(), GSPS);
		}

		if (AGSPlayerController* GSPC = GetController<AGSPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		/*if (PawnData)
		{
			if (ULyraCameraComponent* CameraComponent = ULyraCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}*/
	}
}

void UGSHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UGSPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FGSGameplayTags::Get().InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UGSHeroComponent::CheckDefaultInitialization()
{
	const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { GameplayTags.InitState_Spawned, GameplayTags.InitState_DataAvailable, GameplayTags.InitState_DataInitialized, GameplayTags.InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UGSHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(FGSGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UGSHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UGSHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UGSLocalPlayer* LP = Cast<UGSLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UGSPawnExtensionComponent* PawnExtComp = UGSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UGSPawnData* PawnData = PawnExtComp->GetPawnData<UGSPawnData>())
		{
			if (const UGSInputConfig* InputConfig = PawnData->InputConfig)
			{
				if (DefaultInputMapping)
				{
					Subsystem->AddMappingContext(DefaultInputMapping, 0);
				}

				UGSInputComponent* GSIC = Cast<UGSInputComponent>(PlayerInputComponent);
				if (ensureMsgf(GSIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
				{
					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					GSIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);

					const FGSGameplayTags& GameplayTags = FGSGameplayTags::Get();
					GSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					GSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

bool UGSHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UGSHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UGSPawnExtensionComponent* PawnExtComp = UGSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UGSAbilitySystemComponent* GSASC = PawnExtComp->GetGSAbilitySystemComponent())
			{
				GSASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UGSHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UGSPawnExtensionComponent* PawnExtComp = UGSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UGSAbilitySystemComponent* GSASC = PawnExtComp->GetGSAbilitySystemComponent())
		{
			GSASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UGSHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UGSHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}
