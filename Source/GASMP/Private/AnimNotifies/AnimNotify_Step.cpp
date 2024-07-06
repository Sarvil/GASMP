// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_Step.h"
#include "Actors/BaseGASCharacter.h"
#include "Components/FootstepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    Super::Notify(MeshComp, Animation);

    check(MeshComp);

    ABaseGASCharacter* GASCharacter = MeshComp ? Cast<ABaseGASCharacter>(MeshComp->GetOwner()) : nullptr;
    if(GASCharacter)
    {
        if(UFootstepsComponent* FootstepsComponent = GASCharacter->GetFootstepsComponent())
        {
            FootstepsComponent->HandleFootstep(Foot);
        }
    }
}