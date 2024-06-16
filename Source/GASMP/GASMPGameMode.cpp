// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASMPGameMode.h"
#include "GASMPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGASMPGameMode::AGASMPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
