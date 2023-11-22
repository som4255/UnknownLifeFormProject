// Fill out your copyright notice in the Description page of Project Settings.


#include "ManZombi2.h"
#include "../DefaultAIAnimInstance.h"

AManZombi2::AManZombi2()
{
	PrimaryActorTick.bCanEverTick = true;

	mName = TEXT("ManZombi2");

	mBody->SetCapsuleHalfHeight(85.f);
	mBody->SetCapsuleRadius(33.f);

	mHead->SetCapsuleHalfHeight(25.f);
	mHead->SetCapsuleRadius(22.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Zombi/ManZombi2/Yaku_J_Ignite.Yaku_J_Ignite'"));

	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -85.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mHead->SetRelativeLocation(FVector(30.0, 0.0, 90.0));
	mHead->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>	AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Zombi/AB_ManZombi2.AB_ManZombi2_C'"));

	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);
}

void AManZombi2::BeginPlay()
{
	Super::BeginPlay();
}

void AManZombi2::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AManZombi2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
