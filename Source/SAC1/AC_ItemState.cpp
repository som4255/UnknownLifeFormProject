#include "AC_ItemState.h"

TObjectPtr<UDataTable> UAC_ItemState::WeaponDataTable;

UAC_ItemState::UAC_ItemState()
{
	PrimaryComponentTick.bCanEverTick = false;
	LoadWeaponData();
}

void UAC_ItemState::BeginPlay()
{
	Super::BeginPlay();
}

void UAC_ItemState::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FWeaponData* UAC_ItemState::FindWeaponData(const FName& Name)
{
	return WeaponDataTable->FindRow<FWeaponData>(Name, TEXT(""));
}

void UAC_ItemState::LoadWeaponData()
{
	WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_WeaponData.DT_WeaponData"));
}

FItemData* UAC_ItemState::GetItemData()
{
	return &m_ItemData;
}

FWeaponData* UAC_ItemState::GetWeaponData()
{
	return &m_WeaponData;
}

void UAC_ItemState::SetItemInfo(const FName& name, FItemData* data)
{
	m_Name = name;
	m_ItemData = *data;
	if(m_ItemData.ItemKind==EItem::Weapon)
	{
		  FWeaponData* weaponData = FindWeaponData(name);
		  if(weaponData)
		  {
			  m_WeaponData = *weaponData;
		  }
	}
}

