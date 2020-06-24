// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PTRPGGameMode.h"
#include "PTRPGHUD.h"
#include "PTRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

APTRPGGameMode::APTRPGGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = APTRPGHUD::StaticClass();
}
