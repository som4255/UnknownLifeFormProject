#include "Actor_PickUp.h"

AActor_PickUp::AActor_PickUp()
	: m_IsActive(false)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	//if (GetLocalRole() == ROLE_Authority)
	//{
		m_IsActive = true;
	//}

	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	m_Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	m_Particle->SetupAttachment(GetRootComponent());
}

void AActor_PickUp::BeginPlay()
{
	Super::BeginPlay();
}

void AActor_PickUp::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AActor_PickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(AActor_PickUp, m_IsActive);
}

void AActor_PickUp::ClientOnPickedUpBy_Implementation(APawn* pawn)
{
	m_PickUpInstigator = pawn;
	WasCollected();
}

void AActor_PickUp::OnRep_GetActive()
{

}

bool AActor_PickUp::GetActive()
{
	return m_IsActive;
}

void AActor_PickUp::SetActive(bool state)
{
	//if(GetLocalRole() == ROLE_Authority)
	//{
		m_IsActive = state;
	//}
}

void AActor_PickUp::SetName(const FName& name)
{
	m_Name = name;
}

void AActor_PickUp::WasCollected()
{
	UE_LOG(LogTemp, Log, TEXT("WasCollected_Implementation : %s"), *GetName());
}

bool AActor_PickUp::PickedUpBy(APawn* pawn)
{
	//if(GetLocalRole()==ROLE_Authority)
	//{
		m_PickUpInstigator = pawn;
		ClientOnPickedUpBy(pawn);
	//}
	return true;
}
