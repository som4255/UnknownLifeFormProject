#include "TriggerMainUI.h"
#include "../SAC1HUD.h"

ATriggerMainUI::ATriggerMainUI()
{
	m_DestroyWhenTrigger = true;
	mTriggerType = EUserTriggerType::Player;
	m_Progress = 0;
}

void ATriggerMainUI::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::TriggerOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ASAC1HUD* hud = Cast<ASAC1HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	if (IsValid(hud))
	{
		hud->SetMainUIText(m_Progress,m_UIText);
	}
}
