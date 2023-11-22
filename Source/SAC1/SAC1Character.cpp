#include "SAC1Character.h"
#include "SAC1Projectile.h"
#include "SAC1PlayerController.h"
#include "SAC1AnimInstance.h"
#include "SAC1PlayerState.h"
#include "SAC1GameInstance.h"
#include "SAC1HUD.h"
#include "Actor_PickUp.h"
#include "Actor_PickUpWeapon.h"
#include "TP_WeaponComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Effect/DecalEffect.h"

ASAC1Character::ASAC1Character()
{
	m_MoveForwardValue = 0.f;
	m_MoveRightValue = 0.f;
	m_MaxWalkSpeed = 150.f;
	m_MaxSprintSpeed = 750.f;
	m_IsSprinting = false;
	m_CanMove = true;

	m_CameraSpeed = 25.f;
	m_CurWeaponIndex = -1;
	m_TargetIndexForChangeWeapon = -1;
	m_WeaponIndexDir = 0;
	m_IsInvertX = false;
	m_IsInvertY = true;
	m_IsInvisible = false;
	m_Team = ETeam::Team1;
	m_StartCamRelativeLoc = FVector(20., 15., 0.);

	m_Weapons.Init(nullptr,(int32)ECharacterEquip::Food);
	
	GetCapsuleComponent()->InitCapsuleSize(40.f, 91.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	SetRootComponent(GetCapsuleComponent());

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, 
		-GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->bReceivesDecals=false;

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_SpringArm->SetupAttachment(GetMesh(), TEXT("head"));
	m_SpringArm->TargetArmLength = 0.f;

	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Camera->SetupAttachment(m_SpringArm);
	m_Camera->SetRelativeLocation(m_StartCamRelativeLoc);
	m_Camera->SetRelativeRotation(FRotator(-90.f, 90.f, 0.f));
	m_Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->MaxWalkSpeed = 75.f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Belica_Biohazard(TEXT(
	"/Game/ParagonLtBelica/Characters/Heroes/Belica/Skins/Biohazard/Meshes/Belica_Biohazard.Belica_Biohazard"));
	if (Belica_Biohazard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Belica_Biohazard.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>	AB_Player(TEXT(
		"/Game/ParagonLtBelica/Retargeter/AB_Player.AB_Player_C"));
	if (AB_Player.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AB_Player.Class);
	}
	for (int i = 1; i <= 7;++i) 
	{
		const FString string = FString::Printf(TEXT(
			"/Game/KBJ/Audios/LtBelica_Death_0%d0.LtBelica_Death_0%d0"), i, i);
		ConstructorHelpers::FObjectFinder<USoundBase>	DeadSound(*string);
		if (DeadSound.Succeeded())
		{
			m_DeadSounds.Add(DeadSound.Object);
		}
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MIBloodDecalRE(TEXT(
		"/Game/ZombiDecal/RE/MIBloodDecalRE.MIBloodDecalRE"));
	if (MIBloodDecalRE.Succeeded())
	{
		m_HitMaterial= MIBloodDecalRE.Object;
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_BloodEffect(TEXT(
		"/Game/ZombiDecal/NS_BloodEffect.NS_BloodEffect"));
	if (NS_BloodEffect.Succeeded())
	{
		m_BloodFill = NS_BloodEffect.Object;
	}
}

void ASAC1Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ASAC1Character::BeginPlay()
{
	Super::BeginPlay();
	m_AnimInst = Cast<USAC1AnimInstance>(GetMesh()->GetAnimInstance());

	ASAC1PlayerController* controller = Cast<ASAC1PlayerController>(Controller);
	if (IsValid(controller)&& IsValid(controller->PlayerCameraManager))
	{
		controller->PlayerCameraManager->ViewPitchMax = 70.f;
		controller->PlayerCameraManager->ViewPitchMin = -40.f;
	}

	USAC1GameInstance* gameInst = GetWorld()->GetGameInstance<USAC1GameInstance>();
	if (IsValid(gameInst))
	{
		int32 size = m_Weapons.Num();
		for (int32 i = 0; i < size; ++i)
		{
			if (gameInst->GetHasWeapons(i))
			{
				AActor_PickUpWeapon* pickUP = GetWorld()->SpawnActor<AActor_PickUpWeapon>(FActorSpawnParameters());
				if (IsValid(pickUP) && pickUP->GetActive())
				{
					pickUP->SetName(gameInst->GetWeaponName(i));
					if (pickUP->PickedUpBy(this))
					{
						pickUP->SetActive(false);
						m_Weapons[i]->AttachWeapon();
					}
				}
			}
		}
	}
}

void ASAC1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	ASAC1PlayerController* controller = Cast<ASAC1PlayerController>(Controller);
	if (IsValid(input) && IsValid(controller))
	{
		input->BindAction(controller->m_MousePos, ETriggerEvent::Triggered, this, &ASAC1Character::CameraRotation);
		input->BindAction(controller->m_MouseWheel, ETriggerEvent::Triggered, this, &ASAC1Character::ChangeWeapon);
		input->BindAction(controller->m_Space, ETriggerEvent::Started, this, &ASAC1Character::Jump);
		input->BindAction(controller->m_Space, ETriggerEvent::Completed, this, &ASAC1Character::StopJumping);
		input->BindAction(controller->m_F, ETriggerEvent::Started, this, &ASAC1Character::CollectPickUps);
		input->BindAction(controller->m_Move, ETriggerEvent::Triggered, this, &ASAC1Character::Move);
		input->BindAction(controller->m_LShift, ETriggerEvent::Started, this, &ASAC1Character::Sprint);
		input->BindAction(controller->m_LShift, ETriggerEvent::Completed, this, &ASAC1Character::Sprint);
		input->BindAction(controller->ToggleCheat, ETriggerEvent::Started, this, &ASAC1Character::ToggleCheat);
		input->BindAction(controller->WeaponSlot, ETriggerEvent::Started, this, &ASAC1Character::ChangeWeaponByNum);
		controller->SetNewController();
	}
}

float ASAC1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if(m_IsInvisible)
	{
		return DamageAmount;
	}
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	USAC1GameInstance* instance = GetWorld()->GetGameInstance<USAC1GameInstance>();
	if (IsValid(instance))
	{
		if(instance->AddHp(-(int)DamageAmount))
		{
			OnPlayerDeath();
		}
		else
		{
			m_AnimInst->HitReaction();

			FActorSpawnParameters	actorParam;
			actorParam.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector loc = GetActorLocation();
			loc.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			ADecalEffect* decal = GetWorld()->SpawnActor<ADecalEffect>(loc, FRotator(0., 90., 0.), actorParam);
			decal->SetDecalMaterial(m_HitMaterial);
			decal->SetLifeSpan(10.f);
			decal->SetDecalSize(FVector(GetCapsuleComponent()->GetScaledCapsuleRadius()));

			if (IsValid(m_BloodFill))
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(m_BloodFill, GetMesh(), NAME_None, 
					FVector(0.f), FRotator(0.,90.,0.), EAttachLocation::Type::KeepRelativeOffset, true);
				//niagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
			}
		}
	}
	return DamageAmount;
}

