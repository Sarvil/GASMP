// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_WallRun.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityTasks/AbilityTask_TickWallRun.h"
#include "Actors/BaseGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGameplayAbility_WallRun::UGameplayAbility_WallRun()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);
	UE_LOG(LogTemp, Warning, TEXT("OnGiveAbility called for Actor: %s"), *ActorInfo->OwnerActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OnGiveAbility called for Actor: %s"), *ActorInfo->AvatarActor->GetName());


	if (ActorInfo && ActorInfo->AvatarActor.IsValid() && Spec.Handle.IsValid())
	{
		ABaseGASCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
		if(CharacterAvatar)
		{
			UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();
			CapsuleComponent->OnComponentHit.AddDynamic(this, &UGameplayAbility_WallRun::OnCapsuleComponentHit);
		}
	}
}

void UGameplayAbility_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		ABaseGASCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
		UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

		CapsuleComponent->OnComponentHit.RemoveDynamic(this, &UGameplayAbility_WallRun::OnCapsuleComponentHit);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGameplayAbility_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
    if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	ABaseGASCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
	return CharacterAvatar && !CharacterAvatar->GetCharacterMovement()->IsMovingOnGround();
}

void UGameplayAbility_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WallRunTickTask = UAbilityTask_TickWallRun::CreateWallRunTask(this, Cast<ACharacter>(GetAvatarActorFromActorInfo()), Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent), Wall_Run_TraceObjectsTypes);

	WallRunTickTask->OnFinished.AddDynamic(this, &UGameplayAbility_WallRun::K2_EndAbility);
	WallRunTickTask->OnWallSideDetermined.AddDynamic(this, &UGameplayAbility_WallRun::OnWallSideDetermined);
	
	WallRunTickTask->ReadyForActivation();
}

void UGameplayAbility_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsValid(WallRunTickTask))
	{
		WallRunTickTask->EndTask();
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass, AbilitySystemComponent);
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass, AbilitySystemComponent);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_WallRun::OnCapsuleComponentHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
	}
}

void UGameplayAbility_WallRun::OnWallSideDetermined(bool bLeftSide)
{
    ABaseGASCharacter* CharacterAvatar = GetActionGameCharacterFromActorInfo();
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	if (bLeftSide)
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunLeftSideEffectClass, EffectContextHandle);
	}
	else
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunRightSideEffectClass, EffectContextHandle);
	}
}
