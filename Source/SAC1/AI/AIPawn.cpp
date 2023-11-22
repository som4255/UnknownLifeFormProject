#include "AIPawn.h"
#include "AISpawnPoint.h"
#include "DefaultAIController.h"
#include "AIState.h"
#include "../Effect/DecalEffect.h"
#include "PatrolPoint.h"
#include "../SAC1Character.h"
#include "../SAC1PlayerState.h"
#include "DefaultAIAnimInstance.h"

TObjectPtr<UDataTable> AAIPawn::mAIDataTable;

AAIPawn::AAIPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	mAttackEnd = false;
	mInteractionEnd = false;
	mDeath = false;

	// Controller�� Yaw ȸ���� ��Ī�Ѵ�.
	//bUseControllerRotationYaw = true;

	mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
	mHead = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Head"));
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	mAIState = CreateDefaultSubobject<UAIState>(TEXT("AIState"));

	SetRootComponent(mBody);

	mMesh->SetupAttachment(mBody);
	mHead->SetupAttachment(mBody);

	mMovement->SetUpdatedComponent(mBody);

	mHead->OnComponentHit.AddDynamic(this,
		&AAIPawn::BodyHit);

	mBody->SetCollisionProfileName(TEXT("AI"));
	mHead->SetCollisionProfileName(TEXT("AI"));

	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ADefaultAIController::StaticClass();

	mTeam = ETeam::Team2;

	mHit = false;

	mMesh->bRenderCustomDepth = true;
	// 0 ~ 255 ������ ���ϴ� ���� �־��ش�.
	mMesh->SetCustomDepthStencilValue(0);

	mDissolveCurrentTime = 0.f;
	mDissolveTime = 3.f;
	mDissolveEnable = false;

	//BloodEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BloodEffectComponent"));
	//BloodEffectComponent->SetupAttachment(mBody); 
	//BloodEffectComponent->bAutoActivate = false;

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.Blueprint'/Game/ZombiDecal/BP_BloodEffect.BP_BloodEffect'"));
	//if (ParticleAsset.Succeeded())
	//{
	//	BloodEffectComponent->SetTemplate(ParticleAsset.Object);
	//}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MIBloodDecalRE(TEXT(
		"/Script/Engine.MaterialInstanceConstant'/Game/ZombiDecal/RE/MIBloodDecalRE.MIBloodDecalRE'"));
	if (MIBloodDecalRE.Succeeded())
	{
		mBloodDecal = MIBloodDecalRE.Object;
	}


	for (int i = 1; i <= 4; ++i)
	{
		const FString string = FString::Printf(TEXT("/Script/Engine.SoundWave'/Game/ZombiSound/randomsound/Death_0%d.Death_0%d'"), i, i);

		ConstructorHelpers::FObjectFinder<USoundBase>	DeadSound(*string);
		if (DeadSound.Succeeded())
		{
			mRandomDeadSound.Add(DeadSound.Object);
		}
	}
}

void AAIPawn::LoadAIData()
{
	mAIDataTable = LoadObject<UDataTable>(nullptr,
		TEXT("/Script/Engine.DataTable'/Game/Data/DT_AIData.DT_AIData'"));
}

FAIDataTable* AAIPawn::FindAIData(const FName& Name)
{
	return mAIDataTable->FindRow<FAIDataTable>(Name, TEXT(""));
}

void AAIPawn::SetSpawnPoint(AAISpawnPoint* SpawnPoint)
{
	mSpawnPoint = SpawnPoint;
}

void AAIPawn::SetPatrolPointArray(
	const TArray<TObjectPtr<class APatrolPoint>>& Array)
{
	mPatrolPointArray = Array;

	mPatrolPoint.Add(mSpawnPoint->GetActorLocation());

	for (auto& Point : mPatrolPointArray)
	{
		mPatrolPoint.Add(Point->GetActorLocation());
	}
}

void AAIPawn::NextPatrol()
{
	mPatrolIndex = (mPatrolIndex + 1) % mPatrolPoint.Num();
}

void AAIPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// AIDataTable �ּ��� �ҷ����� �ʾ��� ��� �ּ��� �ҷ��´�.
	if (!IsValid(mAIDataTable))
		LoadAIData();

	if (IsValid(mAIDataTable))
	{
		LOG(TEXT("AIDataTable Valid"));

		FAIDataTable* Data = FindAIData(mName);

		mAIState->SetInfo(mName.ToString(), Data);

		mMovement->MaxSpeed = Data->MoveSpeed;
	}

	mPatrolIndex = 1;

	// Material Element Count
	int32	ElementCount = mMesh->GetNumMaterials();

	for (int32 i = 0; i < ElementCount; ++i)
	{
		UMaterialInstanceDynamic* Mtrl = mMesh->CreateDynamicMaterialInstance(i);

		mMaterialArray.Add(Mtrl);
	}
}

// Called when the game starts or when spawned
void AAIPawn::BeginPlay()
{
	Super::BeginPlay();

	mAnim = Cast<UDefaultAIAnimInstance>(mMesh->GetAnimInstance());

	// SpawnPoint ���� �ٷ� ��ġ�ؼ� ����ϸ� PatrolPoint�� �־��� ���
	// �� ������ ������ �� ���̴�.
	if (!mPatrolPoint.IsEmpty())
	{
		FVector	Loc = GetActorLocation();
		Loc.Z -= mBody->GetScaledCapsuleHalfHeight();

		mPatrolPoint.Add(Loc);

		for (auto& Point : mPatrolPointArray)
		{
			mPatrolPoint.Add(Point->GetActorLocation());
		}
	}
}

void AAIPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(mSpawnPoint))
		mSpawnPoint->ClearObject();
}

float AAIPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float Dmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator,
		DamageCauser);

	FActorSpawnParameters	actorParam;
	actorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector loc = GetActorLocation();
	loc.Z -= mBody->GetScaledCapsuleHalfHeight();
	ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator(-45., 90., 0.), actorParam);
	decal->SetDecalMaterial(mBloodDecal);
	decal->SetLifeSpan(5.f);
	decal->SetDecalSize(FVector(200));

	//FActorSpawnParameters	actorParam;
	//actorParam.SpawnCollisionHandlingOverride =
	//	ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//FVector loc = GetActorLocation();
	//loc.Z -= mBody->GetScaledCapsuleHalfHeight();
	//ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator(0., 90., 0.), actorParam);
	//decal->SetDecalMaterial(mBloodDecal);
	//decal->SetLifeSpan(5.f);
	//decal->SetDecalSize(FVector(1.f,200.f,200.f));



	if (IsValid(m_BloodFill))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(m_BloodFill, mMesh, NAME_None, FVector(0.0, 0.0, 75.0), FRotator(0., 90., 0), EAttachLocation::Type::KeepRelativeOffset, true);
		//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_BloodFill, GetActorLocation(), FRotator::ZeroRotator);
		//niagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
	}


	bool Death = mAIState->AddHP((int32)Dmg);


	if (Death)
	{
		if (mAnim)
			mAnim->ChangeAnim(EAIAnimType::Death);

		mBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AAIController* AI = Cast<AAIController>(GetController());

		if (IsValid(AI))
			AI->BrainComponent->StopLogic(TEXT("Death"));

		//mMesh->SetSimulatePhysics(true);         

		mDeath = true;
	}

	else
	{
		/*
		�𸮾��� Ÿ�̸�
		�۷ι�Ÿ�̸� �Ŵ����� ������ �ְ� �Ŵ������� Ÿ�̸ӵ��� �����Ѵ�.
		FTimerManager�� SetTimer �Լ��� �̿��Ͽ� Ÿ�̸Ӹ� �����Ѵ�.
		*/
		// Ÿ�̸� �ڵ��� ���� �� Ÿ�̸Ӹ� �����Ѵ�.
		if (!mHitTimerHandle.IsValid())
		{
			// ActorŬ������ GetWorldTimerManager() �Լ��� �������ְ� ������
			// �ٸ� Ŭ���������� �������� ���� �� �ֱ� ������
			// GetWorld()->GetTimerManager() �� �����Ѵ�.
			GetWorld()->GetTimerManager().SetTimer(mHitTimerHandle, this,
				&AAIPawn::HitTimer, 0.2f);
		}

		mHit = true;

		// MaterialInstance ��ü�� �ݺ��ϸ� HitColor�� ���������� �����Ѵ�.
		for (auto& Mtrl : mMaterialArray)
		{
			Mtrl->SetVectorParameterValue(TEXT("HitColor"),
				FVector(1.0, 0.0, 0.0));
		}
	}

	//if (BloodEffectComponent)
	//{
	//	BloodEffectComponent->Activate();
	//}

	return Dmg;
}

