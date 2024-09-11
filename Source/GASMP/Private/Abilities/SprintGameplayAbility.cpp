// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/SprintGameplayAbility.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AttributeSets/BaseAttributeSet.h"
#include "PlayerStates/GASActorPlayerState.h"

USprintGameplayAbility::USprintGameplayAbility()
{
    AbilityInputID = EGASAbilityInputID::Sprint;
    bReplicateInputDirectly = true;
    SprintSpeedMultiplier = 2.0f; // Sprint speed is 2x normal speed
    StaminaCostPerSecond = 10.0f; // Cost 10 stamina per second
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USprintGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        AGASActorPlayerState *PS = Cast<AGASActorPlayerState>(ActorInfo->OwnerActor.Get());
        if (PS)
        {
            UBaseAttributeSet *AS = Cast<UBaseAttributeSet>(PS->GetBaseAttributeSet());
            if(AS)
            {
                ACharacter *Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
                AS->SetMoveSpeed(Character->GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier);
            }
        }

        // Implement stamina reduction over time using gameplay effect
        // Apply a gameplay effect here to handle stamina drain
    }
}

void USprintGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USprintGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    if (ActorInfo != NULL && ActorInfo->AvatarActor.Get() != NULL)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
    }
}

void USprintGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    if (ScopeLockCount > 0)
    {
        WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &USprintGameplayAbility::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
        return;
    }
    AGASActorPlayerState *PS = Cast<AGASActorPlayerState>(ActorInfo->OwnerActor.Get());
        if (PS)
        {
            UBaseAttributeSet *AS = Cast<UBaseAttributeSet>(PS->GetBaseAttributeSet());
            if(AS)
            {
                ACharacter *Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
                AS->SetMoveSpeed(Character->GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier);
            }
        }
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