void ASAC1Character::BodyHit(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& result)
{
}

void ASAC1Character::OverlapBegin(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index, bool bFromSweep, const FHitResult& result)
{
}

void ASAC1Character::OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index)
{
}

void ASAC1Character::Move(const FInputActionValue& Value)
{
	FVector movementVector = Value.Get<FVector>();
	m_MoveForwardValue = movementVector.X;
	m_MoveRightValue = movementVector.Y;
	if (!m_CanMove || !Controller)
	{
		return;
	}
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);
	const FVector forwardDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(forwardDir, m_MoveForwardValue);
	AddMovementInput(rightDir, m_MoveRightValue);
}

void ASAC1Character::CameraRotation(const FInputActionValue& Value)
{
	float deltaTime = GetWorld()->GetDeltaSeconds();
	m_ScreenRotVec = Value.Get<FVector2D>();
	double x = m_ScreenRotVec.X * deltaTime * m_CameraSpeed;
	double y = m_ScreenRotVec.Y * deltaTime * m_CameraSpeed;
	if (m_IsInvertX)
	{
		x *= -1;
	}
	if (m_IsInvertY)
	{
		y *= -1;
	}
	AddControllerYawInput(x);
	AddControllerPitchInput(y);
}

void ASAC1Character::ChangeWeapon(const FInputActionValue& Value)
{	
	if(m_CurWeaponIndex==-1||m_AnimInst->IsAnyMontagePlaying())
	{
		return;
	}
	m_WeaponIndexDir = (int)Value.Get<float>();
	m_AnimInst->ChangeWeapon();
}

