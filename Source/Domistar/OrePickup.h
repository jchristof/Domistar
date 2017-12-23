// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "OrePickup.generated.h"

/**
 * 
 */
UCLASS()
class DOMISTAR_API AOrePickup : public APickup
{
	GENERATED_BODY()
	
public:
	AOrePickup();
	void WasCollected_Implementation() override;
	
};
