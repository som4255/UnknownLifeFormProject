// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Trigger.h"
#include "TriggerAISpawn.generated.h"

/**
 * 
 */
UCLASS()
class SAC1_API ATriggerAISpawn : public ATrigger
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Data, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem>	mParticleAsset;

	UPROPERTY(Category = Data, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase>	mSoundAsset;

	UPROPERTY(Category = Data, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AAISpawnPointFromParticle>	mSpawnPoint;

protected:
	virtual void TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