void AAIPawn::BodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ASAC1Character* player = Cast<ASAC1Character>(OtherActor);
	if (!IsValid(player))
	{
		return;
	}

	ASAC1PlayerState* state = Cast<ASAC1PlayerState>(GetPlayerState());
	if (!IsValid(state))
	{
		return;
	}



	float Dmg = state->GetData()->AttackPoint;
	

	FActorSpawnParameters	actorParam;
	actorParam.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector loc = GetActorLocation();
	loc.Z -= mBody->GetScaledCapsuleHalfHeight();
	ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator(0., 90., 0.), actorParam);
	decal->SetDecalMaterial(mBloodDecal);
	decal->SetLifeSpan(5.f);
	decal->SetDecalSize(FVector(200));



	//float DmgHead = state->GetData()->(AttackPoint * 2.f);

	//FActorSpawnParameters	actorParamHead;
	//actorParamHead.SpawnCollisionHandlingOverride =
	//	ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//FVector locHead = GetActorLocation();
	//locHead.Z -= mHead->GetScaledCapsuleHalfHeight();
	//ADecalEffect* decalHead = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator(0., 90., 0.), actorParam);
	//decalHead->SetDecalMaterial(mBloodDecal);
	//decalHead->SetLifeSpan(5.f);
	//decalHead->SetDecalSize(FVector(200));




	if (IsValid(m_BloodFill))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(m_BloodFill, mMesh, NAME_None, FVector(0.0, 0.0, 75.0), FRotator(0., 90., 0), EAttachLocation::Type::KeepRelativeOffset, true);
		//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_BloodFill, GetActorLocation(), FRotator::ZeroRotator);
		//niagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
	}


	bool Death = mAIState->AddHP((int32)Dmg);


	if (Death)
	{
		if (mAnim)
			mAnim->ChangeAnim(EAIAnimType::Death);

		mBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AAIController* AI = Cast<AAIController>(GetController());

		if (IsValid(AI))
			AI->BrainComponent->StopLogic(TEXT("Death"));

		//mMesh->SetSimulatePhysics(true);         

		mDeath = true;
	}

	else
	{
		/*
		�𸮾��� Ÿ�̸�
		�۷ι�Ÿ�̸� �Ŵ����� ������ �ְ� �Ŵ������� Ÿ�̸ӵ��� �����Ѵ�.
		FTimerManager�� SetTimer �Լ��� �̿��Ͽ� Ÿ�̸Ӹ� �����Ѵ�.
		*/
		// Ÿ�̸� �ڵ��� ���� �� Ÿ�̸Ӹ� �����Ѵ�.
		if (!mHitTimerHandle.IsValid())
		{
			// ActorŬ������ GetWorldTimerManager() �Լ��� �������ְ� ������
			// �ٸ� Ŭ���������� �������� ���� �� �ֱ� ������
			// GetWorld()->GetTimerManager() �� �����Ѵ�.
			GetWorld()->GetTimerManager().SetTimer(mHitTimerHandle, this,
				&AAIPawn::HitTimer, 0.2f);
		}

		mHit = true;

		// MaterialInstance ��ü�� �ݺ��ϸ� HitColor�� ���������� �����Ѵ�.
		for (auto& Mtrl : mMaterialArray)
		{
			Mtrl->SetVectorParameterValue(TEXT("HitColor"),
				FVector(1.0, 0.0, 0.0));
		}
	}

	//����Ȯ�ο� �ּ�

}

// Called every frame
void AAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mDissolveEnable)
	{
		mDissolveCurrentTime += DeltaTime;

		if (mDissolveCurrentTime >= mDissolveTime)
			Destroy();

		// ������ ���Ѵ�.
		float	Ratio = mDissolveCurrentTime / mDissolveTime;
		Ratio = 1.f - Ratio;
		Ratio = Ratio * 2.f - 1.f;

		for (auto& Mtrl : mMaterialArray)
		{
			Mtrl->SetScalarParameterValue(TEXT("Dissolve"), Ratio);
		}
	}
}

void AAIPawn::SetCollisionProfile(const FName& Name)
{
	mBody->SetCollisionProfileName(Name);
}

