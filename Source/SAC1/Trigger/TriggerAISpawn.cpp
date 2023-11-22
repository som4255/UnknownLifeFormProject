// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerAISpawn.h"
#include "../AI/AISpawnPointFromParticle.h"

void ATriggerAISpawn::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	LOG(TEXT("AISpawn In Object"));

	if (IsValid(mSpawnPoint))
		mSpawnPoint->SpawnObject(mParticleAsset, mSoundAsset);
}
