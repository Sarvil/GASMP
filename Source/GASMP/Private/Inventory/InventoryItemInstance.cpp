// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Actors/BaseItemActor.h"
#include "Components/GASMPStatics.h"
#include "Components/SkeletalMeshComponent.h"
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
    bEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped()
{
     if(ItemActor)
     {
        ItemActor->Destroy();
        ItemActor = nullptr;
     }
     bEquipped = false;
}

void UInventoryItemInstance::OnDropped()
{
    if(ItemActor)
    {
        ItemActor->OnDropped();
    }
    bEquipped = false;
}
