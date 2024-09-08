// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "Actors/AIProjectile.h"
#include "Actors/BaseGASPlayer.h"
#include "Actors/BaseAICharacter.h"
#include "AIGameplayAbility_Attack.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API UAIGameplayAbility_Attack : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
    UAIGameplayAbility_Attack();

protected:
    // Override the ActivateAbility method to define the ability's behavior
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // Helper function to move the boss towards the player
    void MoveTowardsPlayer(ACharacter* BossCharacter, ACharacter* PlayerCharacter);

    // Helper function to shoot a projectile towards the player
    void ShootProjectile(const FGameplayAbilityActorInfo *ActorInfo);

    // Projectile class to spawn
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<class AAIProjectile> ProjectileClass;

    // Distance to maintain from the player before shooting
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MoveDistance = 500.0f;

    // Speed at which the boss moves towards the player
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MoveSpeed = 600.0f;
};
