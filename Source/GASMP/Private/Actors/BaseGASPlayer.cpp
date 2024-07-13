// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseGASPlayer.h"
#include "PlayerStates/GASActorPlayerState.h"
#include "Components/ActorGASComponent.h"
#include "Controllers/GASPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AttributeSets/BaseAttributeSet.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/GASCharacterMovementComponent.h"

ABaseGASPlayer::ABaseGASPlayer(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 80.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	/* UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/UI/UI_FloatingStatusBar_Hero.UI_FloatingStatusBar_Hero_C"));
	if (!UIFloatingStatusBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	} */
}

void ABaseGASPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	//BindASCInput();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseGASPlayer::JumpPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseGASPlayer::JumpReleased);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseGASPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseGASPlayer::Look);

		if(Ability1Action)
		{
			EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Started, this, &ABaseGASPlayer::Ability1Pressed);
			EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Completed, this, &ABaseGASPlayer::Ability1Released);
		}
		if(DropItemAction)
		{
			EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &ABaseGASPlayer::DropItemPressed);
		}
		if(EquipNextAction)
		{
			EnhancedInputComponent->BindAction(EquipNextAction, ETriggerEvent::Started, this, &ABaseGASPlayer::EquipNextPressed);
		}
		if(UnEquipAction)
		{
			EnhancedInputComponent->BindAction(UnEquipAction, ETriggerEvent::Started, this, &ABaseGASPlayer::UnEquipPressed);
		}
		if(AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABaseGASPlayer::AttackPressed);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ABaseGASPlayer::AttackReleased);
		}
	}
}

void ABaseGASPlayer::BeginPlay()
{
	Super::BeginPlay();
	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	InitializeFloatingStatusBar();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();
}

void ABaseGASPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseGASPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseGASPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABaseGASPlayer::JumpPressed()
{
	if(!GASCharacterMovementComponent->TryTraversal(AbilitySystemComponent.Get()))
	{
		SendLocalInputToASC(true, EGASAbilityInputID::Jump);
	}
}

void ABaseGASPlayer::JumpReleased()
{
	SendLocalInputToASC(false, EGASAbilityInputID::Jump);
}

void ABaseGASPlayer::Ability1Pressed()
{
	SendLocalInputToASC(true, EGASAbilityInputID::Ability1);
}

void ABaseGASPlayer::Ability1Released()
{
	SendLocalInputToASC(false, EGASAbilityInputID::Ability1);
}

void ABaseGASPlayer::DropItemPressed(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::DropItemTag;
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if(PS)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PS, UInventoryComponent::DropItemTag, EventPayload);
	}
}

void ABaseGASPlayer::EquipNextPressed(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::EquipNextItemTag;
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if(PS)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PS, UInventoryComponent::EquipNextItemTag, EventPayload);
	}
	
}

void ABaseGASPlayer::UnEquipPressed(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::UnEquipTag;
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if(PS)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnEquipTag, EventPayload);
	}
}

void ABaseGASPlayer::AttackPressed(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackStartedEventTag;
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if(PS)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartedEventTag, EventPayload);
	}
}

void ABaseGASPlayer::AttackReleased(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackEndedEventTag;
	AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
	if(PS)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndedEventTag, EventPayload);
	}
}

void ABaseGASPlayer::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
    if(PS){
        AbilitySystemComponent = Cast<UActorGASComponent>(PS->GetAbilitySystemComponent());
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		// Set the BaseAttributeSet for convenience attribute functions
		BaseAttributeSet = PS->GetBaseAttributeSet();


		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());

		// End respawn specific things


		AddStartupEffects();

		AddCharacterAbilities();

		AGASPlayerController* PC = Cast<AGASPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		InitializeFloatingStatusBar();
    }
}

UGASFloatingStatusBarWidget *ABaseGASPlayer::GetFloatingStatusBar()
{
    return UIFloatingStatusBar;;
}

void ABaseGASPlayer::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AGASActorPlayerState* PS = GetPlayerState<AGASActorPlayerState>();
    if(PS){
        AbilitySystemComponent = Cast<UActorGASComponent>(PS->GetAbilitySystemComponent());
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		//BindASCInput();

		// Set the BaseAttributeSet for convenience attribute functions
		BaseAttributeSet = PS->GetBaseAttributeSet();

		AGASPlayerController* PC = Cast<AGASPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		//AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
    }
}

void ABaseGASPlayer::InitializeFloatingStatusBar()
{
    // Only create once
	if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AGASPlayerController* PC = Cast<AGASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (UIFloatingStatusBarClass)
		{
			UIFloatingStatusBar = CreateWidget<UGASFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
				UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
			}
		}
	}
}

void ABaseGASPlayer::SendLocalInputToASC(bool bIsPressed, const EGASAbilityInputID AbilityInputID)
{
	if(bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void ABaseGASPlayer::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/GASMP"), FName("EGASAbilityInputID"));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), AbilityEnumAssetPath, static_cast<int32>(EGASAbilityInputID::Confirm), static_cast<int32>(EGASAbilityInputID::Cancel)));
		ASCInputBound = true;
	}
}