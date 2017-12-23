// Fill out your copyright notice in the Description page of Project Settings.

#include "Domistar.h"
#include "OrePickup.h"

AOrePickup::AOrePickup()
{
	GetMesh()->SetSimulatePhysics(true);
}

void AOrePickup::WasCollected_Implementation()
{
	Super::WasCollected_Implementation();
	Destroy();
}
