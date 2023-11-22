// Fill out your copyright notice in the Description page of Project Settings.


#include "AISpawnPoint.h"
#include "AIPawn.h"

// Sets default values
AAISpawnPoint::AAISpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetRootComponent(mRoot);

#if WITH_EDITORONLY_DATA

	mRoot->bVisualizeComponent = true;

	mArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	mArrow->SetupAttachment(mRoot);

#endif

	mSpawnTime = 1.f;
	mTime = 0.f;
}

void AAISpawnPoint::ClearObject()
{
	mSpawnObject = nullptr;
}

void AAISpawnPoint::SpawnObject()
{
	FActorSpawnParameters	ActorParam;
	ActorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Template 변수에 특정 액터를 넣어주면 해당 액터를 복제한 액터를
	// 생성해준다.
	//ActorParam.Template = mSpawnClass->GetDefaultObject();

	// UClass가 가지고 있는 이 타입의 기본 오브젝트를 얻어온다.
	TObjectPtr<AAIPawn> DefaultObj = Cast<AAIPawn>(mSpawnClass->GetDefaultObject());

	float	HalfHeight = 0.f;

	// 기본 오브젝트가 유효할 경우 캡슐의 절반 높이를 얻어온다.
	if (IsValid(DefaultObj))
		HalfHeight = DefaultObj->GetHalfHeight();

	LOG(TEXT("HalfHeight : %.5f"), HalfHeight);

	mSpawnObject = GetWorld()->SpawnActor<AAIPawn>(mSpawnClass,
		GetActorLocation() + FVector(0.0, 0.0, (double)HalfHeight),
		GetActorRotation(), ActorParam);

	mSpawnObject->SetTeam(mTeam);
	mSpawnObject->SetSpawnPoint(this);
	mSpawnObject->SetPatrolPointArray(mPatrolPointArray);
}

// Called when the game starts or when spawned
void AAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(mSpawnClass))
	{
		SpawnObject();
	}
}

// Called every frame
void AAISpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이 스폰포인트에서 생성한 객체가 없고 스폰시킬 클래스가 있을 경우
	if (!IsValid(mSpawnObject) && IsValid(mSpawnClass))
	{
		mTime += DeltaTime;

		// 스폰시킬 시간을 지났을 경우
		if (mTime >= mSpawnTime)
		{
			// 다시 활용해야 하기 때문에 0으로 초기화한다.
			mTime = 0.f;

			SpawnObject();
		}
	}
}

