#include "BTDecorator_CheckTargetDistance.h"
#include "../AIPawn.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTDecorator_CheckTargetDistance::UBTDecorator_CheckTargetDistance()
{
	mCheckType = ECheckDistanceType::Interaction;
}

bool UBTDecorator_CheckTargetDistance::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// BehaviorTreeComponent�� �̿��Ͽ� AIController�� ���� �� �ִ�.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn�� �ƴ� ��� ����.
	if (!IsValid(AIPawn))
		return false;

	float	TypeDistance = 0.f;

	switch (mCheckType)
	{
	case ECheckDistanceType::Attack:
		TypeDistance = AIPawn->GetAIState()->GetData()->AttackDistance;
		break;
	case ECheckDistanceType::Interaction:
		TypeDistance = AIPawn->GetAIState()->GetData()->InteractionDistance;
		break;
	}

	// AIController�� ������ �ִ� BlackboardComponent�� �̿��Ͽ� Target��
	// ���´�.
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
		return false;

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

	LOG(TEXT("CheckDist : %.5f Dist : %.5f"), TypeDistance, Distance);

	return Distance <= TypeDistance;
}
