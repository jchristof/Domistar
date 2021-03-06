// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Domistar.h"
#include "DomistarPawn.h"
#include "DomistarProjectile.h"
#include "TimerManager.h"
#include "Pickup.h"
#include "KismetDebugUtilities.h"
#include "DomistarMissle.h"

const FName ADomistarPawn::MoveForwardBinding("MoveForward");
const FName ADomistarPawn::MoveRightBinding("MoveRight");
const FName ADomistarPawn::FireForwardBinding("FireForward");
const FName ADomistarPawn::FireRightBinding("FireRight");

void ADomistarPawn::CollectPickups()
{
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	for(int32 iColleted = 0; iColleted < CollectedActors.Num(); ++iColleted)
	{
		APickup* const TestPickup = Cast<APickup>(CollectedActors[iColleted]);
		if(!TestPickup || TestPickup->IsPendingKill() || !TestPickup->IsActive())
			continue;

		TestPickup->WasCollected(this);
		TestPickup->SetActive(false);
		++MissileCount;
	}
}

ADomistarPawn::ADomistarPawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	ShipMeshComponent->OnComponentHit.AddDynamic(this, &ADomistarPawn::OnHit);
	ShipMeshComponent->SetNotifyRigidBodyCollision(true);

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	CollideDrag = 0.9f;
	DriftDrag = 0.999f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;

	Drift = FVector(0., 0., 0.);
}

void ADomistarPawn::ReleaseMissle()
{
	UWorld* const World = GetWorld();
	if (World == NULL || MissileCount <= 0)
		return;

	// spawn the projectile
	ADomistarMissle* const shot = World->SpawnActor<ADomistarMissle>(GetActorLocation(), FRotator());
	shot->SetOwner(this);
	shot->SetActorEnableCollision(false);
	shot->DisableComponentsSimulatePhysics();
	--MissileCount;
	
}

void ADomistarPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction("Missle", IE_Pressed, this, &ADomistarPawn::ReleaseMissle);
}

void ADomistarPawn::Tick(float DeltaSeconds)
{
	CollectPickups();
	
//	DrawDebugSphere(
//		GetWorld(),
//		CollectionSphere->GetComponentLocation(),
//		CollectionSphere->GetScaledSphereRadius(),
//		32,
//		FColor(255, 0, 0)
//	);

	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	//make stick input deliberate before it impacts direction
	const FVector InputDirection = FVector(ForwardValue, RightValue, 0.f);
	if (InputDirection.Size() > 0.5f)
	{
		// Scale the directional input to a length of 1.0 for any input magnitude
		const FVector MoveDirection = InputDirection.GetClampedToSize(1.0f, 1.0f);

		// Calculate  movement
		const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

		// If non-zero size, move this actor
		if (Movement.SizeSquared() > 0.0f)
		{
			Move(Movement, DeltaSeconds);

			//this movement becomes the drift
			Drift.X = Movement.X;
			Drift.Y = Movement.Y;
			Drift.Z = Movement.Z;
		}
	}
	else
	{
		Move(Drift, DeltaSeconds);
		Drift = Drift * DriftDrag;
	}
	
	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	FireShot(FireDirection);
}

void ADomistarPawn::Move(FVector movement, float DeltaSeconds)
{
	FHitResult Hit(1.f);
	const FRotator NewRotation = FMath::RInterpTo(RootComponent->GetComponentRotation(), movement.Rotation(), DeltaSeconds, 10.f);
	RootComponent->MoveComponent(movement, NewRotation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
		//reflective bounce
		//R = -2*(V dot N)*N + V
		Drift = CollideDrag  * -2 * FVector::DotProduct(movement, Normal2D) * Normal2D + movement;
	}
}

void ADomistarPawn::FireShot(FVector FireDirection)
{
	// If we it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				ADomistarProjectile* const shot = World->SpawnActor<ADomistarProjectile>(SpawnLocation, FireRotation);
				shot->SetOwner(this);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ADomistarPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void ADomistarPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		auto velocity = GetVelocity();
		OtherComp->AddImpulseAtLocation(FVector(2000.,2000.,2000.), OtherActor->GetActorLocation());
		//OtherActor->Destroy();
	}
}

void ADomistarPawn::ShotTimerExpired()
{
	bCanFire = true;
}

