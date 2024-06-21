// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GSPlayerController.generated.h"

class UGSInputConfig;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class GASFPS_API AGSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AGSPlayerController();

	UFUNCTION(Client, Reliable, WithValidation)
	void ClientSetControlRotation(FRotator NewRotation);
	void ClientSetControlRotation_Implementation(FRotator NewRotation);
	bool ClientSetControlRotation_Validate(FRotator NewRotation);

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

private:

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSInputConfig> InputConfig;


};
