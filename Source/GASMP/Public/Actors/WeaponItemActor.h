// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseItemActor.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API AWeaponItemActor : public ABaseItemActor
{
	GENERATED_BODY()
	
public:

	AWeaponItemActor();

	const UWeaponStaticData* GetWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

protected:

	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;
};