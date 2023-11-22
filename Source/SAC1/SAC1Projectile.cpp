#include "SAC1Projectile.h"
#include "AC_Projectile.h"
#include "Trigger/TriggerTickDamage.h"
#include "Effect/DecalEffect.h"

TObjectPtr<UDataTable> ASAC1Projectile::m_ProjectTileDataTable;

ASAC1Projectile::ASAC1Projectile() 
{
	m_Damage = 0.f;
	m_Name = TEXT("Granade");

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASAC1Projectile::OnHit);		
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->SetupAttachment(RootComponent);

	m_ProjectileMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ProjectileMesh"));
	m_ProjectileMesh->SetupAttachment(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 700.f;
	ProjectileMovement->MaxSpeed = 700.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	m_ProjectileState = CreateDefaultSubobject<UAC_Projectile>(TEXT("ProjectileState"));

}

void ASAC1Projectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetName(m_Name);
}

void ASAC1Projectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FProjectileData* data= m_ProjectileState->GetData();
	if(!data||!data->HasDestroyEvent)
	{
		return;
	}

	TArray<FHitResult> results;
	FVector traceStart = GetActorLocation();
	FVector traceEnd = traceStart + GetActorForwardVector() * data->ExplosionRadius;
	FCollisionQueryParams param(NAME_None, false, this);

	if (IsValid(data->DestroySound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, data->DestroySound, traceStart);
	}
	if (IsValid(data->DestroyParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), data->DestroyParticle, traceStart);
	}
	FActorSpawnParameters	actorParam;
	actorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(traceStart, FRotator::ZeroRotator, actorParam);
	decal->SetDecalMaterial(data->DestroyDecalMaterial);
	decal->SetLifeSpan(15.f);
	decal->SetDecalSize(FVector(data->ExplosionRadius));

	bool isCol = GetWorld()->SweepMultiByChannel(results, traceStart, traceEnd, FQuat::Identity,
		ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(data->ExplosionRadius), param);
//#if ENABLE_DRAW_DEBUG
//	FColor drawColor;
//	if (isCol)
//	{
//		drawColor = FColor::Red;
//	}
//	else
//	{
//		drawColor = FColor::Green;
//	}
//	DrawDebugSphere(GetWorld(), (traceStart + traceEnd) * 0.5, data->ExplosionRadius,0, drawColor, false, 0.5f);
//#endif
	if (isCol)
	{
		for (auto& result : results)
		{
			FDamageEvent	dmgEvent;
			result.GetActor()->TakeDamage(m_Damage, dmgEvent, GetInstigatorController(), this);
		}
	}
}

void ASAC1Projectile::FireBottle()
{
	FProjectileData* data = m_ProjectileState->GetData();
	if (!data)
	{
		return;
	}

	FVector loc = GetActorLocation();
	if (IsValid(data->DestroySound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, data->DestroySound, loc);
	}
	if (IsValid(data->DestroyParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), data->DestroyParticle, loc);
	}


	FActorSpawnParameters	actorParam;
	actorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator::ZeroRotator, actorParam);
	decal->SetDecalMaterial(data->DestroyDecalMaterial);
	decal->SetLifeSpan(15.f);
	decal->SetDecalSize(FVector(data->ExplosionRadius));

	ATriggerTickDamage* trigger= GetWorld()->SpawnActor<ATriggerTickDamage>(
		loc, FRotator::ZeroRotator, actorParam);
	trigger->SetTriggerSetting(10.f,
		FVector(data->ExplosionRadius, data->ExplosionRadius,10.f),m_Damage,1.f);
	Destroy();
}

FProjectileData* ASAC1Projectile::FindProjectileData(const FName& Name)
{
	return m_ProjectTileDataTable->FindRow<FProjectileData>(Name, TEXT(""));
}

void ASAC1Projectile::LoadProjectileData()
{
	m_ProjectTileDataTable = LoadObject<UDataTable>(nullptr,TEXT(
		"/Game/Data/DT_ProjectileData.DT_ProjectileData"));
}

void ASAC1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	if(m_ProjectileState->GetData()->HasHitEvent)
	{
		FireBottle();
	}
}

void ASAC1Projectile::SetName(const FName& name)
{
	m_Name = name;
	if (!IsValid(m_ProjectTileDataTable))
	{
		LoadProjectileData();
	}
	if (!IsValid(m_ProjectTileDataTable))
	{
		return;
	}
	FProjectileData* data = FindProjectileData(m_Name);
	if (!data)
	{
		return;
	}
	m_ProjectileState->SetInfo(m_Name, data);
	SetLifeSpan(data->LifeSpan);
	ProjectileMovement->bShouldBounce = data->IsBounce;
	CollisionComp->SetSphereRadius(data->CollisionRadius);
	ProjectileMovement->InitialSpeed = data->InitSpeed;
	ProjectileMovement->MaxSpeed = data->MaxSpeed;;
}

void ASAC1Projectile::SetProjectileMesh(USkeletalMesh* mesh)
{
	m_ProjectileMesh->SetSkeletalMeshAsset(mesh);
}

void ASAC1Projectile::SetProjectileParticle(UParticleSystem* particle)
{
	m_HitEmitter = particle;
}

void ASAC1Projectile::SetProjectileDecal(UMaterialInstance* decal)
{
	m_HitDecalMaterial = decal;
}

void ASAC1Projectile::SetProjectileDamage(float damage)
{
	m_Damage = damage;
}
