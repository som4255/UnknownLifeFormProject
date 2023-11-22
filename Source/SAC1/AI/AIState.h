#pragma once
#include "../GameInfo.h"
#include "Components/ActorComponent.h"
#include "AIState.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAC1_API UAIState : public UActorComponent
{
	GENERATED_BODY()
public:
	UAIState();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FString	m_Name;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FAIDataTable m_Data;

public:
	FAIDataTable* GetData();
	void SetInfo(const FString& name, FAIDataTable* data);

	bool AddHP(int32 Dmg)
	{
		m_Data.HP -= Dmg;

		if (m_Data.HP <= 0)
		{
			m_Data.HP = 0;
			return true;
		}
		else if (m_Data.HP > m_Data.MaxHP)
		{
			m_Data.HP = m_Data.MaxHP;
		}
		return false;
	}
};
