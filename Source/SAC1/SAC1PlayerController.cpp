#include "SAC1PlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PCM_FPS.h"

ASAC1PlayerController::ASAC1PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerCameraManagerClass = APCM_FPS::StaticClass();
}

void ASAC1PlayerController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FInputModeGameOnly	InputMode;

	SetInputMode(InputMode);
}

void ASAC1PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASAC1PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASAC1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	m_IMC = NewObject<UInputMappingContext>(this);
	m_MouseLClick = NewObject<UInputAction>(this);
	m_MouseLClick->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_MouseLClick, EKeys::LeftMouseButton);
	m_MouseMClick = NewObject<UInputAction>(this);
	m_MouseMClick->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_MouseMClick, EKeys::MiddleMouseButton);
	m_MouseRClick = NewObject<UInputAction>(this);
	m_MouseRClick->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_MouseRClick, EKeys::RightMouseButton);
	m_MouseWheel = NewObject<UInputAction>(this);
	m_MouseWheel->ValueType = EInputActionValueType::Axis1D;
	m_IMC->MapKey(m_MouseWheel, EKeys::MouseWheelAxis);
	m_MousePos = NewObject<UInputAction>(this);
	m_MousePos->ValueType = EInputActionValueType::Axis2D;
	m_IMC->MapKey(m_MousePos, EKeys::Mouse2D);
	m_E = NewObject<UInputAction>(this);
	m_E->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_E, EKeys::E);
	m_R = NewObject<UInputAction>(this);
	m_R->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_R, EKeys::R);
	m_F = NewObject<UInputAction>(this);
	m_F->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_F, EKeys::F);
	m_Space = NewObject<UInputAction>(this);
	m_Space->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_Space, EKeys::SpaceBar);
	m_LShift = NewObject<UInputAction>(this);
	m_LShift->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(m_LShift, EKeys::LeftShift);

	AIHold = NewObject<UInputAction>(this);
	AIHold->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(AIHold, EKeys::Z);
	AIFollow = NewObject<UInputAction>(this);
	AIFollow->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(AIFollow, EKeys::X);
	AIRecon = NewObject<UInputAction>(this);
	AIRecon->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(AIRecon, EKeys::C);

	ToggleCheat	= NewObject<UInputAction>(this);
	ToggleCheat->ValueType = EInputActionValueType::Boolean;
	m_IMC->MapKey(ToggleCheat, EKeys::P);

	WeaponSlot = NewObject<UInputAction>(this);
	WeaponSlot->ValueType = EInputActionValueType::Axis3D;
	//NumModifier(m_IMC, WeaponSlot, EKeys::Zero, FVector(0.));
	NumModifier(m_IMC, WeaponSlot, EKeys::One, FVector(1.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Two, FVector(2.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Three, FVector(3.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Four, FVector(4.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Five, FVector(5.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Six, FVector(6.));
	NumModifier(m_IMC, WeaponSlot, EKeys::Seven, FVector(7.));
	
	m_Move = NewObject<UInputAction>(this);
	m_Move->ValueType = EInputActionValueType::Axis3D;
	MoveModifier(m_IMC, m_Move, EKeys::W);
	MoveModifier(m_IMC, m_Move, EKeys::S, true);
	MoveModifier(m_IMC, m_Move, EKeys::A, true, true);
	MoveModifier(m_IMC, m_Move, EKeys::D, false, true);
}

void ASAC1PlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ASAC1PlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ASAC1PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASAC1PlayerController::MoveModifier(UInputMappingContext* IMC, UInputAction* IA, FKey key,
	bool isNegate, bool isSwizzle, EInputAxisSwizzle swizzleOrder)
{
	FEnhancedActionKeyMapping& mapping = IMC->MapKey(IA, key);
	UObject* outer = IMC->GetOuter();
	if (isNegate)
	{
		UInputModifierNegate* negate = NewObject<UInputModifierNegate>(outer);
		mapping.Modifiers.Add(negate);
	}
	if (isSwizzle)
	{
		UInputModifierSwizzleAxis* swizzle = NewObject<UInputModifierSwizzleAxis>(outer);
		swizzle->Order = swizzleOrder;
		mapping.Modifiers.Add(swizzle);
	}
}

void ASAC1PlayerController::NumModifier(UInputMappingContext* IMC, UInputAction* IA, FKey key, FVector vec)
{
	FEnhancedActionKeyMapping& mapping = IMC->MapKey(IA, key);
	UObject* outer = IMC->GetOuter();
	UInputModifierScalar* scalar = NewObject<UInputModifierScalar>(outer);
	scalar->Scalar = vec;
	mapping.Modifiers.Add(scalar);
}

void ASAC1PlayerController::SetNewController()
{
	ULocalPlayer* localPlayer = GetLocalPlayer();
	if (IsValid(localPlayer))
	{
		UEnhancedInputLocalPlayerSubsystem* subSystem =
			localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (IsValid(subSystem))
		{
			subSystem->ClearAllMappings();
			subSystem->AddMappingContext(m_IMC, 0);
		}
	}
}