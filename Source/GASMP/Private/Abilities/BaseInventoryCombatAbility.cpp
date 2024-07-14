// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BaseInventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../GASMPTypes.h"

FGameplayEffectSpecHandle UBaseInventoryCombatAbility::GetWeaponEffectSpec(const FHitResult &InHitResult)
{
    if(UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if(const UWeaponStaticData* WeaponStaticData = GetEquippedWeaponItemStaticData())
        {
            FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            FGameplayEffectSpecHandle OutSpec = ASC->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);
            return OutSpec;
        }
    }
    return FGameplayEffectSpecHandle();
}