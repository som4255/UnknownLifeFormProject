#include "TP_WeaponComponent.h"
#include "SAC1Character.h"
#include "SAC1PlayerState.h"
#include "SAC1Projectile.h"
#include "SAC1PlayerController.h"
#include "SAC1HUD.h"
#include "SAC1GameInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UTP_WeaponComponent::UTP_WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_ProjectileClass = ASAC1Projectile::StaticClass();
	m_CurArmo = 0;
	m_TotalArmo = 0;
	m_IsAttached = false;

	m_CurTargetingValue = 0.f;
	m_IsTargeting = false;
	m_IsReloading = false;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	bRenderCustomDepth = true;
	SetCustomDepthStencilValue(0);

	static ConstructorHelpers::FObjectFinder<UCurveFloat>	Curve_HorizontalRecoil(TEXT(
		"/Game/KBJ/Curves/Curve_HorizontalRecoil.Curve_HorizontalRecoil"));
	if (Curve_HorizontalRecoil.Succeeded())
	{
		m_HorizontalCurve= Curve_HorizontalRecoil.Object;
	}
	static ConstructorHelpers::FObjectFinder<UCurveFloat>	Curve_VerticalRecoil(TEXT(
		"/Game/KBJ/Curves/Curve_VerticalRecoil.Curve_VerticalRecoil"));
	if (Curve_VerticalRecoil.Succeeded())
	{
		m_VerticalCurve= Curve_VerticalRecoil.Object;
	}
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	FOnTimelineFloat recoilXCurve;
	FOnTimelineFloat recoilYCurve;
	recoilXCurve.BindUFunction(this, FName("StartHorizontalRecoil"));
	recoilYCurve.BindUFunction(this, FName("StartVerticalRecoil"));
	if(IsValid(m_HorizontalCurve)&& IsValid(m_VerticalCurve))
	{
		m_RecoilTimeline.AddInterpFloat(m_HorizontalCurve, recoilXCurve);
		m_RecoilTimeline.AddInterpFloat(m_VerticalCurve, recoilYCurve);
	}
	m_HUD= Cast<ASAC1HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(m_RecoilTimeline.IsPlaying()&& GetVisibleFlag())
	{
		m_RecoilTimeline.TickTimeline(DeltaTime);
	}
	if (m_RecoilTimeline.IsReversing())
	{
		if (!IsValid(Character))
		{
			return;
		}
		AController* controller = Character->GetController();
		if (!IsValid(controller))
		{
			return;
		}	
		FVector2D vec = Character->GetScreenRotVec();
		if (abs(vec.Y > 0.005) || abs(vec.X > 0.005)|| !GetVisibleFlag())
		{
			m_RecoilTimeline.Stop();
			return;
		}
		FRotator newRot = UKismetMathLibrary::RInterpTo(Character->GetControlRotation(),
			m_StartRot, DeltaTime, 4.5f);	
		Character->GetController()->ClientSetRotation(newRot);	
	}
}

void UTP_WeaponComponent::Fire()
{	
	if (!IsValid(Character) || m_CurArmo <= 0 || !GetVisibleFlag()|| m_IsReloading)
	{
		OnStopFire();
		return;
	}
	UWorld* const world = GetWorld();
	AController* controller = Character->GetController();
	if (!IsValid(world)||!IsValid(controller))
	{
		return;
	}

	if (IsValid(m_WeaponData.FireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_WeaponData.FireSound, Character->GetActorLocation());
	}

	if (IsValid(m_WeaponData.FireAnimation))
	{
		UAnimInstance* animInst = Character->GetMesh()->GetAnimInstance();
		if (IsValid(animInst) && !animInst->Montage_IsPlaying(m_WeaponData.FireAnimation))
		{
			animInst->Montage_Play(m_WeaponData.FireAnimation, 1.f);
		}
	}

	if (m_WeaponData.AttackType == EAttackType::Shoot)
	{
		m_CurArmo -= m_WeaponData.BulletCount;
		for (int i = 0; i < m_WeaponData.BulletCount; ++i)
		{
			FHitResult hit;
			FVector camLoc;
			FRotator camRot;
			Character->GetController()->GetPlayerViewPoint(camLoc, camRot);
			FCollisionQueryParams params;
			params.AddIgnoredActor(Character);
			FVector traceStart = camLoc;
			FVector traceEnd = traceStart + camRot.Vector() * 10000;
			if (!m_IsTargeting)
			{
				traceEnd.Y += traceEnd.Y * FMath::RandRange(-0.35f, 0.35f);
				traceEnd.Z += traceEnd.Z * FMath::RandRange(-0.35f, 0.35f);
			}
			else
			{
				traceEnd.Y += traceEnd.Y * FMath::RandRange(-0.05f, 0.05f);
				traceEnd.Z += traceEnd.Z * FMath::RandRange(-0.05f, 0.05f);
			}
			bool isCol = world->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, params);
			//#if ENABLE_DRAW_DEBUG
			//	DrawDebugLine(world, traceStart, traceEnd, FColor::Green, false, 3.f, 0, 0.5f);
			//#endif
			if (isCol)
			{
				//#if ENABLE_DRAW_DEBUG
				//		DrawDebugBox(world,hit.Location,FVector(15.),FColor::Red,false,3.f,0,3.f);
				//#endif
				if (IsValid(m_WeaponData.HitEmitter))
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, m_WeaponData.HitEmitter, hit.Location);
				}
				if (IsValid(m_WeaponData.HitDecalMaterial))
				{
					UGameplayStatics::SpawnDecalAtLocation(world, m_WeaponData.HitDecalMaterial,
						FVector(15.), hit.Location, hit.ImpactNormal.Rotation(), 10.f);
				}
				auto state = Character->GetPlayerState<ASAC1PlayerState>();
				if (IsValid(state))
				{
					float dmg = (float)state->GetData()->AttackPoint* m_WeaponData.AtkRate;
					FDamageEvent	dmgEvent;
					hit.GetActor()->TakeDamage(dmg, dmgEvent, controller, Character);
				}
			}
		}
	}
	SetWeaponUI(ESlateVisibility::Visible);
}

