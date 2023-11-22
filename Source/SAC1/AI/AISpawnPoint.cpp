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

	// Template ������ Ư�� ���͸� �־��ָ� �ش� ���͸� ������ ���͸�
	// �������ش�.
	//ActorParam.Template = mSpawnClass->GetDefaultObject();

	// UClass�� ������ �ִ� �� Ÿ���� �⺻ ������Ʈ�� ���´�.
	TObjectPtr<AAIPawn> DefaultObj = Cast<AAIPawn>(mSpawnClass->GetDefaultObject());

	float	HalfHeight = 0.f;

	// �⺻ ������Ʈ�� ��ȿ�� ��� ĸ���� ���� ���̸� ���´�.
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

	// �� ��������Ʈ���� ������ ��ü�� ���� ������ų Ŭ������ ���� ���
	if (!IsValid(mSpawnObject) && IsValid(mSpawnClass))
	{
		mTime += DeltaTime;

		// ������ų �ð��� ������ ���
		if (mTime >= mSpawnTime)
		{
			// �ٽ� Ȱ���ؾ� �ϱ� ������ 0���� �ʱ�ȭ�Ѵ�.
			mTime = 0.f;

			SpawnObject();
		}
	}
}

