#include "BTTask_Patrol.h"
#include "../AIPawn.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");

	// TickTask 함수를 호출한다.
	bNotifyTick = true;

	// OnTaskFinished 함수를 호출한다.
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(
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

	if (IsValid(Target))
	{
		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return EBTNodeResult::Failed;
	}

	// 타겟의 위치로 NavMesh를 활용하여 길을 찾아 이동시킨다.
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

	if (IsValid(Target))
	{
		// Task를 종료시킨다.
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Controller->StopMovement();

		AIPawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);

		return;
	}

	// 현재 이동 방향을 향하도록 AI를 회전시킨다.
	FVector Dir = AIPawn->GetMovementComponent()->Velocity;
	Dir.Z = 0.f;

	// Dir은 현재 크기까지 가지고 있다. 그러므로 방향만을 남기기 위해서 벡터를
	// 정규화 시켜준다.
	Dir.Normalize();

	AIPawn->SetActorRotation(FRotator(0.0, Dir.Rotation().Yaw, 0.0));

	// 타겟과 AIPawn과의 거리를 구한다.
	FVector	AILoc = AIPawn->GetActorLocation();
	FVector	TargetLoc = AIPawn->GetPatrolLocation();

	AILoc.Z -= AIPawn->GetHalfHeight();

	float Distance = FVector::Distance(AILoc, TargetLoc);

	// 거리에서 각 물체들의 캡슐 반경을 빼준다.
	Distance -= AIPawn->GetCapsuleRadius();

	if (Distance <= 10.f)
	{
		// Task를 종료시킨다.
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