void ASAC1Character::ChangeWeaponByNum(const FInputActionValue& Value)
{
	if (m_CurWeaponIndex == -1 || m_AnimInst->IsAnyMontagePlaying())
	{
		return;
	}
	m_TargetIndexForChangeWeapon= (int)Value.Get<FVector>().X;
	if(m_CurWeaponIndex+1 == m_TargetIndexForChangeWeapon)
	{
		return;
	}
	m_AnimInst->ChangeWeapon();
}

void ASAC1Character::Jump()
{
	Super::Jump();
}

void ASAC1Character::StopJumping()
{
	Super::StopJumping();
}

void ASAC1Character::Sprint()
{
	m_IsSprinting = !m_IsSprinting;
	if (m_IsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = m_MaxSprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed;
	}
}

void ASAC1Character::ToggleCheat()
{
	m_IsInvisible = !m_IsInvisible;
}

void ASAC1Character::CollectPickUps()
{
	if(m_AnimInst->IsAnyMontagePlaying())
	{
		return;
	}
	UTP_WeaponComponent* curWeapon= GetCurWeapon();
	if(IsValid(curWeapon)&& curWeapon->GetIsReloading())
	{
		return;
	}

	TArray<FHitResult> results;
	FVector extent = FVector(GetCapsuleComponent()->GetScaledCapsuleRadius())*0.5f;
	extent.Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector traceStart = GetActorLocation() -GetActorUpVector()* extent.Z*0.5;
	FVector traceEnd = traceStart + GetActorForwardVector() * extent.X;
	FCollisionQueryParams param(NAME_None, false, this);
	bool isCol = GetWorld()->SweepMultiByChannel(results, traceStart, traceEnd, FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeBox(extent), param);
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
//	DrawDebugBox(GetWorld(), (traceStart+ traceEnd)*0.5, extent, drawColor, false, 0.5f);
//#endif
	if (isCol)
	{		
		for (auto& result : results)
		{
			AActor_PickUp* const pickUP = Cast<AActor_PickUp>(result.GetActor());
			if (IsValid(pickUP) && pickUP->GetActive())
			{
				if(pickUP->PickedUpBy(this))
				{
					pickUP->SetActive(false);
					m_AnimInst->CollectPickUps();
					break;
				}
			}
		}
	}
}

void ASAC1Character::SetCharacterState(ECharacterEquip state)
{
	m_AnimInst->SetCharacterState(state);
}

ECharacterEquip ASAC1Character::GetCharacterState()
{
	return m_AnimInst->GetCharacterState();
}

