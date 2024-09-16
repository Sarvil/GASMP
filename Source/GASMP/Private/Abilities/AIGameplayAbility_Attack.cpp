// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AIGameplayAbility_Attack.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Actors/AIProjectile.h"
#include "Actors/BaseGASPlayer.h"
#include "Actors/BaseAICharacter.h"
#include "Controllers/GASAIController.h"

UAIGameplayAbility_Attack::UAIGameplayAbility_Attack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UAIGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    // ABaseAICharacter* BossCharacter = Cast<ABaseAICharacter>(ActorInfo->AvatarActor.Get());
    ABaseAICharacter *BossCharacter = CastChecked<ABaseAICharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
    ABaseGASPlayer *PlayerCharacter = Cast<ABaseGASPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (BossCharacter && PlayerCharacter)
    {
        // Move the boss towards the player
        MoveTowardsPlayer(BossCharacter, PlayerCharacter);

        // After moving, shoot the projectile
        // ShootProjectile(ActorInfo);
        if (ProjectileClass)
        {
            FVector MuzzleLocation = BossCharacter->GetMesh()->GetSocketLocation("RifleSocket");
            FRotator MuzzleRotation = (PlayerCharacter->GetActorLocation() - MuzzleLocation).Rotation();

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = BossCharacter;
            SpawnParams.Instigator = BossCharacter;

            AAIProjectile *Projectile = GetWorld()->SpawnActor<AAIProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            if (Projectile)
            {
                Projectile->FireInDirection(MuzzleRotation.Vector());
            }
        }

        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UAIGameplayAbility_Attack::MoveTowardsPlayer(ACharacter *BossCharacter, ACharacter *PlayerCharacter)
{
    FVector BossLocation = BossCharacter->GetActorLocation();
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();

    FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
    FVector TargetLocation = PlayerLocation - Direction * MoveDistance;

    AAIController *AIController = Cast<AAIController>(BossCharacter->GetController());

    if (AIController)
    {
        // Use MoveToLocation to move the AI smoothly towards the target location
        AIController->MoveToLocation(TargetLocation, -1.0f, true, true, false, true, 0, true);
    }
    //BossCharacter->GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
}

void UAIGameplayAbility_Attack::ShootProjectile(const FGameplayAbilityActorInfo *ActorInfo)
{
    if (ProjectileClass)
    {
        ACharacter *BossCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
        ACharacter *PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        FVector MuzzleLocation = BossCharacter->GetMesh()->GetSocketLocation("RifleSocket"); // Assuming you have a socket named "Muzzle_01"
        FRotator MuzzleRotation = (PlayerCharacter->GetActorLocation() - MuzzleLocation).Rotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = BossCharacter;
        SpawnParams.Instigator = BossCharacter;

        AAIProjectile *Projectile = GetWorld()->SpawnActor<AAIProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
        if (Projectile)
        {
            Projectile->FireInDirection(MuzzleRotation.Vector());
        }
    }
}
