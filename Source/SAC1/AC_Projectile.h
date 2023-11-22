#pragma once
#include "GameInfo.h"
#include "Components/ActorComponent.h"
#include "AC_Projectile.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SAC1_API UAC_Projectile : public UActorComponent
{
	GENERATED_BODY()
public:	
	UAC_Projectile();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FName	m_Name;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Data", meta = (AllowPrivateAccess = true))
	FProjectileData m_Data;

public:
	FProjectileData* GetData();
	void SetInfo(const FName& name, FProjectileData* data);
};
