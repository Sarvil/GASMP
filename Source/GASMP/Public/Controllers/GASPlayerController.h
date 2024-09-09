
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/GASFloatingStatusBarWidget.h"
#include "UI/GASHUDWidget.h"
#include "Components/WidgetComponent.h"
#include "GASPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASMP_API AGASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void CreateHUD();

	class UGASHUDWidget* GetHUD();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UGASHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASDocumentation|UI")
	class UGASHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};
