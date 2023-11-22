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

	// BehaviorTreeComponent를 이용하여 AIController를 얻어올 수 있다.
	AAIController* Controller = OwnerComp.GetAIOwner();

	AAIPawn* AIPawn = Cast<AAIPawn>(Controller->GetPawn());

	// AIPawn이 아닐 경우 정지.
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

	//FVector SweepDirection = FVector(1.0f, 0.0f, 0.0f); // 스위핑 방향 설정 (이 예제에서는 X축으로 스위핑)
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


	// 디버깅 용(에디터)으로 출력한다.
//#if ENABLE_DRAW_DEBUG
//
//	// Collision 값에 따라 true일 경우 red, false일 경우 green으로
//	// 출력한다.
//	FColor	DrawColor = Collision ? FColor::Red : FColor::Green;
//
//	// FRotationMatrix::MakeFromZ(GetActorForwardVector()) : Z축을 캐릭터
//	// 의 앞쪽으로 만들어주는 회전 행렬을 구한다.(FMatrix로 결과가 나온다)
//	// 그래서 .ToQuat() 을 이용해서 FQuat(회전값)으로 변환한다.
//	DrawDebugSphere(GetWorld(), AILoc,
//		AIPawn->GetAIState()->GetData()->InteractionDistance,
//		20, DrawColor, false, 0.35f);
//
//#endif

	// 충돌이 되었다면 Blackboard의 Target 변수에 충돌된 물체를 지정하고
	// 충돌이 아니면 nullptr을 지정한다.
	if (Collision)
	{
		// Controller로부터 Blackboard를 얻어와서 Target을 지정한다.
		// FHitResult의 GetActor() 함수는 충돌된 액터를 가져오는 함수이다.
		Controller->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), result.GetActor());
	}

	else
	{
		Controller->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"), nullptr);
	}
}
