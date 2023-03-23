#include "Effect.h"

//void Effect::DeleteCell()
//{
//	this->Recycle();
//}

E_Effect Effect::GetEffectType()
{
	return m_effectType;
}

void Effect::OnCellRecycle()
{
	BaseCell::OnCellRecycle();
	Animation* animation =(Animation*) GetComponent(E_Component::Component_Animation);
	if (animation != NULL) {
		animation->PauseAnim(true);
	}
}

void Effect::OnCellInit()
{
	BaseCell::OnCellInit();
	Animation* animation = (Animation*)GetComponent(E_Component::Component_Animation);
	if (animation != NULL) {
		animation->PauseAnim(false);
	}
}

Effect::Effect()
{
}

Effect::~Effect()
{
}

void Effect::StartEffect()
{
	SwitchAnim(E_Anim::Anim_Idle, true, false);
}

EffectHit::EffectHit()
{
	cellType = E_CellType::CellType_Effect_Hit;
	m_effectType = E_Effect::Effect_Hit;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(character_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\effectAniSheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_HIT_START_INDEX_X, TEX_EFFECT_HIT_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_W_COUNT;
	effectdrawData->tex_texel_size = effect_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_HIT_START_INDEX_Y * TEX_EFFECT_W_COUNT + TEX_EFFECT_HIT_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_HIT_ANIM_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, false);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, false);
	anim->animEnd_Callback = GameMgr::GetInstance()->effectMgr->hitEffect_end_cb;
	anim->SetAnimState(false);

}

EffectHit::~EffectHit()
{
}

EffectJump::EffectJump()
{
	cellType = E_CellType::CellType_Effect_Jump;
	m_effectType = E_Effect::Effect_Jump;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(jump_effct_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\effectAniSheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_JUMP_DUST_START_INDEX_X, TEX_EFFECT_JUMP_DUST_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_JUMP_CELL_W_COUNT;
	effectdrawData->tex_texel_size = effect_jump_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_JUMP_DUST_START_INDEX_Y * TEX_EFFECT_JUMP_CELL_W_COUNT + TEX_EFFECT_JUMP_DUST_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_JUMP_DUST_ANIM_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, false);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, false);
	anim->animEnd_Callback = GameMgr::GetInstance()->effectMgr->hitEffect_end_cb;
	anim->SetAnimState(false);
}

EffectJump::~EffectJump()
{
}

EffectRush::EffectRush()
{
	cellType = E_CellType::CellType_Effect_Hit;
	m_effectType = E_Effect::Effect_Hit;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(rush_effect_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\effectAniSheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_RUSH_START_INDEX_X, TEX_EFFECT_RUSH_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_JUMP_CELL_W_COUNT;
	effectdrawData->tex_texel_size = effect_jump_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_RUSH_START_INDEX_Y * TEX_EFFECT_JUMP_CELL_W_COUNT + TEX_EFFECT_RUSH_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_RUSH_ANIM_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.15f, true, false);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, false);
	anim->animEnd_Callback = GameMgr::GetInstance()->effectMgr->hitEffect_end_cb;
	anim->SetAnimState(false);
}

EffectRush::~EffectRush()
{
}

EffectSweat::EffectSweat()
{
	cellType = E_CellType::CellType_Effect_Sweat;
	m_effectType = E_Effect::Effect_Sweat;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(character_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\playerAniSheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_RICE_SMALL_START_INDEX_X, TEX_EFFECT_RICE_SMALL_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_PLAYER_ANIM_CELL_W_COUNT;
	effectdrawData->tex_texel_size = character_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_RICE_SMALL_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_EFFECT_RICE_SMALL_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_RICE_SMALL_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.7f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, true);

}

EffectSweat::~EffectSweat()
{
}


EffectRecover::EffectRecover()
{
	cellType = E_CellType::CellType_Effect_Recover;
	m_effectType = E_Effect::Effect_Recover;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_1);

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(character_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\recoverAni_sheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_RECOVER_START_INDEX_X, TEX_EFFECT_RECOVER_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_RECOVER_W_COUNT;
	effectdrawData->tex_texel_size = effect_recover_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_RECOVER_START_INDEX_Y * TEX_EFFECT_RECOVER_W_COUNT + TEX_EFFECT_RECOVER_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_RECOVER_ANIM_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 1.5f, true, false);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, false);
	anim->animEnd_Callback = GameMgr::GetInstance()->effectMgr->hitEffect_end_cb;
	anim->SetAnimState(false);
}

EffectRecover::~EffectRecover()
{
}


EffectSakura::EffectSakura()
{
	cellType = E_CellType::CellType_Effect_Sakura;
	m_effectType = E_Effect::Effect_Sakura;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));
	SetDrawLayer(E_DrawLayer::DrawLayer_Top);

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(sakuraEff_size);
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\Sakura-Sheet.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_SAKURA_START_INDEX_X, TEX_EFFECT_SAKURA_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_SAKURA_CELL_W_COUNT;
	effectdrawData->tex_texel_size = sakura_cell_tex_size;

	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = 0;
	int endTexIndex = startTexIndex + TEX_EFFECT_SAKURA_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_SakuraEff, vector2s(startTexIndex, endTexIndex), SAKURA_SPEED, true, true);
	animation->RegisterAnim(E_Anim::Anim_SakuraEff, anim, true);
}

EffectButton::EffectButton()
{
	cellType = E_CellType::CellType_Effect_Button;
	m_effectType = E_Effect::Effect_Button;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_1);

	DrawData* effectdrawData = new DrawData();
	effectdrawData->shakeWithCamera = true;
	effectdrawData->SetSize(vector2s(100, 100));
	effectdrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\button.png"));
	effectdrawData->texIndex = vector2s(TEX_EFFECT_BUTTON_START_INDEX_X, TEX_EFFECT_BUTTON_START_INDEX_Y);
	effectdrawData->tex_w_count = TEX_EFFECT_BUTTON_W_COUNT;
	effectdrawData->tex_texel_size = effect_button_cell_tex_size;
	SetDrawData(effectdrawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* anim = new AnimClip();
	int startTexIndex = TEX_EFFECT_BUTTON_START_INDEX_Y * TEX_EFFECT_BUTTON_W_COUNT + TEX_EFFECT_BUTTON_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_EFFECT_BUTTON_ANIM_FRAME_COUNT;
	anim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, anim, false);
	anim->animEnd_Callback = GameMgr::GetInstance()->effectMgr->hitEffect_end_cb;
	anim->SetAnimState(false);
}

EffectButton::~EffectButton()
{
}

