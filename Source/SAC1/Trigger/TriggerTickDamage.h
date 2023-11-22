#pragma once
#include "Trigger.h"
#include "TriggerTickDamage.generated.h"

UCLASS()
class SAC1_API ATriggerTickDamage : public ATrigger
{
	GENERATED_BODY()
public:
	ATriggerTickDamage();
protected:
	virtual void TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
	virtual void TriggerOverlapEnd(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index);

private:
	TSet<AActor*> m_ActorSet;
	FTimerHandle m_DamageTimerHandle;
	float m_Damage;
	float m_TickRate;

protected:
	void TickDamage();

public:
	void SetTriggerSetting(float lifeSpan,FVector extent,float damage, float tickRate=0.1f);
};
