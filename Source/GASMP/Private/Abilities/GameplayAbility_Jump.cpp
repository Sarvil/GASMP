// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Jump.h"
#include "GASMP/GASMP.h"
#include "Actors/BaseGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UGameplayAbility_Jump::UGameplayAbility_Jump()
{
    AbilityInputID = EGASAbilityInputID::Jump;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
}

void UGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
		const bool bIsWallrunning = ASC && ASC->HasMatchingGameplayTag(WallrunStateTag);
		if(bIsWallrunning)
		{
			FGameplayTagContainer WallRunTags(WallrunStateTag);
			ASC->CancelAbilities(&WallRunTags);
			FVector JumpOffVector = Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;
			Character->LaunchCharacter(JumpOffVector * OffWallJumpStrength, true, true);
		}
		else
		{
			Character->Jump();
		}
	}
}

bool UGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ABaseGASCharacter* Character = CastChecked<ABaseGASCharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);

	const bool bMovementAllowsJump = Character->GetMovementComponent()->IsJumpAllowed();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
	const bool bIsWallrunning = ASC && ASC->HasMatchingGameplayTag(WallrunStateTag);

	return Character && Character->CanJump() || (bMovementAllowsJump && bIsWallrunning);
}

void UGameplayAbility_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

// Epic's comment
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */

void UGameplayAbility_Jump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplayAbility_Jump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}