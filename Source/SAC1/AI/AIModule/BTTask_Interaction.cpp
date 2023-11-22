// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Interaction.h"
#include "../AIPawn.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTTask_Interaction::UBTTask_Interaction()
{
	NodeName = TEXT("Interaction");

	// TickTask 함수를 호출한다.
	bNotifyTick = true;

	// OnTaskFinished 함수를 호출한다.
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Interaction::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// BehaviorTreeComponent를 이용하여 AIController를 얻어올 수 있다.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn이 아닐 경우 정지.
	if (!IsValid(AIPawn))
		return EBTNodeResult::Failed;

	// AIController가 가지고 있는 BlackboardComponent를 이용하여 Target을
	// 얻어온다.
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return EBTNodeResult::Failed;
	}

	AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Interaction1);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Interaction::AbortTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Type();
}

void UBTTask_Interaction::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// BehaviorTreeComponent를 이용하여 AIController를 얻어올 수 있다.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn이 아닐 경우 정지.
	if (!IsValid(AIPawn))
	{
		// Task를 종료시킨다.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// AIController가 가지고 있는 BlackboardComponent를 이용하여 Target을
	// 얻어온다.
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		// Task를 종료시킨다.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// 공격 모션이 끝났는지를 판단한다.
	if (AIPawn->GetInteractionEnd())
	{
		// 타겟과 AIPawn과의 거리를 구한다.
		FVector	AILoc = AIPawn->GetActorLocation();
		FVector	TargetLoc = Target->GetActorLocation();

		AILoc.Z -= AIPawn->GetHalfHeight();
		TargetLoc.Z -= AIPawn->GetHalfHeight();

		float Distance = FVector::Distance(AILoc, TargetLoc);

		// 거리에서 각 물체들의 캡슐 반경을 빼준다.
		Distance -= AIPawn->GetCapsuleRadius();

		// Target의 RootComponent를 얻어와서 Capsule인지 확인한다.
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Target->GetRootComponent());

		if (IsValid(Capsule))
		{
			Distance -= Capsule->GetScaledCapsuleRadius();
		}

		if (Distance > AIPawn->GetAIState()->GetData()->AttackDistance)
		{
			// Task를 종료시킨다.
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		// 여전히 공격거리 안에 존재할 경우 타겟쪽으로 방향을 변경한다.
		else
		{
			FVector Dir = TargetLoc - AILoc;
			Dir.Z = 0.0;

			Dir.Normalize();

			AIPawn->SetActorRotation(FRotator(0.0, Dir.Rotation().Yaw, 0.0));
		}

		AIPawn->SetInteractionEnd(false);
	}
}

void UBTTask_Interaction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::TickTask(OwnerComp, NodeMemory, TaskResult);
}



