#pragma once
#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "DecalEffect.generated.h"

UCLASS()
class SAC1_API ADecalEffect : public AActor
{
	GENERATED_BODY()
public:	
	ADecalEffect();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(Category = Component, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDecalComponent>	mDecal;

	TObjectPtr<UMaterialInstanceDynamic>	mDecalMaterial;

	float	mTime;
	float	m_FadeTime;

	UPROPERTY(Category = Data, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool	mFadeEnable;

public:
	void SetFaceEnable(bool Enable)
	{
		mFadeEnable = Enable;
	}
	void SetFadeTime(float fadeTime)
	{
		m_FadeTime = fadeTime;
	}
	void SetDecalSize(FVector extent)
	{
		mDecal->DecalSize = extent;
	}
	void SetDecalMaterial(const FString& Path);
	void SetDecalMaterial(UMaterialInterface* material);
};
