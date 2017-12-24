// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/GameModeBase.h"
#include "DomistarGameMode.generated.h"


UCLASS(minimalapi)
class ADomistarGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADomistarGameMode();
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Power", Meta = (BlueprintProtected="true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
};



