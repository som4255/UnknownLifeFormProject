#include "Actor_SpawnVolume.h"
#include "Actor_PickUp.h"
#include "AI/AIPawn.h"

AActor_SpawnVolume::AActor_SpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	if(GetLocalRole()==ROLE_Authority)
	{
		m_SpawnDelayRangeMin = 1.0f;
		m_SpawnDelayRangeMax = 4.5f;
		m_SpawnCountMax = 300;
		m_SpawnCount = 0;
		m_IsRotateX = false;
		m_IsRotateY = false;
		m_IsRotateZ = true;

		m_SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
		SetRootComponent(m_SpawnArea);
		m_SpawnArea->SetCollisionProfileName(TEXT("NoCollision"));
		m_SpawnArea->SetGenerateOverlapEvents(false);
		m_SpawnArea->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

#if WITH_EDITORONLY_DATA
		m_SpawnArea->bVisualizeComponent = true;
#endif
	}
}

void AActor_SpawnVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AActor_SpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AActor_SpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AActor_SpawnVolume::SpawnPickUp()
{
	if(GetLocalRole()==ROLE_Authority&& !m_SpawnThings.IsEmpty())
	{
		UWorld* const world = GetWorld();
		if(!IsValid(world))
		{
			return;
		}
		if(m_SpawnCountMax!=-1&& m_SpawnCount>= m_SpawnCountMax)
		{
			GetWorldTimerManager().ClearTimer(m_SpawnTimer);
			Destroy();
			return;
		}
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = GetInstigator();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector spawnLoc = GetRandomPointInVolume();
		FRotator spawnRot;
		spawnRot.Yaw = FMath::FRand() * 360.f* (float)m_IsRotateZ;
		spawnRot.Pitch = FMath::FRand() * 360.f * (float)m_IsRotateY;
		spawnRot.Roll = FMath::FRand() * 360.f * (float)m_IsRotateX;

		int32 randIndex = FMath::Rand() % m_SpawnThings.Num();
		AActor* const spawnedActor =
			world->SpawnActor<AActor>(m_SpawnThings[randIndex], spawnLoc, spawnRot, spawnParams);
		SetSpawningActive(true);
		++m_SpawnCount;

		AActor_PickUp* spawnedPickUp = Cast<AActor_PickUp>(spawnedActor);
		if(IsValid(spawnedPickUp) && !m_Names.IsEmpty())
		{
			randIndex = FMath::Rand()% m_Names.Num();
			spawnedPickUp->SetName(m_Names[randIndex]);
		}	
	}
}

FVector AActor_SpawnVolume::GetRandomPointInVolume()
{
	if(!IsValid(m_SpawnArea))
	{
		return FVector();
	}
	FVector spawnOrigin = m_SpawnArea->Bounds.Origin;
	FVector spawnExtent = m_SpawnArea->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(spawnOrigin, spawnExtent);
}

void AActor_SpawnVolume::SetSpawningActive(bool isSpawn)
{
	if(GetLocalRole()==ROLE_Authority)
	{
		if(isSpawn)
		{
			m_SpawnDelay = FMath::FRandRange(m_SpawnDelayRangeMin, m_SpawnDelayRangeMax);
			GetWorldTimerManager().SetTimer(m_SpawnTimer, this,
				&AActor_SpawnVolume::SpawnPickUp, m_SpawnDelay, false);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(m_SpawnTimer);
			m_SpawnCount = 0;
		}
	}
}

