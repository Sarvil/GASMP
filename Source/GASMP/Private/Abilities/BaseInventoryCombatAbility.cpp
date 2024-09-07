// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BaseInventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../GASMPTypes.h"
#include "Actors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/BaseGASPlayer.h"
#include "Camera/CameraComponent.h"

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
const bool UBaseInventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult &InHitResult)
{
    AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();
    ABaseGASPlayer* GASCharacter = Cast<ABaseGASPlayer>(GetActionGameCharacterFromActorInfo());
    const FTransform& CameraTransform = GASCharacter->GetFollowCamera()->GetComponentTransform();
    const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;
    TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};
    FHitResult FocusHit;
    UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, FocusHit, true);
    FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();
    const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;
    UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, InHitResult, true);
    return InHitResult.bBlockingHit;
}