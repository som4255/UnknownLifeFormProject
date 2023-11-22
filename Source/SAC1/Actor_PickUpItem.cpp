#include "Actor_PickUpItem.h"
#include "AC_ItemState.h"
#include "SAC1Character.h"
#include "SAC1HUD.h"

TObjectPtr<UDataTable> AActor_PickUpItem::ItemDataTable;

AActor_PickUpItem::AActor_PickUpItem()
{
	SetReplicateMovement(true);

	m_Name = TEXT("RiflePack");

	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	GetStaticMeshComponent()->bRenderCustomDepth = true;
	GetStaticMeshComponent()->SetCustomDepthStencilValue(19);

	m_Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	m_Collider->SetupAttachment(GetStaticMeshComponent());
	m_Collider->SetGenerateOverlapEvents(true);
	m_Collider->SetCollisionProfileName(TEXT("PlayerTrigger"));
	m_Collider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	m_Collider->SetSphereRadius(100.f);

	m_ItemState = CreateDefaultSubobject<UAC_ItemState>(TEXT("ItemState"));
}

void AActor_PickUpItem::BeginPlay()
{
	Super::BeginPlay();
	m_Collider->OnComponentBeginOverlap.AddDynamic(this, &AActor_PickUpItem::OverlapBegin);
	m_Collider->OnComponentEndOverlap.AddDynamic(this, &AActor_PickUpItem::OverlapEnd);

	m_HUD = Cast<ASAC1HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

void AActor_PickUpItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetName(m_Name);
}

void AActor_PickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AActor_PickUpItem::WasCollected()
{
	Super::WasCollected();
	SetLifeSpan(1.f);
}

bool AActor_PickUpItem::PickedUpBy(APawn* pawn)
{
	Super::PickedUpBy(pawn);
	ASAC1Character* player = Cast<ASAC1Character>(m_PickUpInstigator);	
	FItemData* data= m_ItemState->GetItemData();
	if(IsValid(player)&& data)
	{
		if(m_Name==FName(TEXT("Scrap"))) 
		{
			//ai È¸º¹
			//player->PickUpArmo(0, data->Value);
		}
		else if (m_Name == FName(TEXT("RiflePack")))
		{
			player->PickUpArmo(ECharacterEquip::Rifle, data->Value);
		}
		else if (m_Name == FName(TEXT("PistolPack")))
		{
			player->PickUpArmo(ECharacterEquip::Pistrol, data->Value);
		}
		else if (m_Name == FName(TEXT("FirePack")))
		{
			player->PickUpArmo(ECharacterEquip::FireBottle, data->Value);
		}
		else if (m_Name == FName(TEXT("FlarePack")))
		{
			player->PickUpArmo(ECharacterEquip::Flare, data->Value);
		}
		else if (m_Name == FName(TEXT("HealPack")))
		{
			player->PickUpArmo(ECharacterEquip::Food, data->Value);
		}
		else if (m_Name == FName(TEXT("GranadePack")))
		{
			player->PickUpArmo(ECharacterEquip::Granade, data->Value);
		}
		else if (m_Name == FName(TEXT("CardKey")))
		{
			UE_LOG(LogTemp, Warning, TEXT("adad"));
		}
	}
	m_IsActive = false;
	return !m_IsActive;
}

FItemData* AActor_PickUpItem::FindItemData(const FName& Name)
{
	return ItemDataTable->FindRow<FItemData>(Name, TEXT(""));
}

void AActor_PickUpItem::OverlapBegin(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index, bool bFromSweep, const FHitResult& result)
{
	m_HUD->SetInteractionText(ESlateVisibility::Visible, m_ItemState->GetItemData()->UIText);
	GetStaticMeshComponent()->SetCustomDepthStencilValue(15);
}

void AActor_PickUpItem::OverlapEnd(UPrimitiveComponent* comp, AActor* otherActor, 
	UPrimitiveComponent* otherComp, int32 index)
{
	m_HUD->SetInteractionText(ESlateVisibility::Collapsed, m_ItemState->GetItemData()->UIText);
	GetStaticMeshComponent()->SetCustomDepthStencilValue(19);
}

void AActor_PickUpItem::LoadItemData()
{
	ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_ItemData.DT_ItemData"));
}

void AActor_PickUpItem::SetName(const FName& name)
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
	GetStaticMeshComponent()->SetStaticMesh(m_ItemState->GetItemData()->ItemStaticMesh);
}
