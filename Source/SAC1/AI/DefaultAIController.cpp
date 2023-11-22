// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAIController.h"

ADefaultAIController::ADefaultAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>	AITree(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/BT_DefaultAI.BT_DefaultAI'"));

	if (AITree.Succeeded())
		mAITree = AITree.Object;

	static ConstructorHelpers::FObjectFinder<UBlackboardData>	AIBlackboard(TEXT("/Script/AIModule.BlackboardData'/Game/AI/BB_AIDefault.BB_AIDefault'"));

	if (AIBlackboard.Succeeded())
		mAIBlackboard = AIBlackboard.Object;
}

void ADefaultAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(mAITree) && IsValid(mAIBlackboard))
	{
		// AIController가 가지고 있는 BlackboardComponent에 Blackboard애셋을
		// 사용하라고 지정한다.
		UBlackboardComponent* BlackboardRef = Blackboard;
		if (UseBlackboard(mAIBlackboard, BlackboardRef))
		{
			// BehaviorTree를 동작시킨다.
			RunBehaviorTree(mAITree);
		}
	}
}

void ADefaultAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
