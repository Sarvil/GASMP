// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GASHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"


AGASHUD::AGASHUD()
{
    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/UI/CrosshairTexture"));
    CrosshairTexture = CrosshairTexObj.Object;

    // Set default values
    CrosshairSize = 32.0f;
    CrosshairColor = FLinearColor::White;
    CrosshairSpread = 0.0f;
}

void AGASHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrosshairTexture)
    {
        // Calculate the center of the screen
        const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        // Calculate crosshair position with spread
        const FVector2D CrosshairDrawPosition(
            Center.X - (CrosshairSize * 0.5f) + CrosshairSpread,
            Center.Y - (CrosshairSize * 0.5f) + CrosshairSpread
        );

        // Get the texture resource
        FTextureResource* TextureResource = CrosshairTexture->GetResource();
        if (TextureResource)
        {
            // Draw the crosshair
            FCanvasTileItem TileItem(CrosshairDrawPosition, TextureResource, FVector2D(CrosshairSize, CrosshairSize), CrosshairColor);
            TileItem.BlendMode = SE_BLEND_Translucent;
            Canvas->DrawItem(TileItem);
        }
    }
}

void AGASHUD::UpdateCrosshairSpread(float NewSpread)
{
    CrosshairSpread = NewSpread;
}