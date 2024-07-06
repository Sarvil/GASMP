// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BasePhysicalMaterial.generated.h"

UCLASS()
class GASMP_API UBasePhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Materials")
	class USoundBase* FootstepSound = nullptr;
};
