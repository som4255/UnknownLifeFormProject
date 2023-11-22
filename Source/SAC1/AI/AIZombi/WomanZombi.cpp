// Fill out your copyright notice in the Description page of Project Settings.


#include "WomanZombi.h"
#include "../DefaultAIAnimInstance.h"

AWomanZombi::AWomanZombi()
{
	PrimaryActorTick.bCanEverTick = true;     

	mName = TEXT("WomanZombi");

	mBody->SetCapsuleHalfHeight(85.f);
	mBody->SetCapsuleRadius(30.f);

	mHead->SetCapsuleHalfHeight(25.f);
	mHead->SetCapsuleRadius(22.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Zombi/WomanZombi/Zombiegirl_W_Kurniawan.Zombiegirl_W_Kurniawan'"));

	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);       
	 
	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -85.0));    
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mHead->SetRelativeLocation(FVector(20.0, 0.0, 100.0));
	mHead->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>	AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Zombi/AB_WomanZombi.AB_WomanZombi_C'"));    

	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);

}

void AWomanZombi::BeginPlay()
{
	Super::BeginPlay();   
}

void AWomanZombi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWomanZombi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
