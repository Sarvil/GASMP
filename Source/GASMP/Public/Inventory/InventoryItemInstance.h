// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GASMPTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "InventoryItemInstance.generated.h"

class ABaseItemActor;

UCLASS(BlueprintType, Blueprintable)
class GASMP_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:

	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSupportedForNetworking() const override {return true;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnEquipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

	UFUNCTION(BlueprintPure)
	ABaseItemActor* GetItemActor() const;

protected:

	UPROPERTY(Replicated)
	ABaseItemActor* ItemActor;

	void TryGrantAbilities(AActor* InOwner);
	void TryRemoveAbilities(AActor* InOwner);

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};
