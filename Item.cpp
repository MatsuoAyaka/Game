#include "Item.h"

Onigiri::Onigiri()
{
	cellType = E_CellType::CellType_Onigiri;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f,0.0f,0.0f), true);

	DrawData* onigiriDrawData = new DrawData();
	onigiriDrawData->shakeWithCamera = true;
	onigiriDrawData->SetSize(item_size);
	onigiriDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	onigiriDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_ONIGIRI_START_INDEX_X, TEX_ENVIRONMENT_ITEM_ONIGIRI_START_INDEX_Y);
	onigiriDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	onigiriDrawData->tex_texel_size = environment_item_cell_tex_size;
	this->SetDrawData(onigiriDrawData);

	Collider* onigiriCollider = new Collider();
	ColliderData* onigiriColliderData = new ColliderData(0, onigiriCollider, false, E_ColliderType::Collider_Rect,
		onigiriDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	onigiriCollider->AddColliderData(onigiriColliderData);

	this->RegisterComponent(E_Component::Component_Collider, onigiriCollider, true);

}

Onigiri::Onigiri(vector3s pos,vector2s size)
{
	cellType = E_CellType::CellType_Onigiri;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(pos, true);

	DrawData* onigiriDrawData = new DrawData();
	onigiriDrawData->shakeWithCamera = true;
	onigiriDrawData->SetSize(size);
	onigiriDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\onigiri.png"));
	onigiriDrawData->usePreComputeUV = true;
	onigiriDrawData->preComputeUV = vector2s(0, 0);
	onigiriDrawData->preComputeUVSize = vector2s(1, 1);
	onigiriDrawData->tex_texel_size = vector2s(1, 1);
	onigiriDrawData->SetColor(onigiri_color);
	this->SetDrawData(onigiriDrawData);

	Collider* onigiriCollider = new Collider();
	ColliderData* onigiriColliderData = new ColliderData(0, onigiriCollider, false, E_ColliderType::Collider_Rect,
		onigiriDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	onigiriCollider->AddColliderData(onigiriColliderData);

	this->RegisterComponent(E_Component::Component_Collider, onigiriCollider, true);
	
}

Onigiri::~Onigiri()
{
}

void Onigiri::DelOnigiri()
{
	//this->RemoveComponent(E_Component::Component_Collider);

	//22.11.26--temp
	this->SetPos(vector3s(this->GetTransform()->GetPos().x, -1000, 0.0f), true);
	this->SetState(false);

}

void Onigiri::Reset()
{
	DrawData* drawData = GetDrawData();
	Collider* iceCollider = (Collider*)GetComponent(E_Component::Component_Collider);
	drawData->SetSize(const_size_item);
	iceCollider->SetColliderSize(const_size_item, 0);
}

Sugar::Sugar()
{
	cellType = E_CellType::CellType_Sugar;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);

	DrawData* sugarDrawData = new DrawData();
	sugarDrawData->shakeWithCamera = true;
	sugarDrawData->SetSize(item_size);
	sugarDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	sugarDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_SUGAR_START_INDEX_X, TEX_ENVIRONMENT_ITEM_SUGAR_START_INDEX_Y);
	sugarDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	sugarDrawData->tex_texel_size = environment_item_cell_tex_size;
	this->SetDrawData(sugarDrawData);

	Collider* sugarCollider = new Collider();
	ColliderData* sugarColliderData = new ColliderData(0, sugarCollider, false, E_ColliderType::Collider_Rect,
		sugarDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	sugarCollider->AddColliderData(sugarColliderData);

	this->RegisterComponent(E_Component::Component_Collider, sugarCollider, true);
}

Sugar::~Sugar()
{
}

void Sugar::DelSugar()
{

}

void Sugar::Reset()
{
	BaseCell::Reset();
	DrawData* drawData = GetDrawData();
	Collider* iceCollider = (Collider*)GetComponent(E_Component::Component_Collider);
	drawData->SetSize(const_size_item);
	iceCollider->SetColliderSize(const_size_item, 0);
}

