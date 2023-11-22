#include "SAC1GameInstance.h"

USAC1GameInstance::USAC1GameInstance()
{
	m_HasWeapons.Init(false, (int32)ECharacterEquip::Food);
	PlayerHP = 0;
	PlayerHPMax = 0;

	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerTable(TEXT(
		"/Script/Engine.DataTable'/Game/Data/DT_PlayerData.DT_PlayerData'"));
	if (PlayerTable.Succeeded())
	{
		mPlayerDataTable = PlayerTable.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_WeaponData(TEXT(
		"/Game/Data/DT_WeaponData.DT_WeaponData"));
	if (DT_WeaponData.Succeeded())
	{
		m_WeaponDataTable = DT_WeaponData.Object;
	}
}

void USAC1GameInstance::Init()
{
	Super::Init();
	m_WeaponNames = m_WeaponDataTable->GetRowNames();
}

const FPlayerDataTable* USAC1GameInstance::FindPlayerData(const FName& Name)
{
	return mPlayerDataTable->FindRow<FPlayerDataTable>(Name, TEXT(""));
}

const FWeaponData* USAC1GameInstance::FindWeaponData(const FName& Name)
{
	return m_WeaponDataTable->FindRow<FWeaponData>(Name, TEXT(""));
}

void USAC1GameInstance::SetHasWeapons(int32 index, bool b)
{
	if (index < 0 || index >= m_HasWeapons.Num())
	{
		return;
	}
	m_HasWeapons[index]= b;
}

bool USAC1GameInstance::GetHasWeapons(int32 index)
{
	if(index<0|| index>=m_HasWeapons.Num())
	{
		return false;
	}
	return m_HasWeapons[index];
}

bool USAC1GameInstance::AddHp(int32 hp)
{
	PlayerHP += hp;
	if (PlayerHP > PlayerHPMax)
	{
		PlayerHP = PlayerHPMax;
	}
	if (PlayerHP < 0)
	{
		return true;
	}
	return false;
}