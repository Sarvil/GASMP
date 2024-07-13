// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlayerAnimInstance.h"
#include "Actors/BaseGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GASMP/GASMPTypes.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Components/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "AbilitySystemBlueprintLibrary.h"

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
    
    FVector Result = GASCharacter->GetActorRotation().UnrotateVector(GASCharacter->GetVelocity());
    Forward = Result.X;
    Side = Result.Y;

    bIsInAir = GASCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = GASCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    /*FGameplayTag WallRunTag = FGameplayTag::RequestGameplayTag(FName("State.Movement.WallRun"));
    FGameplayTag WallRunDirectionTag = FGameplayTag::RequestGameplayTag(FName("State.Movement.WallRun.Right"));
    bIsWallRunning = GASPlayerState->GetAbilitySystemComponent()->HasMatchingGameplayTag(WallRunTag);
    bMirrorWallRun = GASPlayerState->GetAbilitySystemComponent()->HasMatchingGameplayTag(WallRunDirectionTag); */
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GASCharacter);
	bIsWallRunning = ASC && ASC->HasMatchingGameplayTag(WallrunStateTag);
	bMirrorWallRun = ASC && ASC->HasMatchingGameplayTag(WallrunRightStateTag);
}

const UItemStaticData *UPlayerAnimInstance::GetItemEquippedData() const
{
    UInventoryComponent* InventoryComponent = GASCharacter ? GASCharacter->GetInventoryComponent() : nullptr;
    UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
    return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace *UPlayerAnimInstance::GetUnEquippedLocomotion() const
{
    if(GASCharacter)
    {
        if(const UItemStaticData* ItemData = GetItemEquippedData())
        {
            if(ItemData->CharacterAnimationData.UnEquippedMovementBlendSpace)
            {
                return ItemData->CharacterAnimationData.UnEquippedMovementBlendSpace;
            }
        }
        FCharacterData Data = GASCharacter->GetCharacterData();
        if(Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimationData.UnEquippedMovementBlendSpace;
        }
    }
    return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.UnEquippedMovementBlendSpace : nullptr;
}

UAnimSequenceBase *UPlayerAnimInstance::GetUnEquippedIdleAnimation() const
{
    if(GASCharacter)
    {
        if(const UItemStaticData* ItemData = GetItemEquippedData())
        {
            if(ItemData->CharacterAnimationData.UnEquippedIdleAnimation)
            {
                return ItemData->CharacterAnimationData.UnEquippedIdleAnimation;
            }
        }
        FCharacterData Data = GASCharacter->GetCharacterData();
        if(Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimationData.UnEquippedIdleAnimation;
        }
    }
    return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.UnEquippedIdleAnimation : nullptr;
}