void ASAC1Character::SetCurWeapon()
{
	m_CurWeaponIndex += m_WeaponIndexDir;
	int32 weaponCount = m_Weapons.Num();
	if (m_CurWeaponIndex < 0)
	{
		m_CurWeaponIndex = weaponCount - 1;
	}
	else if (m_CurWeaponIndex >= weaponCount)
	{
		m_CurWeaponIndex = 0;
	}
	for (auto weapon : m_Weapons)
	{
		if (!weapon)
		{
			if (weapon == m_Weapons[m_CurWeaponIndex])
			{
				m_CurWeaponIndex = (m_CurWeaponIndex + m_WeaponIndexDir) % weaponCount;
				if (m_CurWeaponIndex < 0)
				{
					m_CurWeaponIndex = weaponCount - 1;
				}
			}
			continue;
		}
		weapon->SetVisibility(false);
	}
	m_WeaponIndexDir = 0;
	if(m_TargetIndexForChangeWeapon!=-1&& m_TargetIndexForChangeWeapon< m_Weapons.Num())
	{
		m_CurWeaponIndex = m_TargetIndexForChangeWeapon-1;
		m_TargetIndexForChangeWeapon = -1;
	}
	if (!IsValid(m_Weapons[m_CurWeaponIndex])) 
	{
		return; 
	}
	m_Weapons[m_CurWeaponIndex]->SetVisibility(true);
	m_Weapons[m_CurWeaponIndex]->SetWeaponUI(ESlateVisibility::Visible);
	SetCharacterState((ECharacterEquip)(m_CurWeaponIndex + 1));
}

UTP_WeaponComponent* ASAC1Character::GetCurWeapon()
{
	if(m_CurWeaponIndex==-1)
	{
		return nullptr;
	}
	return m_Weapons[m_CurWeaponIndex];
}

bool ASAC1Character::TryAddWeapon(UTP_WeaponComponent* weapon, ECharacterEquip equip)
{
	int32 index = (int)equip - 1;
	if(m_Weapons[index])
	{
		return false;
	}
	m_Weapons[index] = weapon;
	m_CurWeaponIndex = index;
	USAC1GameInstance* gameInst = GetWorld()->GetGameInstance<USAC1GameInstance>();
	gameInst->SetHasWeapons(index,true);
	SetCurWeapon();
	return true;
}

void ASAC1Character::OnPlayerDeath()
{
	ASAC1PlayerController* controller = Cast<ASAC1PlayerController>(Controller);
	if (IsValid(controller))
	{
		controller->SetShowMouseCursor(true);
	}
	DetachFromControllerPendingDestroy();

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	
	//이거 넣으면 틱데미지 에러남
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	ASAC1HUD* hud = Cast<ASAC1HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (IsValid(hud))
	{
		hud->SetPlayerDeadUI(ESlateVisibility::Visible);
	}

	if(m_DeadSounds.IsEmpty())
	{
		return;
	}
	int32 randIndex = FMath::Rand() % m_DeadSounds.Num();
	UGameplayStatics::PlaySoundAtLocation(this, m_DeadSounds[randIndex], GetActorLocation());
}

void ASAC1Character::PickUpArmo(ECharacterEquip equip, float value)
{
	int index = (int)equip - 1;
	if(index<0|| index>=m_Weapons.Num())
	{
		return;
	}
	if (!IsValid(m_Weapons[index]))
	{
		return;
	}
	m_Weapons[index]->PickUpArmo(value);
}

bool ASAC1Character::GetIsADS()
{
	if (!IsValid(GetCurWeapon()))
	{
		return false;
	}
	return GetCurWeapon()->GetIsADS();
}
/*			
FActorSpawnParameters spawnParams;
spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
TSubclassOf<UTP_WeaponComponent> weaponClass = UTP_WeaponComponent::StaticClass();
UTP_WeaponComponent* newWeapon = Cast<UTP_WeaponComponent>(AddComponentByClass(weaponClass, true, FTransform::Identity, true));
if (IsValid(newWeapon))
{
	newWeapon->SetName(TEXT("Rifle"));
	if (newWeapon->TryAttachWeapon(this))
	{
		newWeapon->AttachWeapon();
	}
}
*/