void AAIPawn::HitTimer()
{
	mHit = false;

	// MaterialInstance ��ü�� �ݺ��ϸ� HitColor�� ���������� �����Ѵ�.
	for (auto& Mtrl : mMaterialArray)
	{
		Mtrl->SetVectorParameterValue(TEXT("HitColor"),
			FVector(1.0, 1.0, 1.0));
	}

	GetWorld()->GetTimerManager().ClearTimer(mHitTimerHandle);
}

void AAIPawn::Attack()
{


	FHitResult	result;


	// ���� ĳ������ ��ġ���� 50cm ���� ���������� ����ش�.
	FVector	Start = GetActorLocation() + GetActorForwardVector() * 50.f;

	// ������ ���������κ��� 2m �������� ����ش�.
	FVector	End = Start + GetActorForwardVector() * 200.f;

	FCollisionQueryParams	param(NAME_None, false, this);

	bool Collision = GetWorld()->SweepSingleByChannel(result,
		Start, End, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(50.f), param);

	/*
	
		// ����� ��(������)���� ����Ѵ�.
#if ENABLE_DRAW_DEBUG

	// Collision ���� ���� true�� ��� red, false�� ��� green����
	// ����Ѵ�.
	FColor	DrawColor = Collision ? FColor::Red : FColor::Green;

	// FRotationMatrix::MakeFromZ(GetActorForwardVector()) : Z���� ĳ����
	// �� �������� ������ִ� ȸ�� ����� ���Ѵ�.(FMatrix�� ����� ���´�)
	// �׷��� .ToQuat() �� �̿��ؼ� FQuat(ȸ����)���� ��ȯ�Ѵ�.
	DrawDebugCircle(GetWorld(), (Start + End) / 2.f, 50.f,
		0, DrawColor, false, 1.f);
#endif
	
	*/




	if (Collision)
	{
		FActorSpawnParameters	ActorParam;
		ActorParam.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//ADefaultEffect* Effect = GetWorld()->SpawnActor<ADefaultEffect>(
		//	result.ImpactPoint,
		//	FRotator::ZeroRotator, ActorParam);

		FHitResult	LineResult;

		FCollisionQueryParams	param1(NAME_None, false, this);

		bool LineCollision = GetWorld()->LineTraceSingleByChannel(LineResult,
			result.ImpactPoint,
			result.ImpactPoint - FVector(0.0, 0.0, 200.0),
			ECollisionChannel::ECC_GameTraceChannel8, param1);

		//if (LineCollision)
		//{
		//	//ADecalEffect* Decal = GetWorld()->SpawnActor<ADecalEffect>(
		//	//	LineResult.ImpactPoint,
		//	//	FRotator::ZeroRotator, ActorParam);

		//	//Decal->SetDecalMaterial(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/ZombiDecal/RE/MIBloodDecalRE.MIBloodDecalRE'"));
		//	//// ������ �����ֱ⸦ �����Ѵ�. 5.f�� �����ϸ� �����ǰ� 5�� �ڿ� ���ŵȴ�.
		//	//Decal->SetLifeSpan(5.f);
		//}


		//���ط� ����
		float	Dmg = 0.f;

		if (IsValid(mAIState))
			Dmg = (float)mAIState->GetData()->AttackPoint;

		//�����̺�Ʈ ����
		FDamageEvent	DmgEvent;
		result.GetActor()->TakeDamage(Dmg, DmgEvent, GetController(),
			this);

	}
}

void AAIPawn::DeathEnd()
{
	
	// Dissolve�� Ȱ��ȭ�Ѵ�.
	for (auto& Mtrl : mMaterialArray)
	{
		Mtrl->SetScalarParameterValue(TEXT("DissolveEnable"),
			1.f);
	}

	mMovement->StopMovementImmediately();
	mMovement->Velocity = FVector::Zero();


/*	mMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);
	mMesh->SetAllBodiesSimulatePhysics(true);
	mMesh->SetSimulatePhysics(true);
	mMesh->WakeAllRigidBodies();
	mMesh->bBlendPhysics = true;
	mMesh->SetLinearDamping(20000.f);  */ 


	if (mRandomDeadSound.IsEmpty())
	{
		return;
	}
	int32 randIndex = FMath::Rand() % mRandomDeadSound.Num();
	UGameplayStatics::PlaySoundAtLocation(this, mRandomDeadSound[randIndex], GetActorLocation());


	mDissolveEnable = true;


	mBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}