// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GrassComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TESTINGGROUNDS_API UGrassComponent : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	UGrassComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FBox SpawningExtents;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	int Spawncount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnGrass();

};
