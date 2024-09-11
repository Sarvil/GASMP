// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "SprintGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API USprintGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:

	USprintGameplayAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
    float SprintSpeedMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
    float StaminaCostPerSecond;
};
