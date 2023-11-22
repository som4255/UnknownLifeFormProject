// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerPortal.h"

ATriggerPortal::ATriggerPortal()
{
	mParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));

	mParticle->SetupAttachment(RootComponent);
}

void ATriggerPortal::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	// ��ȯ�� ������ �̸��� ��ȿ�� ���
	if (mTransitionName.IsValid())
	{
		UGameplayStatics::OpenLevel(GetWorld(), mTransitionName);
	}
}
