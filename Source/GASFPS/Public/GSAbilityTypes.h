// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GSAbilityTypes.generated.h"

class UGameplayEffect;
class IGSAbilitySourceInterface;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbiltyLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
};

USTRUCT(BlueprintType)
struct  FGSGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	FGSGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FGSGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FGSGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static GASFPS_API FGSGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	///** Sets the object used as the ability source */
	void SetAbilitySource(const IGSAbilitySourceInterface* InObject, float InSourceLevel);

	///** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const IGSAbilitySourceInterface* GetAbilitySource() const;

	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	virtual FGSGameplayEffectContext* Duplicate() const
	{
		FGSGameplayEffectContext* NewContext = new FGSGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	// Getters
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDuration() const { return DebuffDuration; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }


	// Setters
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }

protected:

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDuration = 0.f;

	TSharedPtr<FGameplayTag> DamageType;

	/** Ability Source object (should implement IGSAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FGSGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FGSGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};