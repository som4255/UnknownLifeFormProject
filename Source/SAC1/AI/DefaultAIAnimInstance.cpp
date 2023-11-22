// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAIAnimInstance.h"
#include "AIPawn.h"

UDefaultAIAnimInstance::UDefaultAIAnimInstance()
{
	mAnimType = EAIAnimType::Idle;
}

void UDefaultAIAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UDefaultAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UDefaultAIAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UDefaultAIAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void UDefaultAIAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();

	 AIPawn = Cast<AAIPawn>(TryGetPawnOwner());



}

void UDefaultAIAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UDefaultAIAnimInstance::AnimNotify_Attack()
{
}

void UDefaultAIAnimInstance::AnimNotify_AttackEnd()
{
	//AAIPawn* AIPawn = Cast<AAIPawn>(TryGetPawnOwner());

	// IsValid : ��ü�� ��ȿ���� �Ǵ��Ѵ�.
	// nullptr�� ��� ��ȿ�� ��ü�� �ƴϴ�.
	if (IsValid(AIPawn))
	{
		AIPawn->SetAttackEnd(true);
	}
}

void UDefaultAIAnimInstance::AnimNotify_InteractionEnd()
{
	//AAIPawn* AIPawn = Cast<AAIPawn>(TryGetPawnOwner());

	// IsValid : ��ü�� ��ȿ���� �Ǵ��Ѵ�.
	// nullptr�� ��� ��ȿ�� ��ü�� �ƴϴ�.
	if (IsValid(AIPawn))
	{
		AIPawn->SetInteractionEnd(true);
	}
}

void UDefaultAIAnimInstance::AnimNotify_DeathEnd()
{
	//AAIPawn* AIPawn = Cast<AAIPawn>(TryGetPawnOwner());

	// IsValid : ��ü�� ��ȿ���� �Ǵ��Ѵ�.
	// nullptr�� ��� ��ȿ�� ��ü�� �ƴϴ�.
	if (IsValid(AIPawn))
	{
		AIPawn->DeathEnd();
	}
}
