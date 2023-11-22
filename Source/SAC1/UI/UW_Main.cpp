#include "UW_Main.h"
#include "../SAC1GameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/Border.h"
//#include "Blueprint/WidgetBlueprintGeneratedClass.h"
//#include "Animation/WidgetAnimation.h"

void UUW_Main::NativeConstruct()
{
	Super::NativeConstruct();
    m_ReloadingImage = Cast<UImage>(GetWidgetFromName(TEXT("ReloadingImage")));

	m_RestartBtn = Cast<UButton>(GetWidgetFromName(TEXT("RestartBtn")));
	m_RestartBtn->OnClicked.AddDynamic(this, &UUW_Main::RestartBtnClick);

	m_ExitBtn = Cast<UButton>(GetWidgetFromName(TEXT("ExitBtn")));
	m_ExitBtn->OnClicked.AddDynamic(this, &UUW_Main::ExitBtnClick);

	m_InfoText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoText")));
	m_ArmoText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ArmoText")));
	m_InteractText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InteractText")));

	m_ArmoBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ArmoBorder")));
}

void UUW_Main::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(m_ReloadingImage->GetVisibility()==ESlateVisibility::Visible)
	{
		float angle= m_ReloadingImage->GetRenderTransformAngle()+InDeltaTime *200.f;
		angle = (int)angle%360;
		m_ReloadingImage->SetRenderTransformAngle(angle);
	}
}

void UUW_Main::RestartBtnClick()
{
	UGameplayStatics::OpenLevel(GetWorld(), *UGameplayStatics::GetCurrentLevelName(GetWorld()));
	USAC1GameInstance* instance = GetWorld()->GetGameInstance<USAC1GameInstance>();
	if (IsValid(instance))
	{
		instance->PlayerHP = instance->PlayerHPMax;
	}
}

void UUW_Main::ExitBtnClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(),
		EQuitPreference::Quit, true);
}

void UUW_Main::SetReloadingImage(ESlateVisibility visible)
{
	m_ReloadingImage->SetVisibility(visible);
}

void UUW_Main::SetPlayerDeadUI(ESlateVisibility visible)
{
	m_RestartBtn->SetVisibility(visible);
	m_ExitBtn->SetVisibility(visible);
}

void UUW_Main::SetWeaponUI(ESlateVisibility visible, const FName& name, int32 curArmo, int32 totalArmo)
{
	m_ArmoBorder->SetVisibility(visible);
	FString temp = name.ToString() + TEXT(" : ") + FString::FromInt(curArmo) + TEXT(" / ");
	if(totalArmo==-1)
	{
		temp+= TEXT("INF");
	}
	else
	{
		temp += FString::FromInt(totalArmo);
	}
	FText text = FText::FromString(temp);
	m_ArmoText->SetText(text);
}

void UUW_Main::SetMainUIText(int32 progress, const FString& text)
{
	m_InfoText->SetText(FText::FromString(text));
}

void UUW_Main::SetInteractionText(ESlateVisibility visible, const FName& name)
{
	m_InteractText->SetVisibility(visible);
	m_InteractText->SetText(FText::FromName(name));
}
