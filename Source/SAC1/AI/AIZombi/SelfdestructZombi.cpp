// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfdestructZombi.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIPawn.h"
#include "../AISpawnPoint.h"
#include "../DefaultAIAnimInstance.h"
#include "../AIState.h"
#include "../../Effect/DecalEffect.h"
#include "../PatrolPoint.h"
#include "../../SAC1Character.h"

ASelfdestructZombi::ASelfdestructZombi()
{

	PrimaryActorTick.bCanEverTick = true;

	mName = TEXT("selfdestructZombi");

	mBody->SetCapsuleHalfHeight(85.f);
	mBody->SetCapsuleRadius(33.f);

	mHead->SetCapsuleHalfHeight(25.f);
	mHead->SetCapsuleRadius(22.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Zombi/selfdestructZombi/Parasite_L_Starkie.Parasite_L_Starkie'"));

	if (MeshAsset.Succeeded())
		mMesh->SetSkeletalMesh(MeshAsset.Object);

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -85.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	mHead->SetRelativeLocation(FVector(20.0, 0.0, 90.0));
	mHead->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance>	AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Zombi/AB_SelfdestructionZombi.AB_SelfdestructionZombi_C'"));

	if (AnimAsset.Succeeded())
		mMesh->SetAnimInstanceClass(AnimAsset.Class);

}

void ASelfdestructZombi::BeginPlay()
{
	Super::BeginPlay();
}

void ASelfdestructZombi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);



}

void ASelfdestructZombi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASelfdestructZombi::Attack()
{
	LOG(TEXT("zombi attack"));

	FAIDataTable* data = mAIState->GetData();

	TArray<FHitResult> results;
	FVector traceStart = GetActorLocation();
	FVector traceEnd = traceStart + GetActorForwardVector() * 300.f;
	FCollisionQueryParams param(NAME_None, false, this);


	FActorSpawnParameters	actorParam;
	actorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(traceStart, FRotator::ZeroRotator, actorParam);
	decal->SetDecalMaterial(mBloodDecal);
	decal->SetLifeSpan(15.f);
	decal->SetDecalSize(FVector(200.f));

	bool isCol = GetWorld()->SweepMultiByChannel(results, traceStart, traceEnd, FQuat::Identity,
		ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(300.f), param);
//#if ENABLE_DRAW_DEBUG
//	FColor drawColor;
//	if (isCol)
//	{
//		drawColor = FColor::Red;
//	}
//	else
//	{
//		drawColor = FColor::Green;
//	}
//	DrawDebugSphere(GetWorld(), (traceStart + traceEnd) * 0.5, 300.f, 0, drawColor, false, 0.5f);
//#endif
	if (isCol)
	{
		for (auto& result : results)
		{
			FDamageEvent	dmgEvent;
			result.GetActor()->TakeDamage(data->AttackPoint, dmgEvent, GetInstigatorController(), this);
		}
	}

	Destroy();
}
