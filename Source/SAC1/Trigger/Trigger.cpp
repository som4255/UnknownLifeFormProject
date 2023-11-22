#include "Trigger.h"
#include "../Effect/DefaultEffect.h"

ATrigger::ATrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	
	mBody = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));

	SetRootComponent(mBody);
#if WITH_EDITORONLY_DATA

	mBody->bVisualizeComponent = true;
#endif
	mBody->SetBoxExtent(FVector(100.0, 100.0, 100.0));

	mBody->SetCollisionProfileName(TEXT("ObjectTrigger"));

	mBody->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	mBody->OnComponentBeginOverlap.AddDynamic(this, &ATrigger::OverlapBegin);
	mBody->OnComponentEndOverlap.AddDynamic(this, &ATrigger::OverlapEnd);

	mTriggerType = EUserTriggerType::AllObject;

	m_DestroyWhenTrigger = false;
}

void ATrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	switch (mTriggerType)
	{
	case EUserTriggerType::Player:
		mBody->SetCollisionProfileName(TEXT("PlayerTrigger"));
		break;
	case EUserTriggerType::Enemy:
		mBody->SetCollisionProfileName(TEXT("EnemyTrigger"));
		break;
	case EUserTriggerType::AllObject:
		mBody->SetCollisionProfileName(TEXT("ObjectTrigger"));
		break;
	}
}

// Called when the game starts or when spawned
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrigger::OverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor * OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	TriggerOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex,
		bFromSweep, SweepResult);

	/*FActorSpawnParameters	ActorParam;
	ActorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADefaultEffect* Effect = GetWorld()->SpawnActor<ADefaultEffect>(
		SweepResult.ImpactPoint,
		FRotator::ZeroRotator, ActorParam);

	Effect->SetParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Ability/Defense/P_Shield_Spawn.P_Shield_Spawn'"));
	Effect->SetAudioAsset(TEXT("/Script/Engine.SoundWave'/Game/Sound/Fire1.Fire1'"));*/
}

void ATrigger::OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 index)
{
	TriggerOverlapEnd(comp, otherActor, otherComp, index);
}

void ATrigger::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_DestroyWhenTrigger)
	{
		Destroy();
	}
}

void ATrigger::TriggerOverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 index)
{
}

