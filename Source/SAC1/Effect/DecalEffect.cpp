#include "DecalEffect.h"

ADecalEffect::ADecalEffect()
{
	PrimaryActorTick.bCanEverTick = true;

	mDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));

	SetRootComponent(mDecal);
#if WITH_EDITORONLY_DATA
	mDecal->bVisualizeComponent = true;
#endif
	mDecal->DecalSize = FVector(128.0, 256.0, 256.0);
	mDecal->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));

	mTime = 0.f;
	m_FadeTime = 3.f;
	mFadeEnable = true;
}

void ADecalEffect::BeginPlay()
{
	Super::BeginPlay();
}

void ADecalEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mFadeEnable && IsValid(mDecalMaterial))
	{
		mTime += DeltaTime;
		if(mTime+ m_FadeTime > InitialLifeSpan)
		{
			float	opacity = (InitialLifeSpan - mTime) / m_FadeTime;
			mDecalMaterial->SetScalarParameterValue(TEXT("Opacity"), opacity);
		}
	}
}

void ADecalEffect::SetDecalMaterial(const FString& Path)
{
	UMaterialInterface* Material = LoadObject<UMaterialInterface>(this, *Path);

	if (IsValid(Material))
	{
		mDecal->SetDecalMaterial(Material);

		mDecalMaterial = mDecal->CreateDynamicMaterialInstance();
	}
}

void ADecalEffect::SetDecalMaterial(UMaterialInterface* material)
{
	if (IsValid(material))
	{
		mDecal->SetDecalMaterial(material);
		mDecalMaterial = mDecal->CreateDynamicMaterialInstance();
	}
}

