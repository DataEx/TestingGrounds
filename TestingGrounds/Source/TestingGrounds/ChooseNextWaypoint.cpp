// Fill out your copyright notice in the Description page of Project Settings.

#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "PatrollingGuard.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();

	// Get Patrol Points
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	auto PatrolPoints = GetPatrolPoints(ControlledPawn);

	// Set Next Waypoint
	int32 Index = SetNextWaypoint(OwnerComp, PatrolPoints);

	// Cycle Index
	CycleIndex(Index, PatrolPoints, OwnerComp);

	UE_LOG(LogTemp, Warning, TEXT("Index: %i"), Index);
	return EBTNodeResult::Succeeded;
}

TArray<AActor*> UChooseNextWaypoint::GetPatrolPoints(APawn* ControlledPawn)
{
	auto PatrollingGuard = Cast<APatrollingGuard>(ControlledPawn);

	return PatrollingGuard->PatrolPointsCPP;
}

int32 UChooseNextWaypoint::SetNextWaypoint(UBehaviorTreeComponent& OwnerComp, TArray<AActor*> PatrolPoints)
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	int32 Index = BlackBoardComp->GetValueAsInt(IndexKey.SelectedKeyName);
	BlackBoardComp->SetValueAsObject(WayPointKey.SelectedKeyName, PatrolPoints[Index]);

	return Index;
}

void UChooseNextWaypoint::CycleIndex(int32 Index, TArray<AActor*> PatrolPoints, UBehaviorTreeComponent& OwnerComp)
{
	int32 NextIndex = (Index + 1) % PatrolPoints.Num();
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	BlackBoardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);
}
