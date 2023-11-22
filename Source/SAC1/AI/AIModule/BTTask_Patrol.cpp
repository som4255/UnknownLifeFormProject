#include "BTTask_Patrol.h"
#include "../AIPawn.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");

	// TickTask �Լ��� ȣ���Ѵ�.
	bNotifyTick = true;

	// OnTaskFinished �Լ��� ȣ���Ѵ�.
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(
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

	if (IsValid(Target))
	{
		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return EBTNodeResult::Failed;
	}

	// Ÿ���� ��ġ�� NavMesh�� Ȱ���Ͽ� ���� ã�� �̵���Ų��.
	FVector	PatrolLoc = AIPawn->GetPatrolLocation();

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, PatrolLoc);

	AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Walk);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Patrol::AbortTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Type();
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp,
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

	if (IsValid(Target))
	{
		// Task�� �����Ų��.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// ���� �̵� ������ ���ϵ��� AI�� ȸ����Ų��.
	FVector Dir = AIPawn->GetMovementComponent()->Velocity;
	Dir.Z = 0.f;

	// Dir�� ���� ũ����� ������ �ִ�. �׷��Ƿ� ���⸸�� ����� ���ؼ� ���͸�
	// ����ȭ �����ش�.
	Dir.Normalize();

	AIPawn->SetActorRotation(FRotator(0.0, Dir.Rotation().Yaw, 0.0));

	// Ÿ�ٰ� AIPawn���� �Ÿ��� ���Ѵ�.
	FVector	AILoc = AIPawn->GetActorLocation();
	FVector	TargetLoc = AIPawn->GetPatrolLocation();

	AILoc.Z -= AIPawn->GetHalfHeight();

	float Distance = FVector::Distance(AILoc, TargetLoc);

	// �Ÿ����� �� ��ü���� ĸ�� �ݰ��� ���ش�.
	Distance -= AIPawn->GetCapsuleRadius();

	if (Distance <= 10.f)
	{
		// Task�� �����Ų��.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		AIPawn->NextPatrol();

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);
	}
}

void UBTTask_Patrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::TickTask(OwnerComp, NodeMemory, TaskResult);
}



