#pragma once
#include "GameInfo.h"
#include "GameFramework/Actor.h"
#include "SAC1Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ASAC1Projectile : public AActor
{
	GENERATED_BODY()
public:
	ASAC1Projectile();
	virtual void OnConstruction(const FTransform& Transform);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	static TObjectPtr<UDataTable>	m_ProjectTileDataTable;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> CollisionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> m_ProjectileMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAC_Projectile>	m_ProjectileState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	FName		m_Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	float		m_Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEvent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> m_HitEmitter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEvent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> m_HitDecalMaterial;

private:
	void FireBottle();
	
protected:
	static FProjectileData* FindProjectileData(const FName& Name);

public:
	static void LoadProjectileData();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void SetName(const FName& name);
	void SetProjectileMesh(USkeletalMesh* mesh);
	void SetProjectileParticle(UParticleSystem* particle);
	void SetProjectileDecal(UMaterialInstance* decal);
	void SetProjectileDamage(float damage);

	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	USkeletalMeshComponent* GetProjectileMesh() const { return m_ProjectileMesh; }
};

