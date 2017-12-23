// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DomistarMissle.generated.h"

UCLASS()
class DOMISTAR_API ADomistarMissle : public AActor
{
	GENERATED_BODY()
	
	UStaticMeshComponent* ProjectileMesh;
public:	
	// Sets default values for this actor's properties
	ADomistarMissle();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	int MissleSpeed = 0;
};
