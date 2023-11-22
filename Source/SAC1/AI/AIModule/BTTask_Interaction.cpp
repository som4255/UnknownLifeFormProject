// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Interaction.h"
#include "../AIPawn.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTTask_Interaction::UBTTask_Interaction()
{
	NodeName = TEXT("Interaction");

	// TickTask �Լ��� ȣ���Ѵ�.
	bNotifyTick = true;

	// OnTaskFinished �Լ��� ȣ���Ѵ�.
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Interaction::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// BehaviorTreeComponent�� �̿��Ͽ� AIController�� ���� �� �ִ�.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn�� �ƴ� ��� ����.
	if (!IsValid(AIPawn))
		return EBTNodeResult::Failed;

	// AIController�� ������ �ִ� BlackboardComponent�� �̿��Ͽ� Target��
	// ���´�.
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

	// BehaviorTreeComponent�� �̿��Ͽ� AIController�� ���� �� �ִ�.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn�� �ƴ� ��� ����.
	if (!IsValid(AIPawn))
	{
		// Task�� �����Ų��.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// AIController�� ������ �ִ� BlackboardComponent�� �̿��Ͽ� Target��
	// ���´�.
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		// Task�� �����Ų��.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// ���� ����� ���������� �Ǵ��Ѵ�.
	if (AIPawn->GetInteractionEnd())
	{
		// Ÿ�ٰ� AIPawn���� �Ÿ��� ���Ѵ�.
		FVector	AILoc = AIPawn->GetActorLocation();
		FVector	TargetLoc = Target->GetActorLocation();

		AILoc.Z -= AIPawn->GetHalfHeight();
		TargetLoc.Z -= AIPawn->GetHalfHeight();

		float Distance = FVector::Distance(AILoc, TargetLoc);

		// �Ÿ����� �� ��ü���� ĸ�� �ݰ��� ���ش�.
		Distance -= AIPawn->GetCapsuleRadius();

		// Target�� RootComponent�� ���ͼ� Capsule���� Ȯ���Ѵ�.
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Target->GetRootComponent());

		if (IsValid(Capsule))
		{
			Distance -= Capsule->GetScaledCapsuleRadius();
		}

		if (Distance > AIPawn->GetAIState()->GetData()->AttackDistance)
		{
			// Task�� �����Ų��.
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		// ������ ���ݰŸ� �ȿ� ������ ��� Ÿ�������� ������ �����Ѵ�.
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



