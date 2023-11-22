#pragma once
#include "GameInfo.h"
#include "GameFramework/PlayerState.h"
#include "SAC1PlayerState.generated.h"

UCLASS()
class SAC1_API ASAC1PlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FPlayerDataTable m_data;

public:
	void InitPlayerData(const FName& name);
	FPlayerDataTable* GetData();
	void SetPlayerData(FPlayerDataTable* data);
};
