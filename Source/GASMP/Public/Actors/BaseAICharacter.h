// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "BaseAICharacter.generated.h"

class UActorGASComponent;
class UBaseAttributeSet;

UCLASS()
class GASMP_API ABaseAICharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseAICharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UBaseAttributeSet* GetBaseAttributeSet() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UActorGASComponent* AbilitySystemComponent;

    // Default attribute set
    UPROPERTY()
    UBaseAttributeSet* BaseAttributeSet;

    // Default abilities to grant the AI
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    // Default attributes
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<UBaseAttributeSet> DefaultAttributeSet;

    // Optional: Level of abilities to grant
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    int32 AbilityLevel;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
