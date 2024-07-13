// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "PlayerAnimInstance.generated.h"

class UItemStaticData;

UCLASS()
class GASMP_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override; 

protected:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallrunStateTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallrunRightStateTag;

	const UItemStaticData* GetItemEquippedData() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	class UBlendSpace* GetUnEquippedLocomotion() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	class UAnimSequenceBase* GetUnEquippedIdleAnimation() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;

private:

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	class ABaseGASCharacter* GASCharacter;

	/* UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	class AGASActorPlayerState* GASPlayerState; */

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float Forward;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float Side;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

 	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bIsWallRunning;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta = (AllowPrivateAccess = "true"))
	bool bMirrorWallRun;
};
