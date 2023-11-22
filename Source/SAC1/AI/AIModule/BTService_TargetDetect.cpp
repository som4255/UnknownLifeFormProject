// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TargetDetect.h"
#include "../AIPawn.h"
#include "../AIState.h"
#include "../DefaultAIController.h"

UBTService_TargetDetect::UBTService_TargetDetect()
{
	NodeName = TEXT("TargzetDetect");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_TargetDetect::TickNode(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// BehaviorTreeComponent�� �̿��Ͽ� AIController�� ���� �� �ִ�.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn�� �ƴ� ��� ����.
	if (!IsValid(AIPawn))
		return;

	

	FVector	AILoc = AIPawn->GetActorLocation();

	AILoc.Z -= AIPawn->GetHalfHeight();

	FHitResult	result;

	FCollisionQueryParams	param(NAME_None, false, AIPawn);

	bool Collision = GetWorld()->SweepSingleByChannel(result,
		AILoc, AILoc,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(AIPawn->GetAIState()->GetData()->InteractionDistance),
		param);






	//FVector	AILoc = AIPawn->GetActorLocation();

	//AILoc.Z -= AIPawn->GetHalfHeight();

	//FVector SweepDirection = FVector(1.0f, 0.0f, 0.0f); // ������ ���� ���� (�� ���������� X������ ������)
	//float SweepDistance = AIPawn->GetAIState()->GetData()->InteractionDistance;


	//FHitResult	result;

	//FCollisionQueryParams	param(NAME_None, false, AIPawn);

	//bool Collision = GetWorld()->SweepSingleByChannel(result,
	//	AILoc, AILoc + SweepDirection * SweepDistance,
	//	FQuat::Identity,
	//	ECollisionChannel::ECC_GameTraceChannel4,
	//	FCollisionShape::MakeBox(FVector(SweepDistance, SweepDistance, 450.f)), param);


//
//#if ENABLE_DRAW_DEBUG
//
//	FColor	DrawColor;
//
//	if (Collision)
//	{
//		DrawColor = FColor::Red;
//	}
//	else
//	{
//		DrawColor = FColor::Green;
//	}
//
//	DrawDebugBox(GetWorld(), AILoc, FVector(SweepDistance, SweepDistance, 450.f), DrawColor, false, 0.35f);
//
//#endif


	// ����� ��(������)���� ����Ѵ�.
//#if ENABLE_DRAW_DEBUG
//
//	// Collision ���� ���� true�� ��� red, false�� ��� green����
//	// ����Ѵ�.
//	FColor	DrawColor = Collision ? FColor::Red : FColor::Green;
//
//	// FRotationMatrix::MakeFromZ(GetActorForwardVector()) : Z���� ĳ����
//	// �� �������� ������ִ� ȸ�� ����� ���Ѵ�.(FMatrix�� ����� ���´�)
//	// �׷��� .ToQuat() �� �̿��ؼ� FQuat(ȸ����)���� ��ȯ�Ѵ�.
//	DrawDebugSphere(GetWorld(), AILoc,
//		AIPawn->GetAIState()->GetData()->InteractionDistance,
//		20, DrawColor, false, 0.35f);
//
//#endif

	// �浹�� �Ǿ��ٸ� Blackboard�� Target ������ �浹�� ��ü�� �����ϰ�
	// �浹�� �ƴϸ� nullptr�� �����Ѵ�.
	if (Collision)
	{
		// Controller�κ��� Blackboard�� ���ͼ� Target�� �����Ѵ�.
		// FHitResult�� GetActor() �Լ��� �浹�� ���͸� �������� �Լ��̴�.
		Controller->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), result.GetActor());
	}

	else
	{
		Controller->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), nullptr);
	}
}
