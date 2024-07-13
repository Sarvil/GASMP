// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BaseInventoryAbility.h"
#include "Components/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "GASMPTypes.h"
#include "Actors/WeaponItemActor.h"
#include "Actors/BaseItemActor.h"

void UBaseInventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);
    InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent *UBaseInventoryAbility::GetInventoryComponent() const
{
    return InventoryComponent;
}

UInventoryItemInstance *UBaseInventoryAbility::GetEquippedItemInstance() const
{
    return InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
}

const UItemStaticData *UBaseInventoryAbility::GetEquippedItemStaticData() const
{
    UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
    return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
}

const UWeaponStaticData *UBaseInventoryAbility::GetEquippedWeaponItemStaticData() const
{
    return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}

ABaseItemActor *UBaseInventoryAbility::GetEquippedItemActor() const
{
    UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
    return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}

AWeaponItemActor *UBaseInventoryAbility::GetEquippedWeaponItemActor() const
{
    return Cast<AWeaponItemActor>(GetEquippedItemActor());
}
