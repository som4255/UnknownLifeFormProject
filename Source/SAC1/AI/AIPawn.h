 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Pawn.h"
#include "AIPawn.generated.h"

UCLASS()
class SAC1_API AAIPawn : public APawn
{
	GENERATED_BODY()

public:
	AAIPawn();

protected:
	static TObjectPtr<UDataTable>	mAIDataTable;

public:
	static void LoadAIData();

protected:
	static FAIDataTable* FindAIData(const FName& Name);

protected:
	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent>	mBody;

	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent>	mHead;

	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent>	mMesh;

	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement>	mMovement;

	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIState>	mAIState;


	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName		mName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInstance> mBloodDecal;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> m_BloodFill;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<USoundBase>> mRandomDeadSound;

	TObjectPtr<class UDefaultAIAnimInstance>	mAnim;

	// 생성된 스폰포인트를 알고 있게 해준다.
	// 스폰포인트를 이용해서 생성한 객체가 아닐 경우 None이 들어가있다.
	TObjectPtr<class AAISpawnPoint>	mSpawnPoint;

	TArray<TObjectPtr<UMaterialInstanceDynamic>>	mMaterialArray;

	bool	mAttackEnd;
	bool	mInteractionEnd;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effect", meta = (AllowPrivateAccess = true))
	bool    mDeath;

	ETeam		mTeam;

	UPROPERTY(Category = Component, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class APatrolPoint>>	mPatrolPointArray;

	TArray<FVector>		mPatrolPoint;
	int32				mPatrolIndex;

	bool				mHit;
	FTimerHandle		mHitTimerHandle;

	UPROPERTY(Category = Component, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float				mDissolveTime;

	float				mDissolveCurrentTime;
	bool				mDissolveEnable;

	FVector m_TargetLoc;

public:
	bool GetPatrolEnable()	const
	{
		return !mPatrolPointArray.IsEmpty();
	}

	const FVector& GetPatrolLocation()
	{
		return mPatrolPoint[mPatrolIndex];
	}

	ETeam GetTeam()
	{
		return mTeam;
	}

	void SetTeam(ETeam Team)
	{
		mTeam = Team;
	}

	const FVector GetTargetLoc()	const
	{
		return m_TargetLoc;
	}
	void SetTargetLoc(FVector vec)
	{
		m_TargetLoc = vec;
	}

	bool GetAttackEnd()	const
	{
		return mAttackEnd;
	}

	bool GetInteractionEnd()	const
	{
		return mInteractionEnd;
	}

	const TObjectPtr<class UAIState>& GetAIState()	const
	{
		return mAIState;
	}

	const TObjectPtr<class UDefaultAIAnimInstance>& GetAIAnimInstance()	const
	{
		return mAnim;
	}

	float GetHalfHeight()	const
	{
		return mBody->GetScaledCapsuleHalfHeight();
	}

	float GetCapsuleRadius()	const
	{
		return mBody->GetScaledCapsuleRadius();
	}

	void SetSpawnPoint(class AAISpawnPoint* SpawnPoint);

	void SetAttackEnd(bool AttackEnd)
	{
		mAttackEnd = AttackEnd;
	}

	void SetInteractionEnd(bool InteractionEnd)
	{
		mInteractionEnd = InteractionEnd;
	}

	void SetPatrolPointArray(const TArray<TObjectPtr<class APatrolPoint>>& Array);
	void NextPatrol();

protected:
	virtual void OnConstruction(const FTransform& Transform);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 생성한 객체가 제거될때 호출된다.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser);

	private:
		UFUNCTION()
		void BodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetCollisionProfile(const FName& Name);

	UFUNCTION()
	void HitTimer();

	UFUNCTION(BlueprintCallable)
	virtual void Attack();
	void DeathEnd();
};
