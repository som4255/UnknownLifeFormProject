// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "../AIPawn.h"
#include "BrutalZombi.generated.h"

/**
 * 
 */
UCLASS()
class SAC1_API ABrutalZombi : public AAIPawn
{
	GENERATED_BODY()
	
public:
	ABrutalZombi();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

};

