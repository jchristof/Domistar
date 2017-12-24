// Fill out your copyright notice in the Description page of Project Settings.

#include "Domistar.h"
#include "DomistarMissle.h"


// Sets default values
ADomistarMissle::ADomistarMissle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ADomistarMissle::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;
}

// Called when the game starts or when spawned
void ADomistarMissle::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ADomistarMissle::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector directionToEnemy = FVector(0.,0.,220.) - GetActorLocation();

	if (directionToEnemy.Size() < 10)
	{
		Destroy();
		return;
	}
	directionToEnemy.Normalize();
	RootComponent->MoveComponent(directionToEnemy * MissleSpeed * DeltaTime, GetActorRotation(), true);
	MissleSpeed = MissleSpeed > 2000 ? MissleSpeed : MissleSpeed + 100;
}

void ADomistarMissle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	Destroy();
}