void UTP_WeaponComponent::PickUpArmo(float value)
{
	m_TotalArmo += value;
	if (m_TotalArmo> m_WeaponData.ArmoMax)
	{
		m_TotalArmo = m_WeaponData.ArmoMax;
	}
	if (GetVisibleFlag())
	{
		SetWeaponUI(ESlateVisibility::Visible);
	}
}

void UTP_WeaponComponent::OnStartFire()
{
	if (!IsValid(Character) || m_CurArmo <= 0|| !GetVisibleFlag()|| m_IsReloading)
	{
		OnStopFire();
		return;
	}
	m_StartRot = Character->GetControlRotation();
	Fire();
	StartRecoil();
	GetWorld()->GetTimerManager().SetTimer(m_AutoFireHandle, this,
		&UTP_WeaponComponent::Fire, m_WeaponData.FireRate, true);
}

void UTP_WeaponComponent::OnStopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(m_AutoFireHandle);
	ReverseRecoil();
}

void UTP_WeaponComponent::OnStartReload()
{
	if (!IsValid(Character)|| !GetVisibleFlag())
	{
		return;
	}
	if(IsValid(m_HUD))
	{
		m_HUD->SetReloadingImage(ESlateVisibility::Visible);
	}

	if (IsValid(m_WeaponData.ReloadAnimation))
	{
		UAnimInstance* animInst = Character->GetMesh()->GetAnimInstance();
		if (IsValid(animInst) && !animInst->Montage_IsPlaying(m_WeaponData.ReloadAnimation))
		{
			animInst->Montage_Play(m_WeaponData.ReloadAnimation, 1.f);
		}
	}
	m_IsReloading = true;
}

void UTP_WeaponComponent::Reload()
{
	if (IsValid(m_HUD))
	{
		m_HUD->SetReloadingImage(ESlateVisibility::Collapsed);
	}
	if(m_TotalArmo==-1)
	{
		m_CurArmo = m_WeaponData.Armo;
	}
	else if(m_TotalArmo>m_WeaponData.Armo)
	{
		m_CurArmo = m_WeaponData.Armo;
		m_TotalArmo -= m_WeaponData.Armo;
	}
	else
	{		
		m_CurArmo = m_TotalArmo;
		m_TotalArmo = 0;
	}
	SetWeaponUI(ESlateVisibility::Visible);
	m_IsReloading = false;
}

void UTP_WeaponComponent::StartHorizontalRecoil(float value)
{
	if (!IsValid(Character)||m_RecoilTimeline.IsReversing())
	{
		return;
	}
	Character->AddControllerYawInput(value);
}

void UTP_WeaponComponent::StartVerticalRecoil(float value)
{
	if (!IsValid(Character) || m_RecoilTimeline.IsReversing())
	{
		return;
	}
	Character->AddControllerPitchInput(value);
}

void UTP_WeaponComponent::StartRecoil()
{
	m_RecoilTimeline.PlayFromStart();
}

void UTP_WeaponComponent::ReverseRecoil()
{
	m_RecoilTimeline.ReverseFromEnd();
}

