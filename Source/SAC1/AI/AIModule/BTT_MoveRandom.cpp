#include "BTT_MoveRandom.h"
#include "../DefaultAIController.h"
#include "NavigationSystem.h"
#include "../AIPawn.h"
#include "../AIState.h"
#include "../DefaultAIAnimInstance.h"

UBTT_MoveRandom::UBTT_MoveRandom()
{
	NodeName = TEXT("MoveRandom");
	bNotifyTick = true;
	//bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_MoveRandom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	FNavLocation location = {};
	AAIController* controller = OwnerComp.GetAIOwner();
	AAIPawn* pawn = Cast<AAIPawn>(controller->GetPawn());
	if (!IsValid(pawn))
	{
		return EBTNodeResult::Failed;
	}
	const FVector origin = pawn->GetActorLocation();
	const UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	float searchRadius = pawn->GetAIState()->GetData()->InteractionDistance;
	if (IsValid(navSystem) && navSystem->GetRandomPointInNavigableRadius(origin, searchRadius, location))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, location.Location);
		pawn->SetTargetLoc(location.Location);
	}
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTT_MoveRandom::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTT_MoveRandom::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* controller = OwnerComp.GetAIOwner();
	AAIPawn* pawn = Cast<AAIPawn>(controller->GetPawn());
	if (!IsValid(pawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		controller->StopMovement();
		return;
	}
	AActor* target = Cast<AActor>(controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (IsValid(target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		controller->StopMovement();
		pawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);
		return;
	}

	FVector dir = pawn->GetMovementComponent()->Velocity;
	dir.Z = 0.f;
	dir.Normalize();
	pawn->SetActorRotation(FRotator(0.0, dir.Rotation().Yaw, 0.0));

	FVector	AILoc = pawn->GetActorLocation();
	AILoc.Z = 0;
	FVector targetLoc = pawn->GetTargetLoc();
	targetLoc.Z = 0.;
	float dist = FVector::Distance(AILoc, targetLoc);
	dist -= pawn->GetCapsuleRadius();
	if (dist <=10.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		controller->StopMovement();
		pawn->GetAIAnimInstance()->ChangeAnim(EAIAnimType::Idle);
	}
}

void UBTT_MoveRandom::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
