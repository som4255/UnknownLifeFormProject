// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckTeam.h"
#include "../AIPawn.h"
#include "../AIState.h"
#include "../../SAC1Character.h"
#include "../DefaultAIController.h"

UBTDecorator_CheckTeam::UBTDecorator_CheckTeam()
{
}

bool UBTDecorator_CheckTeam::CalculateRawConditionValue(
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

	if (!IsValid(CheckTarget))
		return false;

	FVector	AILoc = AIPawn->GetActorLocation();
	FVector	TargetLoc = CheckTarget->GetActorLocation();

	AILoc.Z -= AIPawn->GetHalfHeight();
	TargetLoc.Z -= AIPawn->GetHalfHeight();

	float Distance = FVector::Distance(AILoc, TargetLoc);

	// �Ÿ����� �� ��ü���� ĸ�� �ݰ��� ���ش�.
	Distance -= AIPawn->GetCapsuleRadius();

	// Target�� RootComponent�� ���ͼ� Capsule���� Ȯ���Ѵ�.
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(CheckTarget->GetRootComponent());

	if (IsValid(Capsule))
	{
		Distance -= Capsule->GetScaledCapsuleRadius();
	}

	if (Distance > AIPawn->GetAIState()->GetData()->AttackDistance)
		return false;

	ASAC1Character* Target = Cast<ASAC1Character>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (IsValid(Target))
	{
		return AIPawn->GetTeam() == Target->GetTeam();
	}

	AAIPawn* TargetPawn = Cast<AAIPawn>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (IsValid(TargetPawn))
		return AIPawn->GetTeam() == TargetPawn->GetTeam();

	return false;
}


