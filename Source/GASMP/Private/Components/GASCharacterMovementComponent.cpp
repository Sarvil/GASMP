// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GASCharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
    TEXT("ShowDebugTraversal"),
    0,
    TEXT("Show Debug about Traversal")
    TEXT("  0:Off/n")
    TEXT("  1:On/n"),
    ECVF_Cheat
);

bool UGASCharacterMovementComponent::TryTraversal(UAbilitySystemComponent *ASC)
{
    for(TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
    {
        if(ASC->TryActivateAbilityByClass(AbilityClass, true))
        {
            FGameplayAbilitySpec* Spec;
            Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
            if(Spec && Spec->IsActive())
            {
                return true;
            }
        }
    }
    return false;
}

void UGASCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    HandleMovementDirection();
    if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe"), EGameplayTagEventType::NewOrRemoved)).AddUObject(this, &UGASCharacterMovementComponent::OnEnforcedStrafeTagChanged);
    }
}

EMovementDirectionType UGASCharacterMovementComponent::GetMovementDirectionType() const
{
    return MovementDirectionType;
}

void UGASCharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
    MovementDirectionType = InMovementDirectionType;
    HandleMovementDirection();
}

void UGASCharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if(NewCount)
    {
        SetMovementDirectionType(EMovementDirectionType::Strafe);
    }
    else
    {
        SetMovementDirectionType(EMovementDirectionType::OrientMovement);
    }
}

void UGASCharacterMovementComponent::HandleMovementDirection()
{
    switch (MovementDirectionType)
    {
        case EMovementDirectionType::None:
        break;
        case EMovementDirectionType::Strafe:
        bUseControllerDesiredRotation = true;
        bOrientRotationToMovement = false;
        CharacterOwner->bUseControllerRotationYaw = true;
        break;
        case EMovementDirectionType::OrientMovement:
        break;
        default:
        bUseControllerDesiredRotation = false;
        bOrientRotationToMovement = true;
        CharacterOwner->bUseControllerRotationYaw = false;
        break;
    }
}