Pepper::Pepper()
{
	cellType = E_CellType::CellType_Pepper;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);

	DrawData* pepperDrawData = new DrawData();
	pepperDrawData->shakeWithCamera = true;
	pepperDrawData->SetSize(item_size);
	pepperDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	pepperDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_PEPPER_START_INDEX_X, TEX_ENVIRONMENT_ITEM_PEPPER_START_INDEX_Y);
	pepperDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	pepperDrawData->tex_texel_size = environment_item_cell_tex_size;
	this->SetDrawData(pepperDrawData);

	Collider* pepperCollider = new Collider();
	ColliderData* pepperColliderData = new ColliderData(0, pepperCollider, false, E_ColliderType::Collider_Rect,
		pepperDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	pepperCollider->AddColliderData(pepperColliderData);

	this->RegisterComponent(E_Component::Component_Collider, pepperCollider, true);
}

Pepper::~Pepper()
{
}

void Pepper::DelPepper()
{
	this->SetPos(vector3s(this->GetTransform()->GetPos().x, -1000, 0.0f), true);
	this->SetState(false);
}

void Pepper::Reset()
{
	BaseCell::Reset();
	DrawData* drawData = GetDrawData();
	Collider* iceCollider = (Collider*)GetComponent(E_Component::Component_Collider);
	drawData->SetSize(const_size_item);
	iceCollider->SetColliderSize(const_size_item, 0);
}

PickledPlums::PickledPlums()
{
	cellType = E_CellType::CellType_Pickled_Plums;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);

	DrawData* pickledPlumsDrawData = new DrawData();
	pickledPlumsDrawData->shakeWithCamera = true;
	pickledPlumsDrawData->SetSize(item_size);
	pickledPlumsDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	pickledPlumsDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_PICKLED_PLUMS_START_INDEX_X, TEX_ENVIRONMENT_ITEM_PICKLED_PLUMS_START_INDEX_Y);
	pickledPlumsDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	pickledPlumsDrawData->tex_texel_size = environment_item_cell_tex_size;
	this->SetDrawData(pickledPlumsDrawData);

	Collider* pickledPlumsCollider = new Collider();
	ColliderData* pickledPlumsColliderData = new ColliderData(0, pickledPlumsCollider, false, E_ColliderType::Collider_Rect,
		pickledPlumsDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	pickledPlumsCollider->AddColliderData(pickledPlumsColliderData);

	this->RegisterComponent(E_Component::Component_Collider, pickledPlumsCollider, true);
}

PickledPlums::~PickledPlums()
{
}

void PickledPlums::DelPickledPlums()
{
	this->SetPos(vector3s(this->GetTransform()->GetPos().x, -1000, 0.0f), true);
	this->SetState(false);
}

MisoSoup::MisoSoup()
{
	cellType = E_CellType::CellType_Miso_Soup;
	//m_use = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);

	DrawData* misoSoupDrawData = new DrawData();
	misoSoupDrawData->shakeWithCamera = true;
	misoSoupDrawData->SetSize(item_size);
	misoSoupDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	misoSoupDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_MISO_SOUP_START_INDEX_X, TEX_ENVIRONMENT_ITEM_MISO_SOUP_START_INDEX_Y);
	misoSoupDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	misoSoupDrawData->tex_texel_size = environment_item_cell_tex_size;
	this->SetDrawData(misoSoupDrawData);

	Collider* misoSoupCollider = new Collider();
	ColliderData* misoSoupColliderData = new ColliderData(0, misoSoupCollider, false, E_ColliderType::Collider_Rect,
		misoSoupDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	misoSoupCollider->AddColliderData(misoSoupColliderData);

	this->RegisterComponent(E_Component::Component_Collider, misoSoupCollider, true);
}

MisoSoup::~MisoSoup()
{
}

void MisoSoup::DelMisoSoup()
{
	this->SetPos(vector3s(this->GetTransform()->GetPos().x, -1000, 0.0f), true);
	this->SetState(false);
}

void MisoSoup::Reset()
{
	BaseCell::Reset();
	DrawData* drawData = GetDrawData();
	Collider* iceCollider = (Collider*)GetComponent(E_Component::Component_Collider);
	drawData->SetSize(const_size_item);
	iceCollider->SetColliderSize(const_size_item, 0);
}
