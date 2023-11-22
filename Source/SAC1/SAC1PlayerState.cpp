#include "SAC1PlayerState.h"
#include "SAC1GameInstance.h"

void ASAC1PlayerState::InitPlayerData(const FName& name)
{
	USAC1GameInstance* gameInst = GetWorld()->GetGameInstance<USAC1GameInstance>();
	if (IsValid(gameInst))
	{
		m_data = *gameInst->FindPlayerData(name);
		gameInst->PlayerHP=m_data.HPMax;
		gameInst->PlayerHPMax=m_data.HPMax;
	}
}

FPlayerDataTable* ASAC1PlayerState::GetData()
{
	return &m_data;
}

void ASAC1PlayerState::SetPlayerData(FPlayerDataTable* data)
{
	m_data = *data;
}