// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseGASCharacter.h"
#include "Components/ActorGASComponent.h"
#include "AttributeSets/BaseAttributeSet.h"
#include "Abilities/BaseGameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "Components/GASCharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Components/FootstepsComponent.h"

// Sets default values
ABaseGASCharacter::ABaseGASCharacter(const class FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGASCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsComponent"));
}

void ABaseGASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

void ABaseGASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGASCharacter, CharacterData);
} 

// Called when the game starts or when spawned
void ABaseGASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGASCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UBaseGameplayAbility>& StartupAbility : CharacterData.Abilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ABaseGASCharacter::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : CharacterData.Effects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void ABaseGASCharacter::SetHealth(float Health)
{
	if (BaseAttributeSet.IsValid())
	{
		BaseAttributeSet->SetHealth(Health);
	}
}

void ABaseGASCharacter::SetMana(float Mana)
{
	if (BaseAttributeSet.IsValid())
	{
		BaseAttributeSet->SetMana(Mana);
	}
}

void ABaseGASCharacter::SetStamina(float Stamina)
{
	if (BaseAttributeSet.IsValid())
	{
		BaseAttributeSet->SetStamina(Stamina);
	}
}

void ABaseGASCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void ABaseGASCharacter::InitFromCharacterData(const FCharacterData &InCharacterData, bool bFromReplication)
{
}

UAbilitySystemComponent *ABaseGASCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent.Get();
}

float ABaseGASCharacter::GetHealth() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetHealth();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMaxHealth() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMaxHealth();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMana() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMana();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMaxMana() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMaxMana();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetStamina() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetStamina();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMaxStamina() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMaxStamina();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMoveSpeed() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMoveSpeed();
	}

	return 0.0f;
}

float ABaseGASCharacter::GetMoveSpeedBaseValue() const
{
    if (BaseAttributeSet.IsValid())
	{
		return BaseAttributeSet->GetMoveSpeedAttribute().GetGameplayAttributeData(BaseAttributeSet.Get())->GetBaseValue();
	}

	return 0.0f;
}

void ABaseGASCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterData.Abilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

FCharacterData ABaseGASCharacter::GetCharacterData() const
{
    return CharacterData;
}

void ABaseGASCharacter::SetCharacterData(const FCharacterData &InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

UFootstepsComponent *ABaseGASCharacter::GetFootstepsComponent() const
{
    return FootstepsComponent;
}
