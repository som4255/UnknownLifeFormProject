#pragma once
#include "Actor_PickUp.h"
#include "Actor_PickUpWeapon.generated.h"

UCLASS()
class SAC1_API AActor_PickUpWeapon : public AActor_PickUp
{
	GENERATED_BODY()
public:
	AActor_PickUpWeapon();
protected:
	virtual void BeginPlay() override;
public:
	virtual void OnConstruction(const FTransform& Transform);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void WasCollected() override;
	bool PickedUpBy(APawn* pawn) override;

private:
	TObjectPtr<class ASAC1HUD> m_HUD;

protected:
	static TObjectPtr<UDataTable>	ItemDataTable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> m_Collider;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pickup")
	TObjectPtr<class UTP_WeaponComponent> m_Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAC_ItemState>	m_ItemState;

protected:
	static FItemData* FindItemData(const FName& Name);

	UFUNCTION()
	virtual void OverlapBegin(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index, bool bFromSweep, const FHitResult& result);
	UFUNCTION()
	virtual void OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor,
		UPrimitiveComponent* otherComp, int32 index);

public:
	static void LoadItemData();

	virtual void SetName(const FName& name);
};
