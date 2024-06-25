// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonPlayerController.h"

#include "GSPlayerController.generated.h"

class AGSPlayerState;
class UGSAbilitySystemComponent;

UCLASS()
class GASFPS_API AGSPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
	
public:

	AGSPlayerController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "GS|PlayerController")
	AGSPlayerState* GetGSPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "GS|PlayerController")
	UGSAbilitySystemComponent* GetGSAbilitySystemComponent() const;

	//~AController interface
	virtual void OnUnPossess() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

};
