#include "SAC1GameMode.h"
#include "SAC1Character.h"
#include "SAC1GameState.h"
#include "SAC1PlayerState.h"
#include "SAC1PlayerController.h"
#include "SAC1HUD.h"
#include "UObject/ConstructorHelpers.h"

ASAC1GameMode::ASAC1GameMode() 
	: Super()
{
	DefaultPawnClass = ASAC1Character::StaticClass();
	PlayerStateClass = ASAC1PlayerState::StaticClass();
	GameStateClass = ASAC1GameState::StaticClass();
	PlayerControllerClass = ASAC1PlayerController::StaticClass();
	HUDClass = ASAC1HUD::StaticClass();
}

void ASAC1GameMode::InitGame(const FString& MapName, const FString& Options,
	FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ASAC1GameMode::InitGameState()
{
	Super::InitGameState();
}

void ASAC1GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	auto state = NewPlayer->GetPlayerState<ASAC1PlayerState>();
	if (IsValid(state))
	{
		state->InitPlayerData(TEXT("FPS"));
	}
}

void ASAC1GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ASAC1GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
