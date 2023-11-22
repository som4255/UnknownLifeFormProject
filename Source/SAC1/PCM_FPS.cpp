#include "PCM_FPS.h"
#include "SAC1Character.h"

APCM_FPS::APCM_FPS()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APCM_FPS::BeginPlay()
{
	Super::BeginPlay();
	m_Character = Cast<ASAC1Character>(PCOwner->Player);
	if (IsValid(m_Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("APCM_FPS"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APCM_FPS Failed"));
	}
}

void APCM_FPS::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void APCM_FPS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	WeaponSway();
}

FVector APCM_FPS::SwayIdle()
{
	return FVector();
}

FVector APCM_FPS::SwayADS()
{
	return FVector();
}

void APCM_FPS::WeaponSway()
{
}
