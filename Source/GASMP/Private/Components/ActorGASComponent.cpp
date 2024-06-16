// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ActorGASComponent.h"

void UActorGASComponent::ReceiveDamage(UActorGASComponent *SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
    ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}