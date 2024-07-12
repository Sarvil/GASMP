// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GASCharacterMovementComponent.h"
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