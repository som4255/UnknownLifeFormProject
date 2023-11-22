#include "Actor_PickUpWeapon.h"
#include "TP_WeaponComponent.h"
#include "AC_ItemState.h"
#include "SAC1Character.h"
#include "SAC1HUD.h"

TObjectPtr<UDataTable> AActor_PickUpWeapon::ItemDataTable;

AActor_PickUpWeapon::AActor_PickUpWeapon()
{
	SetReplicateMovement(true);

	m_Name = TEXT("Rifle");

	GetStaticMeshComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("NoCollision"));

	m_Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	SetRootComponent(m_Collider);
	m_Collider->SetGenerateOverlapEvents(true);
	m_Collider->SetCollisionProfileName(TEXT("PlayerTrigger"));
	m_Collider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	m_Collider->SetSphereRadius(100.f);

	m_Weapon = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
	m_Weapon->SetupAttachment(m_Collider);
	m_Weapon->SetCustomDepthStencilValue(19);

	m_ItemState = CreateDefaultSubobject<UAC_ItemState>(TEXT("ItemState"));
}

void AActor_PickUpWeapon::BeginPlay()
{
	Super::BeginPlay();
	m_Collider->OnComponentBeginOverlap.AddDynamic(this, &AActor_PickUpWeapon::OverlapBegin);
	m_Collider->OnComponentEndOverlap.AddDynamic(this, &AActor_PickUpWeapon::OverlapEnd);

	m_HUD = Cast<ASAC1HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

void AActor_PickUpWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetName(m_Name);
}

void AActor_PickUpWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AActor_PickUpWeapon::WasCollected()
{
	Super::WasCollected();
	if(m_IsActive)
	{
		return;
	}
	m_Collider->SetGenerateOverlapEvents(false);
	m_Collider->SetCollisionProfileName(TEXT("NoCollision"));
}

bool AActor_PickUpWeapon::PickedUpBy(APawn* pawn)
{
	ASAC1Character* player = Cast<ASAC1Character>(pawn);
	if (IsValid(player) && m_ItemState->GetItemData()->ItemKind == EItem::Weapon)
	{
		m_Weapon->SetName(m_Name);
		m_IsActive= !m_Weapon->TryAttachWeapon(player);
	}
	Super::PickedUpBy(pawn);
	return !m_IsActive;
}

FItemData* AActor_PickUpWeapon::FindItemData(const FName& Name)
{	
	return ItemDataTable->FindRow<FItemData>(Name, TEXT(""));
}

void AActor_PickUpWeapon::OverlapBegin(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index, bool bFromSweep, const FHitResult& result)
{
	m_HUD->SetInteractionText(ESlateVisibility::Visible, m_ItemState->GetItemData()->UIText);
	m_Weapon->SetCustomDepthStencilValue(15);
}

void AActor_PickUpWeapon::OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index)
{
	m_HUD->SetInteractionText(ESlateVisibility::Collapsed, m_ItemState->GetItemData()->UIText);
	m_Weapon->SetCustomDepthStencilValue(19);
}

void AActor_PickUpWeapon::LoadItemData()
{
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_ItemData.DT_ItemData"));
}

void AActor_PickUpWeapon::SetName(const FName& name)
{
	Super::SetName(name);
	if (!IsValid(ItemDataTable))
	{
		LoadItemData();
	}
	if (!IsValid(ItemDataTable))
	{
		return;
	}
	FItemData* data = FindItemData(m_Name);
	if (!data)
	{
		return;	
	}
	m_ItemState->SetItemInfo(m_Name, data);
	m_Weapon->SetName(m_Name);
	m_Weapon->SetSkeletalMesh(m_ItemState->GetItemData()->ItemSkeletalMesh);
}
