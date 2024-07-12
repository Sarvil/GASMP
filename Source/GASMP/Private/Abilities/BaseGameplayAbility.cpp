// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Actors/BaseGASCharacter.h"

UBaseGameplayAbility::UBaseGameplayAbility()
{
    // Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
ABaseGASCharacter *UBaseGameplayAbility::GetActionGameCharacterFromActorInfo() const
{
    return Cast<ABaseGASCharacter>(GetAvatarActorFromActorInfo());
}