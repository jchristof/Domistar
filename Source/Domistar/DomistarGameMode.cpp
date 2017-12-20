// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Domistar.h"
#include "DomistarGameMode.h"
#include "DomistarPawn.h"

ADomistarGameMode::ADomistarGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ADomistarPawn::StaticClass();
}

