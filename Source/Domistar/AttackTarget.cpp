// Fill out your copyright notice in the Description page of Project Settings.

#include "Domistar.h"
#include "AttackTarget.h"
#include "DomistarProjectile.h"


// Sets default values
AAttackTarget::AAttackTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Geometry/Meshes/1M_Cube_Chamfer"));

	// Create mesh component for the projectile sphere
	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetCube"));
	TargetMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	TargetMesh->SetupAttachment(RootComponent);
	TargetMesh->SetSimulatePhysics(true);
	TargetMesh->SetNotifyRigidBodyCollision(true);
	TargetMesh->OnComponentHit.AddDynamic(this, &AAttackTarget::OnHit);		// set up a notification for when this component hits something
	RootComponent = TargetMesh;

}

// Called when the game starts or when spawned
void AAttackTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAttackTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TargetMesh->AddImpulseAtLocation(FVector(100., 100., 100.), FVector(0., 0., 0.));

}

void AAttackTarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if(OtherActor->IsA(AEnemy::StaticClass())
	//if(OtherActor->StaticClass() == AEnemy::StaticClass())
	//if(OtherActor->GetClass()->IsChildOf(AEnemy::StaticClass()))

	// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics() && Cast<ADomistarProjectile>(OtherActor) != nullptr)
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		Destroy();

	}
}

