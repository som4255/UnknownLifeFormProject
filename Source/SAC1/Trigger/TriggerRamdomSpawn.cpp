#include "TriggerRamdomSpawn.h"
#include "../Actor_SpawnVolume.h"

ATriggerRamdomSpawn::ATriggerRamdomSpawn()
{
	m_DestroyWhenTrigger = true;
	mTriggerType = EUserTriggerType::Player;
}

void ATriggerRamdomSpawn::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::TriggerOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!m_SpawnVolumes.IsEmpty())
	{
		for (auto spawnVolume : m_SpawnVolumes)
		{
			spawnVolume->SetSpawningActive(true);
		}
	}
}
