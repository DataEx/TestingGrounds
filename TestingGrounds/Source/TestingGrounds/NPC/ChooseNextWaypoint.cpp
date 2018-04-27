// Fill out your copyright notice in the Description page of Project Settings.

#include "ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PatrolRoute.h"
#include "AIController.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerComp, NodeMemory);



	// Get Patrol Route
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	auto PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolRoute>();
	if (!ensure(PatrolRoute)) {	return EBTNodeResult::Failed; }

	auto PatrolPoints = PatrolRoute->GetPatrolPoints();
	if (PatrolPoints.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("A guard is missing Patrol Points"));
		return EBTNodeResult::Failed;
	}

	// Set Next Waypoint
	int32 Index = SetNextWaypoint(OwnerComp, PatrolPoints);

	// Cycle Index
	CycleIndex(Index, PatrolPoints, OwnerComp);

	return EBTNodeResult::Succeeded;
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
