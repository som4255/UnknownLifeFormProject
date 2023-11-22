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

	// BehaviorTreeComponent�� �̿��Ͽ� AIController�� ���� �� �ִ�.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn�� �ƴ� ��� ����.
	if (!IsValid(AIPawn))
		return false;

	// AIController�� ������ �ִ� BlackboardComponent�� �̿��Ͽ� Target��
	// ���´�.
	AActor* CheckTarget = Cast<AActor>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (IsValid(CheckTarget))
		return false;

	return AIPawn->GetPatrolEnable();
}




