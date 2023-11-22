#pragma once
#include "GameInfo.h"
#include "Engine/GameInstance.h"
#include "SAC1GameInstance.generated.h"


UCLASS()
class SAC1_API USAC1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USAC1GameInstance();
public:
	virtual void Init();

private:
	TObjectPtr<UDataTable>	mPlayerDataTable;
	TObjectPtr<UDataTable>	m_WeaponDataTable;
	TArray<FName>	m_WeaponNames;
	TArray<bool> m_HasWeapons;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	int32 PlayerHP;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Player")
	int32 PlayerHPMax;

public:
	const FPlayerDataTable* FindPlayerData(const FName& Name);
	const FWeaponData* FindWeaponData(const FName& Name);

	void SetHasWeapons(int32 index,bool b);
	bool GetHasWeapons(int32 index);

	FName GetWeaponName(int32 index)
	{
		if(index<0||index>= m_WeaponNames.Num())
		{
			return FName();
		}
		return m_WeaponNames[index];
	}

	bool AddHp(int32 hp);
};
