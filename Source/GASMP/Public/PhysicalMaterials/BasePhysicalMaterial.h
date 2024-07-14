// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BasePhysicalMaterial.generated.h"

class USoundBase;
class UNiagaraSystem;
class UParticleSystem;

UCLASS()
class GASMP_API UBasePhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Materials")
	USoundBase* FootstepSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Materials")
	USoundBase* PointImpactSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Materials")
	UNiagaraSystem* PointImpactVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Materials")
	UParticleSystem* PointImpactCVFX = nullptr;
};
