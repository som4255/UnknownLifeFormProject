#pragma once
#include "GameInfo.h"
#include "Camera/PlayerCameraManager.h"
#include "PCM_FPS.generated.h"

class ASAC1Character;

UCLASS()
class SAC1_API APCM_FPS : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	APCM_FPS();
protected:
	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
public:
	virtual void Tick(float DeltaTime) override;

protected:
	TObjectPtr<ASAC1Character> m_Character;
	FRotator m_FinalCamRot;
	FVector m_FinalCamLoc;
	FVector m_SwayOffset;
	float m_SwayPitchIntensity_Idle;
	float m_SwayYawIntensity_Idle;
	float m_SwayPitchIntensity_ADS;
	float m_SwayYawIntensity_ADS;

protected:
	FVector SwayIdle();
	FVector SwayADS();
	void WeaponSway();
};
