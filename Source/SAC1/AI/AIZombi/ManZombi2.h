// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "../AIPawn.h"
#include "ManZombi2.generated.h"

/**
 * 
 */
UCLASS()
class SAC1_API AManZombi2 : public AAIPawn
{
	GENERATED_BODY()
	
public:
	AManZombi2();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual void Tick(float DeltaTime) override;

};