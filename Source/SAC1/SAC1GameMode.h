#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SAC1GameMode.generated.h"

UCLASS(minimalapi)
class ASAC1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASAC1GameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
	virtual void InitGameState();
	virtual void PostLogin(APlayerController* NewPlayer);
	virtual void BeginPlay()	override;
	virtual void Tick(float DeltaTime)	override;
};



