// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override; 

private:

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	class ABaseGASCharacter* GASCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
};
