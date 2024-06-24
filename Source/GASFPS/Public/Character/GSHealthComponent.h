// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGSHealth_AttributeChanged, UGSHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGSHealth_OutOfHealth, AActor*, OwningActor);

class UGSHealthSet;
class UGSAbilitySystemComponent;
class UGSHealthSet;
struct FGameplayEffectSpec;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASFPS_API UGSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGSHealthComponent();

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "GS|Health")
	void InitializeWithAbilitySystem(UGSAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "GS|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "GS|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "GS|Health")
	float GetMaxHealth() const;

public:

	UPROPERTY(BlueprintAssignable)
	FGSHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FGSHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FGSHealth_OutOfHealth OnOutOfHealth;


protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UGSHealthSet> HealthSet;
};
