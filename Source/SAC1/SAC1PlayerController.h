#pragma once
#include "GameInfo.h"
#include "InputModifiers.h"
#include "GameFramework/PlayerController.h"
#include "SAC1PlayerController.generated.h"

UCLASS()
class SAC1_API ASAC1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASAC1PlayerController();
protected:
	virtual void OnConstruction(const FTransform& Transform);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> m_IMC;

public:
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_MouseLClick;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_MouseMClick;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_MouseRClick;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_MouseWheel;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_MousePos;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_E;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_R;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_F;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_Space;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_Move;
	UPROPERTY(VisibleAnywhere, Category = Input)
	TObjectPtr<class UInputAction> m_LShift;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UInputAction> AIHold;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UInputAction> AIFollow;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UInputAction> AIRecon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UInputAction> ToggleCheat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UInputAction> WeaponSlot;


private:
	static void MoveModifier(UInputMappingContext* IMC, UInputAction* IA, FKey key,
		bool isNegate = false, bool isSwizzle = false, EInputAxisSwizzle swizzleOrder = EInputAxisSwizzle::YXZ);
	static void NumModifier(UInputMappingContext* IMC, UInputAction* IA, FKey key, FVector vec);

public:
	void SetNewController();
};
