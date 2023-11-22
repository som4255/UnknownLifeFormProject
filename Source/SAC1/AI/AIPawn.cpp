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

	// Controller의 Yaw 회전에 매칭한다.
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
	// 0 ~ 255 사이의 원하는 값을 넣어준다.
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

	// AIDataTable 애셋을 불러오지 않았을 경우 애셋을 불러온다.
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

	// SpawnPoint 없이 바로 배치해서 사용하며 PatrolPoint를 넣어준 경우
	// 이 안으로 들어오게 될 것이다.
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
		언리얼엔진 타이머
		글로벌타이머 매니저를 가지고 있고 매니저에서 타이머들을 관리한다.
		FTimerManager의 SetTimer 함수를 이용하여 타이머를 생성한다.
		*/
		// 타이머 핸들이 없을 때 타이머를 생성한다.
		if (!mHitTimerHandle.IsValid())
		{
			// Actor클래스는 GetWorldTimerManager() 함수를 지원해주고 있지만
			// 다른 클래스에서는 지원하지 않을 수 있기 때문에
			// GetWorld()->GetTimerManager() 로 접근한다.
			GetWorld()->GetTimerManager().SetTimer(mHitTimerHandle, this,
				&AAIPawn::HitTimer, 0.2f);
		}

		mHit = true;

		// MaterialInstance 전체를 반복하며 HitColor를 붉은색으로 변경한다.
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
		언리얼엔진 타이머
		글로벌타이머 매니저를 가지고 있고 매니저에서 타이머들을 관리한다.
		FTimerManager의 SetTimer 함수를 이용하여 타이머를 생성한다.
		*/
		// 타이머 핸들이 없을 때 타이머를 생성한다.
		if (!mHitTimerHandle.IsValid())
		{
			// Actor클래스는 GetWorldTimerManager() 함수를 지원해주고 있지만
			// 다른 클래스에서는 지원하지 않을 수 있기 때문에
			// GetWorld()->GetTimerManager() 로 접근한다.
			GetWorld()->GetTimerManager().SetTimer(mHitTimerHandle, this,
				&AAIPawn::HitTimer, 0.2f);
		}

		mHit = true;

		// MaterialInstance 전체를 반복하며 HitColor를 붉은색으로 변경한다.
		for (auto& Mtrl : mMaterialArray)
		{
			Mtrl->SetVectorParameterValue(TEXT("HitColor"),
				FVector(1.0, 0.0, 0.0));
		}
	}

	//에러확인용 주석

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

		// 비율을 구한다.
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

	// MaterialInstance 전체를 반복하며 HitColor를 붉은색으로 변경한다.
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


	// 현재 캐릭터의 위치에서 50cm 앞을 시작점으로 잡아준다.
	FVector	Start = GetActorLocation() + GetActorForwardVector() * 50.f;

	// 끝점은 시작점으로부터 2m 전방으로 잡아준다.
	FVector	End = Start + GetActorForwardVector() * 200.f;

	FCollisionQueryParams	param(NAME_None, false, this);

	bool Collision = GetWorld()->SweepSingleByChannel(result,
		Start, End, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(50.f), param);

	/*
	
		// 디버깅 용(에디터)으로 출력한다.
#if ENABLE_DRAW_DEBUG

	// Collision 값에 따라 true일 경우 red, false일 경우 green으로
	// 출력한다.
	FColor	DrawColor = Collision ? FColor::Red : FColor::Green;

	// FRotationMatrix::MakeFromZ(GetActorForwardVector()) : Z축을 캐릭터
	// 의 앞쪽으로 만들어주는 회전 행렬을 구한다.(FMatrix로 결과가 나온다)
	// 그래서 .ToQuat() 을 이용해서 FQuat(회전값)으로 변환한다.
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
		//	//// 액터의 생명주기를 지정한다. 5.f를 지정하면 생성되고 5초 뒤에 제거된다.
		//	//Decal->SetLifeSpan(5.f);
		//}


		//피해량 설정
		float	Dmg = 0.f;

		if (IsValid(mAIState))
			Dmg = (float)mAIState->GetData()->AttackPoint;

		//피해이벤트 생성
		FDamageEvent	DmgEvent;
		result.GetActor()->TakeDamage(Dmg, DmgEvent, GetController(),
			this);

	}
}

void AAIPawn::DeathEnd()
{
	
	// Dissolve를 활성화한다.
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