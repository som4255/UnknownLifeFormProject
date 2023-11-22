// Fill out your copyright notice in the Description page of Project Settings.


#include "AISpawnPointFromParticle.h"
#include "AIPawn.h"

// Sets default values
AAISpawnPointFromParticle::AAISpawnPointFromParticle()
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

	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));

	mParticle->SetupAttachment(mRoot);

}

void AAISpawnPointFromParticle::SpawnObject(
	TObjectPtr<UParticleSystem> ParticleAsset,
	TObjectPtr<USoundBase> SoundAsset)
{
	if (!IsValid(mSpawnClass))
		return;

	mSoundAsset = SoundAsset;

	if (IsValid(ParticleAsset))
	{
		mParticle->SetTemplate(ParticleAsset);
		mParticle->ActivateSystem(true);
	}
}

// Called when the game starts or when spawned
void AAISpawnPointFromParticle::BeginPlay()
{
	Super::BeginPlay();

	mParticle->OnSystemFinished.AddDynamic(this, &AAISpawnPointFromParticle::ParticleFinish);
}

// Called every frame
void AAISpawnPointFromParticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAISpawnPointFromParticle::ParticleFinish(UParticleSystemComponent* System)
{
	// 사운드 재생.
	UGameplayStatics::PlaySoundAtLocation(this, mSoundAsset,
		GetActorLocation());

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

	GetWorld()->SpawnActor<AAIPawn>(mSpawnClass,
		GetActorLocation() + FVector(0.0, 0.0, (double)HalfHeight),
		GetActorRotation(), ActorParam);
}

