// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../../GASMPTypes.h"
#include "AnimNotify_Step.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API UAnimNotify_Step : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	EFoot Foot;

	virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
	
};
