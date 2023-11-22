// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "../AIPawn.h"
#include "WomanZombi.generated.h"

/**
 * 
 */
UCLASS()
class SAC1_API AWomanZombi : public AAIPawn
{
	GENERATED_BODY()

public:
	AWomanZombi();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

};
