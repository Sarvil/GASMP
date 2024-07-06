// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryList.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/BaseItemActor.h"
#include "GameplayTagsManager.h"

FGameplayTag UInventoryComponent::EquipItemActorTag;
FGameplayTag UInventoryComponent::DropItemTag;
FGameplayTag UInventoryComponent::EquipNextItemTag;
FGameplayTag UInventoryComponent::UnEquipTag;

static TAutoConsoleVariable<int32> CVarShowDebugInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws debug info about Inventory")
	TEXT("0 :off\n")
	TEXT("1: on\n"),
	ECVF_Cheat
);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
	static bool bHandledAddingTags = false;
	if(!bHandledAddingTags)
	{
		bHandledAddingTags = true;
		UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this, &UInventoryComponent::AddInventoryTags);
	}
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(GetOwner()->HasAuthority())
	{
		for(auto ItemClass : DefaultItems)
		{
			InventoryList.AddItem(ItemClass);
		}
	}
	if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipItemActorTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::DropItemTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipNextItemTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::UnEquipTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool wroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(FInventoryListItem& Item : InventoryList.GetItemsRef())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;
		if(IsValid(ItemInstance))
		{
			wroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

    return wroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
	DOREPLIFETIME(UInventoryComponent, CurrentItem);
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance *InItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemInstance);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance *InItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		for(auto Item : InventoryList.GetItemsRef())
		{
			if(Item.ItemInstance == InItemInstance)
			{
				APlayerState* OwnedState = Cast<APlayerState>(GetOwner());
				AActor* OwnedActor = Cast<AActor>(OwnedState->GetPawn());
				Item.ItemInstance->OnEquipped(OwnedActor);
				CurrentItem = Item.ItemInstance; 
				break; 
			}
		}
	}
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if(GetOwner()->HasAuthority())
	{
		for(auto Item : InventoryList.GetItemsRef())
		{
			
			if(Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
			{
				APlayerState* OwnedState = Cast<APlayerState>(GetOwner());
				AActor* OwnedActor = Cast<AActor>(OwnedState->GetPawn());
				Item.ItemInstance->OnEquipped(OwnedActor);
				CurrentItem = Item.ItemInstance; 
				break; 
			}
		}
	}
}

void UInventoryComponent::UnEquipItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnUnEquipped();
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{

			CurrentItem->OnUnEquipped();
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
			
		}
	}
}

void UInventoryComponent::EquipNextItem()
{
	TArray<FInventoryListItem>& Items = InventoryList.GetItemsRef();
	const bool bNoItems = Items.Num() == 0;
	const bool bOneAndEquipped = Items.Num() == 1 && CurrentItem;
	if(bNoItems || bOneAndEquipped) return;
	
	UInventoryItemInstance* TargetItem = CurrentItem;

	for(auto Item : Items)
	{
		if(Item.ItemInstance->GetItemStaticData()->bCanBeEquipped)
		{
			if(Item.ItemInstance != CurrentItem)
			{
				TargetItem = Item.ItemInstance;
				break;
			}
		}
	}
	if(CurrentItem)
	{
		if(TargetItem == CurrentItem)
		{
			return;
		}
		UnEquipItem();
	}
	EquipItemInstance(TargetItem);
}

UInventoryItemInstance *UInventoryComponent::GetEquippedItem() const
{
    return CurrentItem;
}

void UInventoryComponent::GameplayEventCallback(const FGameplayEventData *Payload)
{
	ENetRole NetRole = GetOwnerRole();
	if(NetRole == ROLE_Authority)
	{
		HandleGameplayEventInternal(*Payload);
	}
	else if(NetRole == ROLE_AutonomousProxy)
	{
		ServerHandleGameplayEvent(*Payload);
	}
}

void UInventoryComponent::AddInventoryTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	UInventoryComponent::EquipItemActorTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"),TEXT("Equip Item from Item Actor Event"));
	UInventoryComponent::DropItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"),TEXT("Drop Equipped Item"));
	UInventoryComponent::EquipNextItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNextItem"),TEXT("Try Equip Next Item"));
	UInventoryComponent::UnEquipTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.UnEquip"),TEXT("Unequip Current Item"));

	TagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
}

void UInventoryComponent::HandleGameplayEventInternal(FGameplayEventData Payload)
{
	ENetRole NetRole = GetOwnerRole();
	if(NetRole == ROLE_Authority)
	{
		FGameplayTag EventTag = Payload.EventTag;

		if(EventTag == UInventoryComponent::EquipItemActorTag)
		{
			if(const UInventoryItemInstance* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
			{
				AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));

				if(Payload.Instigator)
				{
					const_cast<AActor*>(Payload.Instigator.Get())->Destroy();
				}
			}
		}
		else if(EventTag == UInventoryComponent::EquipNextItemTag)
		{
			EquipNextItem();
		}
		else if(EventTag == UInventoryComponent::DropItemTag)
		{
			DropItem();
		}
		else if(EventTag == UInventoryComponent::UnEquipTag)
		{
			UnEquipItem();
		}
	}
}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
	HandleGameplayEventInternal(Payload);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bShowDebug = CVarShowDebugInventory.GetValueOnGameThread() != 0;
	if(bShowDebug)
	{
		for(auto Item : InventoryList.GetItemsRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();
			if(Item.ItemInstance)
			{
				if(GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Item: %s"), *ItemStaticData->Name.ToString()));
					}
			}
		}
	}
}