void UTP_WeaponComponent::StartTargeting()
{
	if (!GetVisibleFlag()
		||m_WeaponData.AttackType!=EAttackType::Shoot
		||Character->GetIsSprinting())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_TargetingHandle);
		return;
	}
	m_IsTargeting = true;
	GetWorld()->GetTimerManager().SetTimer(m_TargetingHandle, this,
		&UTP_WeaponComponent::Targeting, 0.01f, true);
}

void UTP_WeaponComponent::StopTargeting()
{
	if (!GetVisibleFlag()
		||m_WeaponData.AttackType!=EAttackType::Shoot
		||Character->GetIsSprinting())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_TargetingHandle);
		return;
	}
	m_IsTargeting = false;
	GetWorld()->GetTimerManager().SetTimer(m_TargetingHandle, this,
		&UTP_WeaponComponent::UnTargeting, 0.01f, true);
}

void UTP_WeaponComponent::Targeting()
{
	FTransform ironSightTransform = GetSocketTransform(TEXT("Ironsight"));
	FTransform ironSightTransform2 = GetSocketTransform(TEXT("Ironsight"),ERelativeTransformSpace::RTS_ParentBoneSpace);
	FVector ironSightLoc = ironSightTransform.GetLocation();
	UCameraComponent* cam = Character->GetFirstPersonCameraComponent();

	cam->SetWorldLocation(FMath::VInterpTo(cam->GetComponentLocation(),
		ironSightLoc, GetWorld()->DeltaTimeSeconds, 4.f));
	if (cam->GetComponentLocation().Equals(ironSightLoc))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_TargetingHandle);
	}

	m_CurTargetingValue -= 0.5f;
	if (m_CurTargetingValue < m_FOVMin)
	{
		m_CurTargetingValue = m_FOVMin;
	}
	cam->SetFieldOfView(m_CurTargetingValue);
}

void UTP_WeaponComponent::UnTargeting()
{
	FVector startCamLoc = Character->GetStartCamRelativeLoc();
	UCameraComponent* cam = Character->GetFirstPersonCameraComponent();
	cam->SetRelativeLocation(FMath::VInterpTo(cam->GetRelativeLocation(),
		startCamLoc, GetWorld()->DeltaTimeSeconds, 4.f));
	if (cam->GetComponentLocation().Equals(startCamLoc))
	{
		GetWorld()->GetTimerManager().ClearTimer(m_TargetingHandle);
	}

	m_CurTargetingValue += 1.f;
	if (m_CurTargetingValue > m_FOVMax)
	{
		m_CurTargetingValue = m_FOVMax;
	}
	cam->SetFieldOfView(m_CurTargetingValue);
}

bool UTP_WeaponComponent::TryAttachWeapon(ASAC1Character* TargetCharacter)
{
	Character = TargetCharacter;
	m_CurTargetingValue = Character->GetFirstPersonCameraComponent()->FieldOfView;
	m_FOVMin = m_CurTargetingValue * 0.75f;
	m_FOVMax = m_CurTargetingValue;
	if(!Character->TryAddWeapon(this, m_WeaponData.State))
	{		
		return false;
	}
	return true;
}

void UTP_WeaponComponent::AttachWeapon()
{
	if(m_IsAttached)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	m_IsAttached=AttachToComponent(Character->GetMesh(), AttachmentRules, FName(TEXT("hand_r")));
	PickUpArmo(m_WeaponData.ArmoCountWhenPick);
	Reload();
	SetRelativeLocationAndRotation(m_WeaponData.WeaponOffset, m_WeaponData.WeaponRot);
	SetCustomDepthStencilValue(0);

	ASAC1PlayerController* controller = Cast<ASAC1PlayerController>(Character->GetController());
	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(controller->InputComponent);
	if (IsValid(input) && IsValid(controller))
	{
		input->BindAction(controller->m_MouseLClick, ETriggerEvent::Started, this, &UTP_WeaponComponent::OnStartFire);
		input->BindAction(controller->m_MouseLClick, ETriggerEvent::Completed, this, &UTP_WeaponComponent::OnStopFire);
		input->BindAction(controller->m_MouseRClick, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartTargeting);
		input->BindAction(controller->m_MouseRClick, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopTargeting);
		input->BindAction(controller->m_R, ETriggerEvent::Started, this, &UTP_WeaponComponent::OnStartReload);
		//controller->SetNewController();
	}
}

