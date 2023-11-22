#pragma once
#include "../GameInfo.h"
#include "Trigger.h"
#include "TriggerRamdomSpawn.generated.h"

UCLASS()
class SAC1_API ATriggerRamdomSpawn : public ATrigger
{
	GENERATED_BODY()
public:
	ATriggerRamdomSpawn();
protected:
	virtual void TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Trigger", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<class AActor_SpawnVolume>>	m_SpawnVolumes;
};
