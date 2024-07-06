// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTasks/AttributeChangeAsyncAction.h"

UAttributeChangeAsyncAction *UAttributeChangeAsyncAction::ListenForAttributeChange(UAbilitySystemComponent *AbilitySystemComponent, FGameplayAttribute Attribute)
{
    UAttributeChangeAsyncAction* WaitForAttributeChangedTask = NewObject<UAttributeChangeAsyncAction>();
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributeToListenFor = Attribute;

	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		WaitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangedTask, &UAttributeChangeAsyncAction::AttributeChanged);

	return WaitForAttributeChangedTask;
}

UAttributeChangeAsyncAction *UAttributeChangeAsyncAction::ListenForAttributesChange(UAbilitySystemComponent *AbilitySystemComponent, TArray<FGameplayAttribute> Attributes)
{
    UAttributeChangeAsyncAction* WaitForAttributeChangedTask = NewObject<UAttributeChangeAsyncAction>();
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributesToListenFor = Attributes;

	if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
	{
		WaitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}

	for (FGameplayAttribute Attribute : Attributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangedTask, &UAttributeChangeAsyncAction::AttributeChanged);
	}

	return WaitForAttributeChangedTask;
}

void UAttributeChangeAsyncAction::EndTask()
{
    if (IsValid(ASC))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (FGameplayAttribute Attribute : AttributesToListenFor)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAttributeChangeAsyncAction::AttributeChanged(const FOnAttributeChangeData &Data)
{
    OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}
