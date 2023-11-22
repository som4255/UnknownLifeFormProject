#include "SAC1AnimInstance.h"
#include "SAC1Character.h"
#include "TP_WeaponComponent.h"

USAC1AnimInstance::USAC1AnimInstance()
{
	m_CharState=ECharacterEquip::None;
	m_Speed =0.f;
	m_IsInAir = false;
	m_PitchInput = 0.f;
	m_YawInput = 0.f;
	m_YawFrameChange = 0.f;
	m_YawLastTick = 0.f;
	m_Yaw = 0.f;
	m_RootYawOffset = 0.f;
	m_DistanceCurveValueLastFrame = 0.f;
	m_DistanceCurveValue = 0.f;
	m_IsAccerelating = false;
	m_IsSprinting = false;
	m_DoOnce = false;
	m_IsADS = false;

	m_RHandEffectorLoc = FVector(-1,-4.,2.);
	m_RHandJointTargetLoc = FVector(-45.,-27.,18.);
	m_RHandRotIntensity = 10.f;
	m_TurnInPlaceLimit = 90.f;
	m_LHandIK = 1.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Picking(TEXT(
		"/Game/AnimStarterPack/ECT/AM_Picking.AM_Picking"));
	if (AM_Picking.Succeeded())
	{
		m_GrabItem=AM_Picking.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_EquipChange(TEXT(
		"/Game/AnimStarterPack/ECT/AM_EquipChange.AM_EquipChange"));
	if (AM_EquipChange.Succeeded())
	{
		m_EquipChange = AM_EquipChange.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_RifleHitReact1(TEXT(
		"/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Mixamo/AM_HitReact.AM_HitReact"));
	if (AM_RifleHitReact1.Succeeded())
	{
		m_DamagedMontage = AM_RifleHitReact1.Object;
	}
}

void USAC1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ASAC1Character* player = Cast<ASAC1Character>(TryGetPawnOwner());
	if(IsValid(player))
	{
		m_Character = player;
	}
}

void USAC1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!IsValid(m_Character))
	{
		return;
	}
	BaseVariableSetting();
	AimOffset(DeltaSeconds);
	TurnInChange();
	SnapLHandToWeapon();
	WeaponSway(DeltaSeconds);
}

void USAC1AnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void USAC1AnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void USAC1AnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void USAC1AnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void USAC1AnimInstance::CollectPickUps()
{
	if (!IsValid(m_GrabItem))
	{
		return;
	}
	if (!Montage_IsPlaying(m_GrabItem))
	{
		Montage_Play(m_GrabItem, 2.f);
	}
}

void USAC1AnimInstance::ChangeWeapon()
{
	if (!IsValid(m_EquipChange))
	{
		return;
	}
	if (!Montage_IsPlaying(m_EquipChange))
	{
		Montage_Play(m_EquipChange, 1.f);
	}
}

void USAC1AnimInstance::HitReaction()
{
	if (!IsValid(m_DamagedMontage))
	{
		return;
	}
	if (!Montage_IsPlaying(m_DamagedMontage))
	{
		Montage_Play(m_DamagedMontage, 1.f);
	}
}

void USAC1AnimInstance::BaseVariableSetting()
{
	FVector velocity = m_Character->GetVelocity();
	m_Speed = velocity.Length();
	//m_Dir = UKismetAnimationLibrary::CalculateDirection(velocity, m_Character->GetActorRotation());
	UCharacterMovementComponent* moveComp = m_Character->GetCharacterMovement();
	m_IsAccerelating = moveComp->GetCurrentAcceleration().Length() > 0.f;
	//m_IsCrouching = m_Character->m_IsCrouching;
	m_IsInAir = moveComp->IsFalling();
	m_IsSprinting = m_Character->m_IsSprinting;
	m_IsADS = m_Character->GetIsADS();
}

void USAC1AnimInstance::AimOffset(float DeltaSeconds)
{
	FRotator curRot = FRotator(m_PitchInput, m_YawInput, 0.f);
	FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(
		m_Character->GetControlRotation(),m_Character->GetActorRotation());
	FRotator interpolate = FMath::RInterpTo(curRot, delta, DeltaSeconds, 20.f);
	m_PitchInput = FMath::ClampAngle(interpolate.Pitch, -90., 90.);
	m_YawInput = FMath::ClampAngle(interpolate.Yaw, -90., 90.);
}

void USAC1AnimInstance::TurnInChange()
{
	m_YawLastTick = m_Yaw;
	m_Yaw = m_Character->GetControlRotation().Yaw;
	m_YawFrameChange = m_YawLastTick - m_Yaw;
	if(IsAnyMontagePlaying()||m_IsAccerelating)
	{
		m_RootYawOffset = 0.f;
	}
	else
	{
		m_RootYawOffset = FRotator::NormalizeAxis(m_YawFrameChange+ m_RootYawOffset);
	}
	float curveValue = GetCurveValue(TEXT("IsTurning"));
	if(FMath::IsNearlyEqual(curveValue, 1.f,0.001f))
	{
		if(!m_DoOnce)
		{
			m_DoOnce = true;
			m_DistanceCurveValue = m_RootYawOffset;
		}
		m_DistanceCurveValueLastFrame = m_RootYawOffset;
		m_DistanceCurveValue = FMath::Clamp(GetCurveValue(TEXT("DistanceCurve")),-90.f,0.f);
		float mul = 1.f;
		if(m_RootYawOffset>0.f)
		{
			mul = -1.f;
		}
		m_RootYawOffset-=(m_DistanceCurveValueLastFrame - m_DistanceCurveValue)* mul;
	}
	else
	{
		m_DoOnce = false;
	}
}

void USAC1AnimInstance::SnapLHandToWeapon()
{
	UTP_WeaponComponent* weapon= m_Character->GetCurWeapon();
	if(!IsValid(weapon))
	{
		return;
	}
	FTransform weaponTransform = weapon->GetSocketTransform(TEXT("LeftHandPlacement"));
	FVector outLoc;
	FRotator outRot;
	m_Character->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), weaponTransform.GetLocation(), 
		weaponTransform.Rotator(), outLoc, outRot);
	m_LHandTransform.SetLocation(outLoc);
	m_LHandTransform.SetRotation(outRot.Quaternion());
}

void USAC1AnimInstance::WeaponSway(float DeltaSeconds)
{
	if (!m_IsADS)
	{
		float clamp = FMath::Clamp(m_Character->m_MoveRightValue * m_RHandRotIntensity, -7.f, 7.f);
		m_RHandRot = FMath::RInterpTo(m_RHandRot, FRotator(clamp, 0., 0.), DeltaSeconds, 6.f);
		return;
	}
	m_RHandRot = FMath::RInterpTo(m_RHandRot, FRotator::ZeroRotator, DeltaSeconds, 6.f);
}
