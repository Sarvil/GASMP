// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/GASAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/BaseGASCharacter.h"


AGASAIController::AGASAIController()
{
    // Initialize the perception component and bind to the OnPerceptionUpdated function
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGASAIController::OnPerceptionUpdated);

    // Configure the sight sense for the perception component
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.f;
    SightConfig->LoseSightRadius = 2200.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AGASAIController::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the AI controller starts with the assigned behavior tree
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AGASAIController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    if (BlackboardComponent)
    {
        BlackboardComponent->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);
    }
}

void AGASAIController::OnPerceptionUpdated(AActor *Actor, FAIStimulus Stimulus)
{
    // Check if the perceived actor is the player
    if (Actor && Stimulus.WasSuccessfullySensed())
    {
        // Get the blackboard component and set the TargetActor key to the player actor
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsObject(PlayerActorKeyName, Actor);
        }
    }
    else
    {
        // Clear the TargetActor key if the player is no longer sensed
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->ClearValue(PlayerActorKeyName);
        }
    }
}
