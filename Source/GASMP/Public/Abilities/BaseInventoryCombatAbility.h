// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseInventoryAbility.h"
#include "BaseInventoryCombatAbility.generated.h"

UCLASS()
class GASMP_API UBaseInventoryCombatAbility : public UBaseInventoryAbility
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);

	UFUNCTION(BlueprintPure)
	const bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& InHitResult);
};
