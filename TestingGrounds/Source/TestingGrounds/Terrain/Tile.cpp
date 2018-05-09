// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius = 500.0f, float MinScale = 1.0f, float MaxScale = 1.0f)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

	for (int i = 0; i < NumberToSpawn; i++) {
		float ActorScale = FMath::RandRange(MinScale, MaxScale);

		// Generate spawn location
		FVector SpawnPoint;
		bool CanSpawn = FindEmptyLocation(SpawnPoint, Radius * ActorScale);
		if (!CanSpawn) { continue; }

		// Spawn actor in generated transform
		float RandRotation = FMath::RandRange(-180.0f, 180.0f);

		PlaceActor(ToSpawn, SpawnPoint, RandRotation, ActorScale);
	}
}

bool ATile::FindEmptyLocation(FVector & SpawnPoint, float Radius)
{
	FVector MinPoint(0, -2000, 0);
	FVector MaxPoint(4000, 2000, 0);
	FBox Bounds(MinPoint, MaxPoint);
	const int NUM_ATTEMPTS = 100;
	
	for (int i = 0; i < NUM_ATTEMPTS; i++) {
		FVector SpawnPointCandidate = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(SpawnPointCandidate, Radius)) {
			SpawnPoint = SpawnPointCandidate;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	Spawned->SetActorRelativeLocation(SpawnPoint);
	Spawned->SetActorRotation(FRotator(0, Rotation, 0));
	Spawned->SetActorScale3D(FVector(Scale));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);

	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);

	return !HasHit;
}


