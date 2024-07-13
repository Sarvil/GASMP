// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "BaseInventoryAbility.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UWeaponStaticData;
class AWeaponItemActor;

UCLASS()
class GASMP_API UBaseInventoryAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent() const;
	
	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance() const;

	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData() const;

	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedWeaponItemStaticData() const;

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor() const;

protected:

	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;
};
