// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Domistar.h"
#include "DomistarGameMode.h"
#include "DomistarPawn.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


ADomistarGameMode::ADomistarGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ADomistarPawn::StaticClass();
}

void ADomistarGameMode::BeginPlay()
{
	Super::BeginPlay();

	ADomistarPawn* pawn = Cast<ADomistarPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if(pawn)
	{
		
	}

	if(HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if(CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

