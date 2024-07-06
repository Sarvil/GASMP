// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlayerAnimInstance.h"
#include "Actors/BaseGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    GASCharacter = Cast<ABaseGASCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if(GASCharacter == nullptr)
    {
        GASCharacter = Cast<ABaseGASCharacter>(TryGetPawnOwner());
    }
    if(GASCharacter == nullptr) return;
    FVector Velocity = GASCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = GASCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = GASCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}
