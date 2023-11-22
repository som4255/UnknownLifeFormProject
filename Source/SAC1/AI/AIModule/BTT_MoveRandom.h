#pragma once
#include "../../GameInfo.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveRandom.generated.h"

UCLASS()
class SAC1_API UBTT_MoveRandom : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_MoveRandom();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};
