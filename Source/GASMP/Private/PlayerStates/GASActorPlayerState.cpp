// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStates/GASActorPlayerState.h"
#include "Actors/BaseGASPlayer.h"
#include "Controllers/GASPlayerController.h"
#include "UI/GASFloatingStatusBarWidget.h"
#include "Components/InventoryComponent.h"
#include "UI/GASHUDWidget.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AGASActorPlayerState::AGASActorPlayerState()
{
	NetUpdateFrequency = 100.0f;
	AbilitySystemComponent = CreateDefaultSubobject<UActorGASComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	BaseAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("BaseAttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
}

UAbilitySystemComponent *AGASActorPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet *AGASActorPlayerState::GetBaseAttributeSet() const
{
	return BaseAttributeSet;
}

void AGASActorPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGASActorPlayerState, InventoryComponent);
}

float AGASActorPlayerState::GetHealth() const
{
	return BaseAttributeSet->GetHealth();
}

float AGASActorPlayerState::GetMaxHealth() const
{
	return BaseAttributeSet->GetMaxHealth();
}

float AGASActorPlayerState::GetHealthRegenRate() const
{
	return BaseAttributeSet->GetHealthRegenRate();
}

float AGASActorPlayerState::GetMana() const
{
	return BaseAttributeSet->GetMana();
}

float AGASActorPlayerState::GetMaxMana() const
{
	return BaseAttributeSet->GetMaxMana();
}

float AGASActorPlayerState::GetManaRegenRate() const
{
	return BaseAttributeSet->GetManaRegenRate();
}

float AGASActorPlayerState::GetStamina() const
{
	return BaseAttributeSet->GetStamina();
}

float AGASActorPlayerState::GetMaxStamina() const
{
	return BaseAttributeSet->GetMaxStamina();
}

float AGASActorPlayerState::GetStaminaRegenRate() const
{
	return BaseAttributeSet->GetStaminaRegenRate();
}

float AGASActorPlayerState::GetMoveSpeed() const
{
	return BaseAttributeSet->GetMoveSpeed();
}

UInventoryComponent *AGASActorPlayerState::GetInventoryComponent() const
{
	return InventoryComponent ? InventoryComponent : nullptr;
}

void AGASActorPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetHealthAttribute()).AddUObject(this, &AGASActorPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &AGASActorPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetHealthRegenRateAttribute()).AddUObject(this, &AGASActorPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetManaAttribute()).AddUObject(this, &AGASActorPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetMaxManaAttribute()).AddUObject(this, &AGASActorPlayerState::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetManaRegenRateAttribute()).AddUObject(this, &AGASActorPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetStaminaAttribute()).AddUObject(this, &AGASActorPlayerState::StaminaChanged);
		MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetMaxStaminaAttribute()).AddUObject(this, &AGASActorPlayerState::MaxStaminaChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetStaminaRegenRateAttribute()).AddUObject(this, &AGASActorPlayerState::StaminaRegenRateChanged);
		MoveSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &AGASActorPlayerState::MoveSpeedChanged);
	}
}

void AGASActorPlayerState::HealthChanged(const FOnAttributeChangeData &Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	ABaseGASPlayer *Hero = Cast<ABaseGASPlayer>(GetPawn());
	if (Hero)
	{
		UGASFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}
}

void AGASActorPlayerState::MaxHealthChanged(const FOnAttributeChangeData &Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	ABaseGASPlayer *Hero = Cast<ABaseGASPlayer>(GetPawn());
	if (Hero)
	{
		UGASFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}
	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxHealth(MaxHealth);
		}
	}
}

void AGASActorPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData &Data)
{
	float HealthRegenRate = Data.NewValue;

	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHealthRegenRate(HealthRegenRate);
		}
	}
}

void AGASActorPlayerState::ManaChanged(const FOnAttributeChangeData &Data)
{
	float Mana = Data.NewValue;

	// Update floating status bar
	ABaseGASPlayer *Hero = Cast<ABaseGASPlayer>(GetPawn());
	if (Hero)
	{
		UGASFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
		}
	}
}

void AGASActorPlayerState::MaxManaChanged(const FOnAttributeChangeData &Data)
{
	float MaxMana = Data.NewValue;

	// Update floating status bar
	ABaseGASPlayer *Hero = Cast<ABaseGASPlayer>(GetPawn());
	if (Hero)
	{
		UGASFloatingStatusBarWidget *HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
		}
	}

	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxMana(MaxMana);
		}
	}
}

void AGASActorPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData &Data)
{
	float ManaRegenRate = Data.NewValue;

	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetManaRegenRate(ManaRegenRate);
		}
	}
}

void AGASActorPlayerState::StaminaChanged(const FOnAttributeChangeData &Data)
{
	float Stamina = Data.NewValue;

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AGASActorPlayerState::MaxStaminaChanged(const FOnAttributeChangeData &Data)
{
	float MaxStamina = Data.NewValue;

	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxStamina(MaxStamina);
		}
	}
}

void AGASActorPlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData &Data)
{
	float StaminaRegenRate = Data.NewValue;

	// Update the HUD
	AGASPlayerController *PC = Cast<AGASPlayerController>(GetOwner());
	if (PC)
	{
		UGASHUDWidget *HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetStaminaRegenRate(StaminaRegenRate);
		}
	}
}
void AGASActorPlayerState::MoveSpeedChanged(const FOnAttributeChangeData &Data)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Activated"));
	}
	float MewMoveSpeed = Data.NewValue;
	ACharacter *OwningCharacter = Cast<ACharacter>(GetPawn());
	UCharacterMovementComponent *CharacterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
	if (CharacterMovement)
	{
		CharacterMovement->MaxWalkSpeed = MewMoveSpeed;
	}
}