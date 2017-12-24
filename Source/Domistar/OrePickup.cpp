// Fill out your copyright notice in the Description page of Project Settings.

#include "Domistar.h"
#include "OrePickup.h"

AOrePickup::AOrePickup()
{
	GetMesh()->SetSimulatePhysics(true);
}

void AOrePickup::WasCollected_Implementation(AActor* collector)
{
	Super::WasCollected_Implementation(collector);

	SetActorEnableCollision(false);
	GetMesh()->SetSimulatePhysics(false);
	
	Collector = collector;
}

void AOrePickup::Tick(float DeltaSeconds)
{
	if (!Collector)
		return;

	FVector directionToCollector = Collector->GetActorLocation() - GetActorLocation();
	
	if(directionToCollector.Size() < 10)
	{
		Destroy();
		return;
	}
	directionToCollector.Normalize();
	RootComponent->MoveComponent(directionToCollector * CollectSpeed * DeltaSeconds, GetActorRotation(), true);
	CollectSpeed += 100;
}
