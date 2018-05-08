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

void ATile::PlaceActors(int MinSpawn, int MaxSpawn)
{

	if (SpawnableActors.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("SpawnableActors is empty"));
		return;
	}


	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

	FVector MinPoint(0, -2000, 0);
	FVector MaxPoint(4000, 2000, 0);
	FBox Bounds(MinPoint, MaxPoint);
	for (int i = 0; i < NumberToSpawn; i++) {
		
		// Generate spawn location
		FVector SpawnPoint = FMath::RandPointInBox(Bounds);

		// Determine which actor to spawn
		int Index = FMath::RandRange(0, SpawnableActors.Num() - 1);
		TSubclassOf<AActor> ToSpawn = SpawnableActors[Index];
		if (ToSpawn == nullptr) { 
			UE_LOG(LogTemp, Warning, TEXT("No valid actor at SpawnableActors index: %d"), Index);
			continue;
		}

		// Spawn actor in generated position
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		Spawned->SetActorRelativeLocation(SpawnPoint);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	CastSphere(GetActorLocation(), 300);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CastSphere(FVector Location, float Radius)
{
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Location,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);

	FColor DebugColor = HasHit ? FColor(255, 0, 0) : FColor(0, 255, 0);

	DrawDebugCapsule(
		GetWorld(),
		Location,
		0,
		Radius,
		FQuat::Identity,
		DebugColor,
		true,
		100
	);


	return HasHit;
}

