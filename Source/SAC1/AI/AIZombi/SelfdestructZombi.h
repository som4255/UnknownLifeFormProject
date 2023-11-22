// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "../AIPawn.h"
#include "SelfdestructZombi.generated.h"

/**
 * 
 */
UCLASS()
class SAC1_API ASelfdestructZombi : public AAIPawn
{
	GENERATED_BODY()
	
public:
	ASelfdestructZombi();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;
	void Attack() override;

};
