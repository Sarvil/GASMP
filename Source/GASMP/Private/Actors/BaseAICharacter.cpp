// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseAICharacter.h"
#include "Components/ActorGASComponent.h"
#include "AttributeSets/BaseAttributeSet.h"


// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize the Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UActorGASComponent>(TEXT("AbilitySystemComponent"));

    // Ensure the component is replicated
    AbilitySystemComponent->SetIsReplicated(true);

    // Initialize the default attribute set (Optional, depending on your use case)
    BaseAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSetBase"));

    // Set the default ability level
    AbilityLevel = 1;

}

UAbilitySystemComponent *ABaseAICharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UBaseAttributeSet *ABaseAICharacter::GetBaseAttributeSet() const
{
    return BaseAttributeSet;
}

// Called when the game starts or when spawned
void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
    {
        // Initialize default attributes
        if (DefaultAttributeSet)
        {
            AbilitySystemComponent->InitStats(DefaultAttributeSet, nullptr);
        }

        // Grant default abilities
        for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
        {
            if (StartupAbility)
            {
                AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, AbilityLevel, 0));
            }
        }

        // Activate abilities and initialize actor info
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

// Called to bind functionality to input
void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