void UTP_WeaponComponent::MeleeAttack()
{
	UWorld* const world = GetWorld();
	AController* controller= Character->GetController();
	if (!IsValid(world)|| !IsValid(controller))
	{
		return;
	}
	FVector	start = Character->GetActorLocation();
	FVector	end = start+ Character->GetActorForwardVector() * 200.f;
	TArray<FHitResult>	resultArray;
	FCollisionQueryParams	param(NAME_None, false, Character);
	bool isCol = world->SweepMultiByChannel(resultArray, start, end, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3, FCollisionShape::MakeSphere(50.f), param);

#if ENABLE_DRAW_DEBUG
	FColor	drawColor = isCol ? FColor::Red : FColor::Green;
	DrawDebugSphere(world, (start + end) *0.5f, 50.f,0, drawColor, false, 0.75f);
#endif
	if (isCol)
	{
		FActorSpawnParameters	actorParam;
		actorParam.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FHitResult	lineResult;
		FCollisionQueryParams	param1(NAME_None, false, Character);

		for (auto& result : resultArray)
		{
			isCol = world->LineTraceSingleByChannel(lineResult, result.ImpactPoint,
				result.ImpactPoint - FVector(0.0, 0.0, 200.0),
				ECollisionChannel::ECC_GameTraceChannel8, param1);
			if (isCol)
			{
				if (IsValid(m_WeaponData.HitEmitter))
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, m_WeaponData.HitEmitter, result.Location);
				}
				if (IsValid(m_WeaponData.HitDecalMaterial))
				{
					UGameplayStatics::SpawnDecalAtLocation(world, m_WeaponData.HitDecalMaterial,
						FVector(15.), result.Location, result.ImpactNormal.Rotation(), 10.f);
				}
				auto state = Character->GetPlayerState<ASAC1PlayerState>();
				if (IsValid(state))
				{
					float dmg = (float)state->GetData()->AttackPoint * m_WeaponData.AtkRate;
					FDamageEvent	dmgEvent;
					result.GetActor()->TakeDamage(dmg, dmgEvent, controller, Character);
				}
			}
		}
	}
}

void UTP_WeaponComponent::ThrowThing()
{
	UWorld* const world = GetWorld();
	ASAC1PlayerController* controller = Cast<ASAC1PlayerController>(Character->GetController());
	if (!IsValid(world) || !IsValid(controller))
	{
		return;
	}
	FRotator spawnRotation = controller->PlayerCameraManager->GetCameraRotation();
	const FVector spawnLocation =GetComponentLocation()
		+ spawnRotation.RotateVector(m_WeaponData.MuzzleOffset);
	FActorSpawnParameters actorSpawnParams;
	actorSpawnParams.SpawnCollisionHandlingOverride = 
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASAC1Projectile* thing=world->SpawnActor<ASAC1Projectile>(m_ProjectileClass, 
		spawnLocation, spawnRotation, actorSpawnParams);
	thing->SetName(m_Name);
	thing->SetProjectileMesh(GetSkeletalMeshAsset());
	thing->SetProjectileParticle(m_WeaponData.HitEmitter);
	thing->SetProjectileDecal(m_WeaponData.HitDecalMaterial);
	auto state = Character->GetPlayerState<ASAC1PlayerState>();
	if (IsValid(state))
	{
		float dmg = (float)state->GetData()->AttackPoint * m_WeaponData.AtkRate;
		thing->SetProjectileDamage(dmg);
	}

	--m_TotalArmo;
	if(m_TotalArmo<0)
	{
		m_TotalArmo = 0;
		--m_CurArmo;
	}
}

void UTP_WeaponComponent::EatFood()
{
	USAC1GameInstance* instance = GetWorld()->GetGameInstance<USAC1GameInstance>();
	//auto state = Character->GetPlayerState<ASAC1PlayerState>();
	if (IsValid(instance))
	{
		instance->AddHp(m_WeaponData.BulletCount);
	}
	--m_TotalArmo;
	if (m_TotalArmo < 0)
	{
		m_TotalArmo = 0;
		--m_CurArmo;
	}
}

bool UTP_WeaponComponent::GetIsADS()
{
	return m_IsTargeting;
}

void UTP_WeaponComponent::SetName(const FName& name)
{
	m_Name = name;
	USAC1GameInstance* gameInst = GetWorld()->GetGameInstance<USAC1GameInstance>();
	if (!IsValid(gameInst))
	{
		return;
	}
	m_WeaponData = *gameInst->FindWeaponData(name);
}

void UTP_WeaponComponent::SetWeaponData(FWeaponData* data)
{
	m_WeaponData = *data;
}

void UTP_WeaponComponent::SetWeaponUI(ESlateVisibility visible)
{
	if(!IsValid(m_HUD))
	{
		return;
	}
	m_HUD->SetWeaponUI(visible, m_Name, m_CurArmo, m_TotalArmo);
}