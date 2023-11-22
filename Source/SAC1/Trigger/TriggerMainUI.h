#pragma once
#include "Trigger.h"
#include "TriggerMainUI.generated.h"

UCLASS()
class SAC1_API ATriggerMainUI : public ATrigger
{
	GENERATED_BODY()
public:
	ATriggerMainUI();
protected:
	virtual void TriggerOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Trigger", meta = (AllowPrivateAccess = true))
	FString m_UIText;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Trigger", meta = (AllowPrivateAccess = true))
	int32 m_Progress;
};
