// Fill out your copyright notice in the Description page of Project Settings.


#include "BrutalZombi.h"
#include "../DefaultAIAnimInstance.h"

ABrutalZombi::ABrutalZombi()
{
	PrimaryActorTick.bCanEverTick = true;

	mName = TEXT("BrutalZombi");

	mBody->SetCapsuleHalfHeight(83.f);
	mBody->SetCapsuleRadius(35.f);

	mHead->SetCapsuleHalfHeight(17.f);
	mHead->SetCapsuleRadius(17.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Zombi/BrutalZombi/Skeletonzombie_T_Avelange.Skeletonzombie_T_Avelange'"));

	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -83.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mHead->SetRelativeLocation(FVector(20.0, 0.0, 90.0));
	mHead->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>	AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Zombi/AB_BrutalZombi.AB_BrutalZombi_C'"));

	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);


}

void ABrutalZombi::BeginPlay()
{
	Super::BeginPlay();
}

void ABrutalZombi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABrutalZombi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
