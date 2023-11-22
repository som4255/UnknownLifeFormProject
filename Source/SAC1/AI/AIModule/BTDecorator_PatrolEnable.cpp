#include "BTDecorator_PatrolEnable.h"
#include "../AIPawn.h"
#include "../AIState.h"
#include "../../SAC1Character.h"
#include "../DefaultAIController.h"

UBTDecorator_PatrolEnable::UBTDecorator_PatrolEnable()
{
}

bool UBTDecorator_PatrolEnable::CalculateRawConditionValue(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// BehaviorTreeComponent를 이용하여 AIController를 얻어올 수 있다.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn이 아닐 경우 정지.
	if (!IsValid(AIPawn))
		return false;

	// AIController가 가지고 있는 BlackboardComponent를 이용하여 Target을
	// 얻어온다.
	AActor* CheckTarget = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (IsValid(CheckTarget))
		return false;

	return AIPawn->GetPatrolEnable();
}




