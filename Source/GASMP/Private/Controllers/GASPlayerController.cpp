// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/GASPlayerController.h"
#include "PlayerStates/GASActorPlayerState.h"
#include "UI/GASHUD.h"

void AGASPlayerController::CreateHUD()
{
    // Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UGASHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / FMath::Max<float>(PS->GetMaxHealth(), 1.f));
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
    UIHUDWidget->SetManaPercentage(PS->GetMana() / FMath::Max<float>(PS->GetMaxMana(), 1.f));
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / FMath::Max<float>(PS->GetMaxStamina(), 1.f));
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
}
UGASHUDWidget *AGASPlayerController::GetHUD()
{
    return UIHUDWidget;
}

void AGASPlayerController::BeginPlay()
{
	Super::BeginPlay();

    // Set the HUD class to our custom one
    if (GetHUD())
    {
        AGASHUD* HUD = Cast<AGASHUD>(GetHUD());
        if (HUD)
        {
            // Initial setup for crosshair
            HUD->UpdateCrosshairSpread(0.0f);
        }
    }
}

void AGASPlayerController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AGASPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}
