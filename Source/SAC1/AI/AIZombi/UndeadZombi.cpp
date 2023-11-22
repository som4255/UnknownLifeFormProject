// Fill out your copyright notice in the Description page of Project Settings.


#include "UndeadZombi.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIPawn.h"
#include "../AISpawnPoint.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../../Effect/DecalEffect.h"
#include "../PatrolPoint.h"
#include "../../SAC1Character.h"



AUndeadZombi::AUndeadZombi()
{
	PrimaryActorTick.bCanEverTick = true;

	mName = TEXT("UndeadZombi");

	mBody->SetCapsuleHalfHeight(85.f);
	mBody->SetCapsuleRadius(40.f);

	mHead->SetCapsuleHalfHeight(30.f);
	mHead->SetCapsuleRadius(30.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Zombi/UndeadZombi/Pumpkinhulk_L_Shaw.Pumpkinhulk_L_Shaw'"));

	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -85.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mHead->SetRelativeLocation(FVector(60.0, 0.0, 70.0));
	mHead->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>	AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Zombi/AB_UndeadZombi.AB_UndeadZombi_C'"));

	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);


}

void AUndeadZombi::BeginPlay()
{
	Super::BeginPlay();
}

void AUndeadZombi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AUndeadZombi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



