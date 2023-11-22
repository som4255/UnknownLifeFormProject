#pragma once
#include "GameInfo.h"
#include "GameFramework/Character.h"
#include "SAC1Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class USAC1AnimInstance;
struct FInputActionValue;

UCLASS(config=Game)
class ASAC1Character : public ACharacter
{
	GENERATED_BODY()	
	friend USAC1AnimInstance;
public:
	ASAC1Character();
protected:
	virtual void OnConstruction(const FTransform& Transform);
	virtual void BeginPlay();
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Component")
	TObjectPtr<USpringArmComponent> m_SpringArm;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Component")
	TObjectPtr<UCameraComponent> m_Camera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	float m_MoveForwardValue;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	float m_MoveRightValue;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	float m_MaxWalkSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	float m_MaxSprintSpeed;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool m_IsSprinting;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool m_CanMove;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> m_DeadSounds;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInstance> m_HitMaterial;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> m_BloodFill;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	FVector m_StartCamRelativeLoc;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	FVector2D m_ScreenRotVec;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	float m_CameraSpeed;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	bool m_IsInvertX;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	bool m_IsInvertY;

	TArray<TObjectPtr<class UTP_WeaponComponent>> m_Weapons;
	TArray<TObjectPtr<UMaterialInstanceDynamic>>	m_MaterialArray;
	TObjectPtr<USAC1AnimInstance>	m_AnimInst;
	ETeam		m_Team;
	int m_CurWeaponIndex;
	int m_WeaponIndexDir;
	int m_TargetIndexForChangeWeapon;
	bool m_IsInvisible;
	
protected:
	UFUNCTION()
	virtual void BodyHit(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& result);
	UFUNCTION()
	virtual void OverlapBegin(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index, bool bFromSweep, const FHitResult& result);
	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index);

	void Move(const FInputActionValue& Value);
	void CameraRotation(const FInputActionValue& Value);
	void ChangeWeapon(const FInputActionValue& Value);
	void ChangeWeaponByNum(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void Sprint();
	void ToggleCheat();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void CollectPickUps();
public:
	ETeam GetTeam()
	{
		return m_Team;
	}
	void SetTeam(ETeam Team)
	{
		m_Team = Team;
	}

	UFUNCTION(BlueprintCallable, Category = Anim)
	void SetCharacterState(ECharacterEquip state);
	UFUNCTION(BlueprintCallable, Category = Anim)
	ECharacterEquip GetCharacterState();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetCurWeapon();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UTP_WeaponComponent* GetCurWeapon();

	bool TryAddWeapon(UTP_WeaponComponent* weapon, ECharacterEquip equip);
	void OnPlayerDeath();
	void PickUpArmo(ECharacterEquip equip,float value);

	bool GetIsSprinting(){ return m_IsSprinting; }
	bool GetIsADS();
	FVector GetStartCamRelativeLoc() { return m_StartCamRelativeLoc; }
	FVector2D GetScreenRotVec()	{return m_ScreenRotVec;}
	USpringArmComponent* GetSpringArmComponent() const { return m_SpringArm; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return m_Camera; }
};

