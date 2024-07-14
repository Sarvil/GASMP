// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Actors/BaseItemActor.h"
#include "Components/GASMPStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    ItemStaticDataClass = InItemStaticDataClass;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass); 
    DOREPLIFETIME(UInventoryItemInstance, bEquipped); 
    DOREPLIFETIME(UInventoryItemInstance, ItemActor); 
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
    return UGASMPStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
    if(GEngine)
	    {
		    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Reached InventoryInstance")));
	    }
    if(UWorld* World = InOwner->GetWorld())
    {
        if(GEngine)
	    {
		    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Reached inside GetWorld")));
	    }
        const UItemStaticData* ItemData = GetItemStaticData();
        FTransform Transform;
        ItemActor = World->SpawnActorDeferred<ABaseItemActor>(ItemData->ItemActorClass, Transform, InOwner);
        ItemActor->Init(this);
        ItemActor->OnEquipped();
        ItemActor->FinishSpawning(Transform);
        ACharacter* Character = Cast<ACharacter>(InOwner);
        if(GEngine)
	    {
		    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Reached InventoryInstance : %s"), Character));
	    }
        if(USkeletalMeshComponent* SkeletelMesh = Character ? Character->GetMesh() : nullptr)
        {
            ItemActor->AttachToComponent(SkeletelMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemData->AttachmentSocket);
        }
    }
    TryGrantAbilities(InOwner);
    TryApplyEffects(InOwner);
    bEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped(AActor* InOwner)
{
     if(ItemActor)
     {
        ItemActor->Destroy();
        ItemActor = nullptr;
     }
    TryRemoveAbilities(InOwner);
    TryRemoveEffects(InOwner);
    bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
    if(ItemActor)
    {
        ItemActor->OnDropped();
    }
    TryRemoveAbilities(InOwner);
    TryRemoveEffects(InOwner);
    bEquipped = false;
}

ABaseItemActor *UInventoryItemInstance::GetItemActor() const
{
    return ItemActor;
}

void UInventoryItemInstance::TryGrantAbilities(AActor *InOwner)
{
    if(InOwner && InOwner->HasAuthority())
    {
        if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
        {
            const UItemStaticData* StaticData = GetItemStaticData();
            for(auto ItemAbility : StaticData->GrantedAbilities)
            {
                GrantedAbilityHandles.Add(ASC->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
            }
        }
    }
}

void UInventoryItemInstance::TryRemoveAbilities(AActor *InOwner)
{
    if(InOwner && InOwner->HasAuthority())
    {
        if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
        {
            const UItemStaticData* StaticData = GetItemStaticData();
            for(auto AbilityHandle : GrantedAbilityHandles)
            {
                ASC->ClearAbility(AbilityHandle);
            }
            GrantedAbilityHandles.Empty();
        }
    }
}

void UInventoryItemInstance::TryApplyEffects(AActor *InOwner)
{
    if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
    {
        const UItemStaticData* ItemStaticData = GetItemStaticData();
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        for(auto GameplayEffect : ItemStaticData->OngoingEffects)
        {
            if(!GameplayEffect.Get()) continue;
            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
            if(SpecHandle.IsValid())
            {
                FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if(!ActiveGEHandle.WasSuccessfullyApplied())
                {
                    
                }
                else
                {
                    OngoingEffectHandles.Add(ActiveGEHandle);
                }
            }
        }
    }
}

void UInventoryItemInstance::TryRemoveEffects(AActor *InOwner)
{
    if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
    {
        for(FActiveGameplayEffectHandle ActiveEffectHandle : OngoingEffectHandles)
        {
            if(ActiveEffectHandle.IsValid())
            {
                ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
            }
        }
    }
    OngoingEffectHandles.Empty();
}
