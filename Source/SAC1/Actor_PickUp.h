#pragma once
#include "GameInfo.h"
#include "Engine/StaticMeshActor.h"
#include "Actor_PickUp.generated.h"

UCLASS()
class SAC1_API AActor_PickUp : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AActor_PickUp();
protected:
	virtual void BeginPlay() override;
public:
	virtual void OnConstruction(const FTransform& Transform);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Pickup")
	APawn* m_PickUpInstigator;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pickup")
	TObjectPtr<UParticleSystemComponent> m_Particle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	FName		m_Name;

	UPROPERTY(ReplicatedUsing = "OnRep_GetActive")
	bool m_IsActive;

private:
	//�Ʒ�ó�� �Լ������ڸ� �Է��ϸ� _Implementation�ٿ��� �Ѵ�
	UFUNCTION(NetMulticast, Unreliable)
	void ClientOnPickedUpBy(APawn* pawn);

protected:
	UFUNCTION()
	virtual void OnRep_GetActive();

public:
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool GetActive();
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool state);

	virtual void SetName(const FName& name);

	//UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	virtual void WasCollected();
	//���� �Լ��� �������Ʈ ,c++���� ���ÿ� ȣ���ϰ� �����ϴ� �Լ�
	//���߿� 1���� �ۼ��ؾ� �Ѵ�.
	//virtual void WasCollected_Implementation();
	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	virtual bool PickedUpBy(APawn* pawn);
};
