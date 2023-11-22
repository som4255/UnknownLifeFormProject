// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultEffect.h"

// Sets default values
ADefaultEffect::ADefaultEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	mAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	SetRootComponent(mParticle);

	mAudio->SetupAttachment(mParticle);
}

// Called when the game starts or when spawned
void ADefaultEffect::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADefaultEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultEffect::SetParticleAsset(const FString& Path)
{
	// FString�� �������� �ϸ� �� ���ڿ� Ŭ������ ������ �ִ�
	// ���ڿ� ������(TCHAR*) �� ��ȯ���ش�.
	// �׷��� 2��° ���ڿ� Path�� �������� ���� �ּ��� ��θ�
	// �־��ָ� �ش� �ּ��� �ҷ����� �ȴ�.
	UParticleSystem* Particle = LoadObject<UParticleSystem>(nullptr,
		*Path);

	// �ҷ��� �ּ��� ��ȿ�� �˻縦 �ǽ��Ѵ�.
	// ��ȿ�� ��� ParticleSystemComponent�� ��ƼŬ �ּ��� �����Ѵ�.
	if (IsValid(Particle))
	{
		mParticle->SetTemplate(Particle);
		mParticle->OnSystemFinished.AddDynamic(this, &ADefaultEffect::ParticleFinish);
	}
}

void ADefaultEffect::SetParticleAsset(UParticleSystem* Particle)
{
	if (IsValid(Particle))
	{
		mParticle->SetTemplate(Particle);
		mParticle->OnSystemFinished.AddDynamic(this, &ADefaultEffect::ParticleFinish);
	}
}

void ADefaultEffect::SetAudioAsset(const FString& Path)
{
	// FString�� �������� �ϸ� �� ���ڿ� Ŭ������ ������ �ִ�
	// ���ڿ� ������(TCHAR*) �� ��ȯ���ش�.
	// �׷��� 2��° ���ڿ� Path�� �������� ���� �ּ��� ��θ�
	// �־��ָ� �ش� �ּ��� �ҷ����� �ȴ�.
	USoundBase* Sound = LoadObject<USoundBase>(nullptr,
		*Path);

	// �ҷ��� �ּ��� ��ȿ�� �˻縦 �ǽ��Ѵ�.
	// ��ȿ�� ��� AudioComponent�� Sound �ּ��� �����Ѵ�.
	if (IsValid(Sound))
	{
		mAudio->SetSound(Sound);
		mAudio->Play();
	}
}

void ADefaultEffect::SetAudioAsset(USoundBase* Sound)
{
	if (IsValid(Sound))
	{
		mAudio->SetSound(Sound);
		mAudio->Play();
	}
}

void ADefaultEffect::ParticleFinish(UParticleSystemComponent* System)
{
	//LOG(TEXT("Particle Finish"));
	Destroy();
}

