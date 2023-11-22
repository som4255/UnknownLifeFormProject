#pragma once
#include "GameInfo.h"
#include "Animation/AnimInstance.h"
#include "SAC1AnimInstance.generated.h"

UCLASS()
class SAC1_API USAC1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	USAC1AnimInstance();
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);
	virtual void NativePostEvaluateAnimation();
	virtual void NativeUninitializeAnimation();
	virtual void NativeBeginPlay();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage>	m_GrabItem;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage>	m_EquipChange;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage>	m_DamagedMontage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	TObjectPtr<class ASAC1Character> m_Character;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	ECharacterEquip m_CharState;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	float m_Speed;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	bool m_IsSprinting;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Anim", meta = (AllowPrivateAccess = true))
	bool m_IsInAir;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Offset", meta = (AllowPrivateAccess = true))
	float m_PitchInput;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Offset", meta = (AllowPrivateAccess = true))
	float m_YawInput;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_TurnInPlaceLimit;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_YawFrameChange;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_YawLastTick;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_Yaw;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_RootYawOffset;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_DistanceCurveValueLastFrame;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	float m_DistanceCurveValue;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	bool m_IsAccerelating;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TurnInChange", meta = (AllowPrivateAccess = true))
	bool m_DoOnce;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	FTransform m_LHandTransform;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	FVector m_RHandEffectorLoc;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	FVector m_RHandJointTargetLoc;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	FRotator m_RHandRot;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float m_RHandRotIntensity;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	float m_LHandIK;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	bool m_IsADS;

public:
	void SetCharacterState(ECharacterEquip state)
	{
		m_CharState = state;
	}
	ECharacterEquip GetCharacterState()
	{
		return m_CharState;
	}

	void CollectPickUps();
	void ChangeWeapon();
	void HitReaction();
	
	void BaseVariableSetting();
	void AimOffset(float DeltaSeconds);
	void TurnInChange();
	void SnapLHandToWeapon();
	void WeaponSway(float DeltaSeconds);

	//UFUNCTION()
	//void AnimNotify_TransitionFall();
};
