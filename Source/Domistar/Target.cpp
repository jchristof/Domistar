// Fill out your copyright notice in the Description page of Project Settings.

#include "Domistar.h"
#include "Target.h"
#include "DomistarProjectile.h"


// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Geometry/Meshes/1M_Cube_Chamfer"));

	// Create mesh component for the projectile sphere
	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetCube"));
	TargetMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	TargetMesh->SetupAttachment(RootComponent);
	TargetMesh->SetSimulatePhysics(true);
	TargetMesh->SetNotifyRigidBodyCollision(true);
	TargetMesh->OnComponentHit.AddDynamic(this, &ATarget::OnHit);		// set up a notification for when this component hits something
	RootComponent = TargetMesh;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("/Game/StarterContent/Particles/P_Smoke"));
	PSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitParticle"));
	PSC->SetTemplate(PS.Object);
	PSC->ActivateSystem(true);
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATarget::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATarget::Hit(FVector NormalImpulse, const FHitResult& Hit)
{

	//UGameplayStatics::SpawnEmitterAttached(YourParticleSystem, Component, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false);
	Destroy();
	
}

void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics() && Cast<ADomistarProjectile>(OtherActor) != nullptr)
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		Destroy();
		
	}
}


