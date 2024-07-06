// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GASMPTypes.h"
#include "InventoryItemInstance.generated.h"

/**
 * 
 */
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
	virtual void OnUnEquipped();
	virtual void OnDropped();

protected:

	UPROPERTY(Replicated)
	ABaseItemActor* ItemActor;
};
