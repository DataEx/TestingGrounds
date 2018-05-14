// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "WorldCollision.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "AI/Navigation/NavigationSystem.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);

	NavigationBoundsOffset = FVector(2000, 0, 0);
}

template<class T>
void ATile::RandomlyGenerateActors(TSubclassOf<T> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale) {
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool FoundLocation = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (!FoundLocation) { continue; }
		SpawnPosition.Rotation = FMath::RandRange(-180.0f, 180.0f);
		PlaceActor(ToSpawn, SpawnPosition);
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius = 500.0f, float MinScale = 1.0f, float MaxScale = 1.0f)
{
	RandomlyGenerateActors(ToSpawn, MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int MinSpawn, int MaxSpawn, float Radius)
{
	RandomlyGenerateActors(ToSpawn, MinSpawn, MaxSpawn, Radius, 1, 1);
}

TArray<FSpawnPosition> ATile::GenerateSpawnPositions(int Count, int MinScale, int MaxScale, int Radius)
{
	TArray<FSpawnPosition> SpawnPositions = TArray<FSpawnPosition>();
	for (size_t i = 0; i < Count; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);

		// Generate spawn location
		bool CanSpawn = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (!CanSpawn) { continue; }

		SpawnPosition.Rotation = FMath::RandRange(-180.0f, 180.0f);
		SpawnPositions.Add(SpawnPosition);
	}

	return SpawnPositions;
}

bool ATile::FindEmptyLocation(FVector& SpawnPoint, float Radius)
{
	FBox Bounds(MinExtent, MaxExtent);
	const int NUM_ATTEMPTS = 20;
	
	for (int i = 0; i < NUM_ATTEMPTS; i++) {
		FVector SpawnPointCandidate = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(SpawnPointCandidate, Radius)) {
			SpawnPoint = SpawnPointCandidate;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (!Spawned) { return; }
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn);
	if (!Spawned) { return; }
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
	Spawned->SpawnDefaultController();
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (!Pool || !NavMeshBoundsVolume) { return; }
	Pool->Return(NavMeshBoundsVolume);
}


// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::SetPool(UActorPool * InPool)
{
	Pool = InPool;
	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr) { return; }

	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();
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


