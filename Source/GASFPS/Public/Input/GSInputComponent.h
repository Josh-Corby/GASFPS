// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GSInputConfig.h"
#include "GameplayTagContainer.h"

#include "GSInputComponent.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class GASFPS_API UGSInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

	UGSInputComponent(const FObjectInitializer& ObjectInitializer);

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UGSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UGSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);

};

template<class UserClass, typename FuncType>
void UGSInputComponent::BindNativeAction(const UGSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}


template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UGSInputComponent::BindAbilityActions(const UGSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FGSInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}
