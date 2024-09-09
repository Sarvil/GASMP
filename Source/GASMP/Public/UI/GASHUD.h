// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GASHUD.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API AGASHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AGASHUD();
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
    UTexture2D* CrosshairTexture;

    UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
    float CrosshairSize;

    UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
    FLinearColor CrosshairColor;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
    float CrosshairSpread;

    void UpdateCrosshairSpread(float NewSpread);
};
