#pragma once
#include "GameInfo.h"
#include "Components/ActorComponent.h"
#include "AC_ItemState.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAC1_API UAC_ItemState : public UActorComponent
{
	GENERATED_BODY()
public:	
	UAC_ItemState();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FName	m_Name;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FItemData m_ItemData;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FWeaponData m_WeaponData;

protected:
	static TObjectPtr<UDataTable>	WeaponDataTable;

protected:
	static FWeaponData* FindWeaponData(const FName& Name);

public:
	static void LoadWeaponData();

	FItemData* GetItemData();
	FWeaponData* GetWeaponData();
	void SetItemInfo(const FName& name, FItemData* data);

	bool AddHP(int32 Dmg)
	{
		if(m_ItemData.MaxHP==-1)
		{
			return false;
		}
		m_ItemData.HP -= Dmg;
		if (m_ItemData.HP <= 0)
		{
			m_ItemData.HP = 0;
			return true;
		}
		else if (m_ItemData.HP > m_ItemData.MaxHP)
		{
			m_ItemData.HP = m_ItemData.MaxHP;
		}
		return false;
	}
};
