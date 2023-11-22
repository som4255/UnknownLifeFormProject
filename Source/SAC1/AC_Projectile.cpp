#include "AC_Projectile.h"

UAC_Projectile::UAC_Projectile()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void UAC_Projectile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FProjectileData* UAC_Projectile::GetData()
{
	return &m_Data;
}

void UAC_Projectile::SetInfo(const FName& name, FProjectileData* data)
{
	m_Name = name;
	m_Data = *data;
}

