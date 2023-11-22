#include "TriggerTickDamage.h"

ATriggerTickDamage::ATriggerTickDamage()
{
	m_Damage = 0.f;
	m_TickRate = 0.1f;
}

void ATriggerTickDamage::TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::TriggerOverlap(OverlappedComponent, OtherActor, 
		OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	m_ActorSet.Emplace(OtherActor);
}

void ATriggerTickDamage::TriggerOverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index)
{
	Super::TriggerOverlapEnd(comp, otherActor, otherComp, index);
	m_ActorSet.Remove(otherActor);
}

void ATriggerTickDamage::TickDamage()
{
	for (auto& actor : m_ActorSet)
	{
		FDamageEvent	dmgEvent;
		actor->TakeDamage(m_Damage, dmgEvent, GetInstigatorController(), this);
	}
}

void ATriggerTickDamage::SetTriggerSetting(float lifeSpan, FVector extent, float damage, float tickRate)
{
	SetLifeSpan(lifeSpan);
	mBody->SetBoxExtent(extent);
	m_Damage = damage;
	m_TickRate = tickRate;
	GetWorld()->GetTimerManager().SetTimer(m_DamageTimerHandle, this, &ATriggerTickDamage::TickDamage, tickRate, true);
}
