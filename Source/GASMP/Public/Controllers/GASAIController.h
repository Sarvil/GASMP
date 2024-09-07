// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GASAIController.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API AGASAIController : public AAIController
{
	GENERATED_BODY()

public:
    AGASAIController();

protected:
    virtual void BeginPlay() override;

    // Called when the AI perception component detects an actor
    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
    // Perception component to detect the player
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UAIPerceptionComponent* AIPerceptionComponent;

    // Blackboard key for storing the player reference
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName PlayerActorKeyName = "TargetActor";

    // The behavior tree asset that will be run
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    UPROPERTY(EditAnywhere, Category = "AI")
    UBehaviorTreeComponent* BehaviorTreeComponent;

    virtual void OnPossess(APawn* InPawn) override;
	
};
