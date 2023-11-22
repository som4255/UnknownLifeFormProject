#pragma once
#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

UENUM(BlueprintType)
enum class EUserTriggerType : uint8
{
	Player,
	Enemy,
	AllObject
};

UCLASS()
class SAC1_API ATrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrigger();

protected:
	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent>	mBody;

	UPROPERTY(Category = Trigger, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EUserTriggerType	mTriggerType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Trigger", meta = (AllowPrivateAccess = true))
	bool m_DestroyWhenTrigger;

public:
	void SetTriggerType(EUserTriggerType Type)
	{
		mTriggerType = Type;

		switch (mTriggerType)
		{
		case EUserTriggerType::Player:
			mBody->SetCollisionProfileName(TEXT("PlayerTrigger"));
			break;
		case EUserTriggerType::Enemy:
			mBody->SetCollisionProfileName(TEXT("EnemyTrigger"));
			break;
		case EUserTriggerType::AllObject:
			mBody->SetCollisionProfileName(TEXT("ObjectTrigger"));
			break;
		}
	}

protected:
	virtual void OnConstruction(const FTransform& Transform);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent,	AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index);

protected:
	virtual void TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
	virtual void TriggerOverlapEnd(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index);
};
