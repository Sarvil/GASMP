// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GASMPStatics.h"

const UItemStaticData *UGASMPStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
    if(IsValid(ItemDataClass))
    {
        return GetDefault<UItemStaticData>(ItemDataClass);
    }
    return nullptr;
}