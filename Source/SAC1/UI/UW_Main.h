#pragma once
#include "../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "UW_Main.generated.h"

class UImage;
class UButton;
class UTextBlock;
class UBorder;

UCLASS()
class SAC1_API UUW_Main : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
private:
	UImage* m_ReloadingImage;
	UButton* m_RestartBtn;
	UButton* m_ExitBtn;
	UTextBlock* m_InfoText;
	UTextBlock* m_ArmoText;
	UTextBlock* m_InteractText;
	UBorder* m_ArmoBorder;

private:
	UFUNCTION()
	void RestartBtnClick();
	UFUNCTION()
	void ExitBtnClick();

public:
	UFUNCTION()
	void SetReloadingImage(ESlateVisibility visible);
	UFUNCTION()
	void SetPlayerDeadUI(ESlateVisibility visible);
	UFUNCTION()
	void SetWeaponUI(ESlateVisibility visible, const FName& name, int32 curArmo, int32 totalArmo);
	UFUNCTION()
	void SetMainUIText(int32 progress,const FString& text);
	UFUNCTION()
	void SetInteractionText(ESlateVisibility visible, const FName& name);
};
