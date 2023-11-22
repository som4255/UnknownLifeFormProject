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

	// BehaviorTreeComponent를 이용하여 AIController를 얻어올 수 있다.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn이 아닐 경우 정지.
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

	// AIController가 가지고 있는 BlackboardComponent를 이용하여 Target을
	// 얻어온다.
	AActor* Target = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
		return false;

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

	LOG(TEXT("CheckDist : %.5f Dist : %.5f"), TypeDistance, Distance);

	return Distance <= TypeDistance;
}
