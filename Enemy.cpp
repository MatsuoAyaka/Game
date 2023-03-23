
#pragma once

#include "Player.h"
#include "Enemy.h"
#include "MapGenerator.h"
#include "EventCenter.h"
#include "Bullet.h"
#include "Scene.h"
#include "Effect.h"

static int monkeyMoveCount = 20;

#pragma region enemy
Enemy::Enemy(E_EnemyType eType):enemyType(eType), m_movement(vector2s(0.0f, 0.0f)), m_createPoint(vector3s(0.0f, 0.0f, 0.0f)), m_isGround(false)
{
	EventCenter::GetInstance()->AddEventListener(this, "EnemyContactBullet", &Enemy::Hurt);

	//EventCenter::GetInstance()->AddMultiEventListener(this, "EnemyContactBullet", &Enemy::Hurt);

	cellType = E_CellType::CellType_Enemy;
}
Enemy::~Enemy()
{
	EventCenter::GetInstance()->RemoveEventListener<Enemy, void, int>(this, "EnemyContactBullet");
	//EventCenter::GetInstance()->RemoveMultiEventListener<Enemy, void, int>("EnemyContactBullet");

	m_ground = NULL;
}
void Enemy::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);
}

void Enemy::SetParent(BaseCell* _parent)
{
	if (_parent != NULL && _parent->GetCellType() == E_CellType::CellType_Obstacle) {
		this->SetGroundBinded((Ground*)_parent);
	}
	BaseCell::SetParent(_parent);
}

void Enemy::Reset()
{
	BaseCell::Reset();
	this->Resize();
}

void Enemy::Resize()
{
	vector2s size = vector2s(0, 0);
	switch (this->enemyType) {
	//case E_CellType::CellType_Enemy_Dumpling:
	case E_EnemyType::Enemy_Hiyoko:
		size = const_size_enemy_dumpling;
		break;
	//case E_CellType::CellType_Enemy_Pasta:
	case E_EnemyType::Enemy_Monkey:
		size = const_size_enemy_pasta;
		break;
	//case E_CellType::CellType_Enemy_Cornet:
	case E_EnemyType::Enemy_Nezumi:
		size = const_size_enemy_cornet;
		break;
	//case E_CellType::CellType_Enemy_Ittanmomen:
	case E_EnemyType::Enemy_Karasu:
		size = const_size_enemy_ittanmomen;
		break;
	//case E_CellType::CellType_Boss:
	case E_EnemyType::Enemy_Boss:
		size = const_size_enemy_boss;
		break;
	default:
		return;
	}
	this->SetDrawDataSize(size);
	Collider* itemCollider = (Collider*)this->GetComponent(E_Component::Component_Collider);
	if (itemCollider != NULL && itemCollider->GetColliderData(0) != NULL) {
		itemCollider->GetColliderData(0)->size = size;
	}
}

void Enemy::Move()
{
}
void Enemy::Hurt(int _damage)
{
	m_hp -= _damage;
	if (m_hp <= 0)
		Die();
}
void Enemy::Die()
{
	this->SetPos(vector3s(this->GetTransform()->GetPos().x, -1000, 0.0f), true);
	m_status = ENEMY_STS_NONE;
	m_useGravityFlag = false;
	SetState(false);
	GameMgr::GetInstance()->uiMgr->EnemyCounterAdd();
}

#pragma endregion enemy

//==================================================================サル

#pragma region monkey

Monkey::Monkey():Enemy(E_EnemyType::Enemy_Monkey)
{
	cellType = E_CellType::CellType_Enemy;
	m_fallSpeed = 0.0f;
	//SetStatus(ENEMY_STS_PATROL);	//地面にいる
	//m_searchRange = ENEMY_SEARCHRANGE;	//索敵範囲の初期化

	m_status = ENEMY_STS_PATROL;	//猿の状態
	m_useFlag = true;
	m_hp = 2;
	m_damage = 1;
	m_useGravityFlag = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));
	DrawData* monkey_drawData = new DrawData();
	monkey_drawData->shakeWithCamera = true;
	monkey_drawData->SetSize(character_size);
	monkey_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnemyAniSheet.png"));
	monkey_drawData->texIndex = vector2s(TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_X, TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_Y);
	monkey_drawData->tex_w_count = TEX_ENEMY_CELL_W_COUNT;
	monkey_drawData->tex_texel_size = enemy_cell_tex_size;
	SetDrawData(monkey_drawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENEMY_PASTA_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_ENEMY_PASTA_ANIM_MOVE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_PASTA_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_ENEMY_PASTA_ANIM_IDLE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	Collider* monkey_collider = new Collider();
	ColliderData* monkey_colliderData =
		new ColliderData(0, monkey_collider, false, E_ColliderType::Collider_Rect, monkey_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	monkey_collider->AddColliderData(monkey_colliderData);
	RegisterComponent(E_Component::Component_Collider, monkey_collider, true);

}

void Monkey::DoUpdate(float deltatime) {

	if (m_status == E_EnemyStatus::ENEMY_STS_NONE)return;

	BaseCell::DoUpdate(deltatime);
	PatrolRangeUpdate();
	//vector2s movement = vector2s(0, 0);

	//======================== 23.1 パスタ星人のAIを調整する必要があるので、今はひたすらジャンプさせるだけ
	vector3s tempPos = this->GetTransform()->GetPos();
	bool playerMovementSign = false;

	vector3s playerPos;
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerPos = player->GetTransform()->GetPos();

	Gravity();

	//vector3s curPos_2 = this->GetTransform()->GetPos();
	//curPos_2.x += m_movement.x;
	//curPos_2.y += m_movement.y;

	////AdjustScreenPoint(curPos, this->GetSize(true));	//当たり判定
	////AdjustGroundPos(curPos_2);
	//this->SetPos(curPos_2, true);

	//GameMgr::GetInstance()->sceneObjMgr->CheckMonkeyCollision(this);

	//return;
	//======================== 

	switch (m_status)
	{
	case ENEMY_STS_PATROL:
		//Patrol(&movement, &tempPos, 750, 550);	//パトロール
		Patrol();
		//Search(&tempPos, playerPos, m_searchRange);
		if (Search(playerPos))
		{
			m_muki = playerPos.x > tempPos.x ? false : true;

			m_isGround = false;
			m_fallSpeed = m_jumpPower;
			m_movement.y = m_fallSpeed;
			m_fallAcc = ENEMY_GRAVITY * 10;
			m_status = ENEMY_STS_JUMP;
		}
		else
		{
			m_fallAcc = ENEMY_GRAVITY * 3;
		}

		break;

	case ENEMY_STS_JUMP:
		if (Search(playerPos))
		{
			if (m_isGround)
				Jump();

			m_muki = playerPos.x > tempPos.x ? false : true;
			m_movement.x = m_muki ? -ENEMY_MAX_MOVE_SPEED : ENEMY_MAX_MOVE_SPEED;
		}
		else if (!Search(playerPos) && m_isGround)
		{
			m_status = ENEMY_STS_PATROL;
		}

		//GameMgr::GetInstance()->sceneObjMgr->CheckMonkeyCollision(this);
		break;
	case ENEMY_STS_MOVETOPLAYER:
		//Invoke(switch to Jump, t seconds);
		monkeyMoveCount--;
		if (monkeyMoveCount < 0)
		{
			monkeyMoveCount = 20;

			m_muki = playerPos.x > tempPos.x ? false : true;

			m_isGround = false;
			m_fallSpeed = m_jumpPower;
			m_movement.y = m_fallSpeed;
			m_status = ENEMY_STS_JUMP;
		}
		break;
	case ENEMY_STS_FALL:
		m_movement.x = -ENEMY_MOVEMENT_SPEED;
		m_movement.y = m_fallSpeed;

		GameMgr::GetInstance()->sceneObjMgr->CheckMonkeyCollision(this);
		break;

	case ENEMY_STS_GONE:
		Gone(&tempPos, playerPos, &m_movement, this);
		break;
	}

	if (m_movement.x > 0) {
		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	}
	else if (m_movement.x < 0) {
		this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
	}
	//Patrol(&movement, tempCell, &tempPos, 600, 150);	//パトロール

	//if (movement.x > 0) {
	//	this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	//}
	//else if (movement.x < 0) {
	//	this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
	//}

	//Jump();	//ジャンプ
	//Gravity(&movement, tempCell);	//重力

	vector3s curPos = this->GetTransform()->GetPos();
	curPos.x += m_movement.x;
	curPos.y += m_movement.y;
	
	//AdjustScreenPoint(curPos, this->GetSize(true));	//当たり判定
	AdjustGroundPos(curPos);
	this->SetPos(curPos, true);

	m_movement = vector2s(0.0f, 0.0f);

	GameMgr::GetInstance()->sceneObjMgr->CheckMonkeyCollision(this);
}

void Monkey::Search(vector3s* tempPos, vector3s playerPos, float searchRange)
{	//プレイヤーが索敵範囲に入る時
	if (playerPos.x >= tempPos->x - searchRange)
	{
		m_status = ENEMY_STS_JUMP;
	}
}

bool Monkey::Search(vector3s playerPos)
{
	vector3s tempPos = GetTransform()->GetPos();

	//プレイヤーが索敵範囲に入る時
	//if (VecDistS(playerPos, tempPos) < m_searchRange)
	//{
	//	m_muki = playerPos.x > tempPos.x ? false : true;

	//	m_isGround = false;
	//	m_fallSpeed = m_jumpPower;
	//	m_movement.y = m_fallSpeed;
	//	m_status = ENEMY_STS_JUMP;
	//}

		//プレイヤーが索敵範囲に入る時
	if (VecDistS(playerPos, tempPos) < m_searchRange)
	{
		return true;
	}

	return false;
}

void Monkey::Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l)
{//	actRange_r：右の行動限界　actRange_l:左の行動限界

	if (m_muki == false || GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) //右向き
	{
		movement->x = ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x > actRange_r)		//右の行動限界を超えたら
			m_muki = true;					//左向きにする
	}

	if (m_muki == true || GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) //左向き
	{
		movement->x = -ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x < actRange_l)		//左の行動限界を超えたら
			m_muki = false;					//右向きにする
	}
}

void Monkey::Patrol()
{//m_patrolRange 行動限界
	vector3s tempPos = transform->GetPos();
	if (m_muki == false) //右向き
	{
		m_movement.x = ENEMY_MAX_MOVE_SPEED;
		//if (tempPos.x > m_createPoint.x && VecDistS(m_createPoint, tempPos) > m_patrolRange)		//右の行動限界を超えたら
		if ((tempPos.x > m_createPoint.x) && tempPos.x - m_createPoint.x > m_patrolRange)//右の行動限界を超えたら
		{
			m_muki = true;					//左向きにする
		}
		return;
	}

	if (m_muki == true) //左向き
	{
		m_movement.x = -ENEMY_MAX_MOVE_SPEED;
		//if (tempPos.x < m_createPoint.x && VecDistS(m_createPoint, tempPos) > m_patrolRange)		//左の行動限界を超えたら
		if ((tempPos.x < m_createPoint.x) && m_createPoint.x - tempPos.x > m_patrolRange)//左の行動限界を超えたら	
		{
			m_muki = false;					//右向きにする
		}
		return;
	}
}

void Monkey::PatrolRangeUpdate()
{
	m_ground = (Ground*)m_parent;
	if (m_ground != nullptr && m_status == ENEMY_STS_PATROL)
	{
		//int rangeX = m_ground->GetSize(true).x;
		//int rangeY = m_ground->GetSize(true).y;
		//m_patrolRange = rangeX * rangeX;
		//m_createPoint = m_ground->GetTransform()->GetPos();
		//m_createPoint.y -= (rangeY / 2);
		//m_createPoint.y -= (this->GetSize(true).y / 2);

		m_patrolRange = m_ground->GetSize(true).x * 0.5f - this->GetSize(true).x * 0.5f - 10.0f;//パトロールの範囲（ｘだけを使う、ブロックの位置を中心として
		m_createPoint = m_ground->GetTransform()->GetPos();//今いるブロックの位置
	}
}

//void Monkey::Gravity(vector2s* movement, BaseCell* tempCell)
//{
//	vector3s tempPos = this->GetTransform()->GetPos();
//	tempPos.y = this->GetTransform()->GetPos().y + 2.0f;
//	GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
//	if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() && GetJumppower() >= 0)
//	{	//地面にいたら
//		SetStatus(STS_NORMAL);		//statusを地面にする
//		SetJumppower(0);			//重力の影響を受けない
//	}
//	else //地面から離れていたら（ジャンプしてたら）
//	{
//		SetStatus(STS_JUMP);							//statusをジャンプにする
//		SetJumppower(GetJumppower() + GetGravity());	//重力の影響を受ける
//	}
//	movement->y += GetJumppower();
//}

void Monkey::Gravity()
{
	//空中にいたら,重力をかける
	if (!m_isGround && m_useGravityFlag)
	{
		m_fallSpeed += m_fallAcc;

		m_movement.y += m_fallSpeed;
	}
	else {
		m_movement.y = 0.0f;
		m_fallSpeed = 0.0f;
	}
}

void Monkey::AdjustGroundPos(vector3s pos)
{
	if (m_ground == nullptr)
		return;
	vector3s groundPos = m_ground->GetTransform()->GetPos();
	vector2s groundSize = m_ground->GetSize(true);
	float xLeft = groundPos.x - groundSize.x * 0.5f + 25.0f;
	float xRight = groundPos.x + groundSize.x * 0.5f - 25.0f;

	vector3s monkeyPos = transform->GetPos();
	if (pos.x < xLeft)
		monkeyPos.x = xLeft;
	if (pos.x > xRight)
		monkeyPos.x = xRight;

	SetPos(monkeyPos, true);
	//if(pos.x - size.x * 0.5f < )
}

void Monkey::Jump()
{
	//if (GetStatus() == STS_NORMAL)		//地面にいたら
	//{		
	//	SetJumppower(ENEMY_JUMPPOWER);
	//	SetStatus(STS_JUMP);			//ジャンプさせる
	//}

	//tempPos->y = this->GetTransform()->GetPos().y + 2.0f;
	//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(*tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
	//if (m_isGround == true) {
	//	m_movement.y = m_jumpPower;
	//	m_fallSpeed = 0.0f;
	//	//m_movement.x = ENEMY_MOVEMENT_SPEED * (m_muki ? -1 : 1);
	//	m_isGround = false;
	//}
	/*if (tempPos->y <= SCREEN_HEIGHT / 2)
	{
		tempPos->y = SCREEN_HEIGHT / 2;
		m_status = ENEMY_STS_FALL;
	}*/

	m_movement.y = m_jumpPower;
	m_fallSpeed = m_jumpPower;
	//m_movement.x = ENEMY_MOVEMENT_SPEED * (m_muki ? -1 : 1);
	m_isGround = false;
}

void Monkey::Gone(vector3s* tempPos, vector3s playerPos, vector2s* movement, BaseCell* monkey)
{
	movement->x = -ENEMY_MAX_MOVE_SPEED;
	movement->y = 0;

	//if (tempPos->x <= playerPos.x - 200.0f)
	//{
	//	this->m_useFlag = false;
	//	SetStatus(ENEMY_STS_NONE);
	//	monkey->SetState(false);
	//}
}

#pragma endregion monkey

//==================================================================ひよこ
#pragma region hiyoko
Hiyoko::Hiyoko():Enemy(E_EnemyType::Enemy_Hiyoko)
{
	m_dirFactor = 0;
	cellType = E_CellType::CellType_Enemy;
	m_damage = 1;
	m_useGravityFlag = true;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));
	DrawData* hiyoko_drawData = new DrawData();
	hiyoko_drawData->shakeWithCamera = true;
	hiyoko_drawData->SetSize(character_size);
	hiyoko_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnemyAniSheet.png"));
	//hiyoko_drawData->usePreComputeUV = true;
	//hiyoko_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//hiyoko_drawData->preComputeUVSize = character_size;

	hiyoko_drawData->texIndex = vector2s(TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_X, TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_Y);
	hiyoko_drawData->tex_w_count = TEX_ENEMY_CELL_W_COUNT;
	hiyoko_drawData->tex_texel_size = enemy_cell_tex_size;
	SetDrawData(hiyoko_drawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENEMY_DUMPLING_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_ENEMY_DUMPLING_ANIM_MOVE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_ENEMY_DUMPLING_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_ENEMY_DUMPLING_ANIM_IDLE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	//===================コライダー
	Collider* hiyoko_collider = new Collider();
	ColliderData* hiyoko_colliderData =
		new ColliderData(0, hiyoko_collider, false, E_ColliderType::Collider_Rect, hiyoko_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	hiyoko_collider->AddColliderData(hiyoko_colliderData);
	RegisterComponent(E_Component::Component_Collider, hiyoko_collider, true);
}

Hiyoko::Hiyoko(int _tempMethod) :Enemy(E_EnemyType::Enemy_Hiyoko)
{
	m_dirFactor = -1;
	cellType = E_CellType::CellType_Enemy;
	m_damage = 1;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));
	DrawData* hiyoko_drawData = new DrawData();
	hiyoko_drawData->shakeWithCamera = true;
	hiyoko_drawData->SetSize(character_size);
	hiyoko_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnemyAniSheet.png"));
	//hiyoko_drawData->usePreComputeUV = true;
	//hiyoko_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//hiyoko_drawData->preComputeUVSize = character_size;

	hiyoko_drawData->texIndex = vector2s(TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_X, TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_Y);
	hiyoko_drawData->tex_w_count = TEX_ENEMY_CELL_W_COUNT;
	hiyoko_drawData->tex_texel_size = enemy_cell_tex_size;
	SetDrawData(hiyoko_drawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENEMY_CORNET_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_ENEMY_CORNET_ANIM_MOVE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	//===================コライダー
	Collider* hiyoko_collider = new Collider();
	ColliderData* hiyoko_colliderData =
		new ColliderData(0, hiyoko_collider, false, E_ColliderType::Collider_Rect, hiyoko_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	hiyoko_collider->AddColliderData(hiyoko_colliderData);
	RegisterComponent(E_Component::Component_Collider, hiyoko_collider, true);
}

void Hiyoko::DoUpdate(float deltatime)
{
	if (m_status == E_EnemyStatus::ENEMY_STS_NONE)return;

	BaseCell::DoUpdate(deltatime);
	PatrolRangeUpdate();
	//vector2s movement = vector2s(0, 0);
	vector3s tempPos = this->GetTransform()->GetPos();

	//BaseCell* tempCell = NULL;
	//tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();

	//Patrol(&movement, &tempPos, 500, 150);	//パトロール
	Patrol();
	//Gravity(&movement);						//重力処理
	Gravity();

	/*if (m_muki) 
		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	else if (!m_muki)
		this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));*/
	transform->SetRot(m_muki ? vector3s(0.0f + 180.0f * m_dirFactor, 0.0f, 0.0f) : vector3s(-180.0f - 180.0f * m_dirFactor, 0.0f, 0.0f));

	vector3s curPos = transform->GetPos();
	curPos.x += m_movement.x;
	curPos.y += m_movement.y;

	if ((int)(VecLength(m_movement) + 0.5f) != 0) {
		SwitchAnim(E_Anim::Anim_Move, true, false);
	}
	else {
		SwitchAnim(E_Anim::Anim_Idle, true, false);
	}
	//AdjustScreenPoint(curPos, this->GetSize(true));
	this->SetPos(curPos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckHiyokoCollision(this);
}

void Hiyoko::Reset()
{
	m_status = ENEMY_STS_PATROL;
	Enemy::Reset();
}


void Hiyoko::Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l)
{//	actRange_r：右の行動限界　actRange_l:左の行動限界

	if (m_muki == false || GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) //右向き
	{
		movement->x = ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x > actRange_r)		//右の行動限界を超えたら
			m_muki = true;					//左向きにする
	}

	if (m_muki == true || GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) //左向き
	{
		movement->x = -ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x < actRange_l)		//左の行動限界を超えたら
			m_muki = false;					//右向きにする
	}
}

void Hiyoko::Patrol()
{
	vector3s tempPos = transform->GetPos();

	if (m_muki == false) //右向き
	{
		m_movement.x = ENEMY_MAX_MOVE_SPEED;
		//if (tempPos.x > m_createPoint.x && VecDistS(m_createPoint, tempPos) > m_patrolRange)	//右の行動限界を超えたら
		if((tempPos.x > m_createPoint.x) && tempPos.x-m_createPoint.x > m_patrolRange)//右の行動限界を超えたら
		{
			m_muki = true;					//左向きにする
			return;
		}
		
	}

	if (m_muki == true) //左向き
	{
		m_movement.x = -ENEMY_MAX_MOVE_SPEED;
		//if (tempPos.x < m_createPoint.x && VecDistS(m_createPoint, tempPos) > m_patrolRange)	//左の行動限界を超えたら
		if ((tempPos.x < m_createPoint.x) &&  m_createPoint.x - tempPos.x > m_patrolRange)//左の行動限界を超えたら
		{
			m_muki = false;					//右向きにする
			return;
		}
	}
}

void Hiyoko::PatrolRangeUpdate()
{
	m_ground = (Ground*)m_parent;
	if (m_ground != nullptr)
	{
		//int rangeX = m_ground->GetSize(true).x - 500;
		//int rangeY = m_ground->GetSize(true).y;
		//m_patrolRange = rangeX * rangeX;
		//m_createPoint = m_ground->GetTransform()->GetPos();
		//m_createPoint.y -= (rangeY / 2);
		//m_createPoint.y -= (this->GetSize(true).y / 2);

		m_patrolRange = m_ground->GetSize(true).x * 0.5f - this->GetSize(true).x * 0.5f - 10.0f;//パトロールの範囲（ｘだけを使う、ブロックの位置を中心として
		m_createPoint = m_ground->GetTransform()->GetPos();//今いるブロックの位置
	}
}


void Hiyoko::Gravity(vector2s* movement)
{
	movement->y = 5.0f;	//重力をかける（簡易的に）
}

void Hiyoko::Gravity()
{
	//空中にいたら,重力をかける
	if (!m_isGround && m_useGravityFlag)
	{
		m_fallSpeed += m_fallAcc;

		m_movement.y = m_fallSpeed;
	}
	else
	{
		m_movement.y = m_fallSpeed = 0;
	}
}

#pragma endregion hiyoko

//==================================================================カラス
#pragma region Karasu
Karasu::Karasu():Enemy(E_EnemyType::Enemy_Karasu)
{
	cellType = E_CellType::CellType_Enemy;
	m_status = ENEMY_STS_PATROL;
	m_searchRange = ENEMY_SEARCHRANGE;


	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));
	DrawData* Karasu_drawData = new DrawData();
	Karasu_drawData->shakeWithCamera = true;
	Karasu_drawData->SetSize(character_size);
	Karasu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnemyAniSheet.png"));
	//Karasu_drawData->usePreComputeUV = true;
	//Karasu_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//Karasu_drawData->preComputeUVSize = character_size;

	Karasu_drawData->texIndex = vector2s(TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_X, TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_Y);
	Karasu_drawData->tex_w_count = TEX_ENEMY_CELL_W_COUNT;
	Karasu_drawData->tex_texel_size = enemy_cell_tex_size;
	SetDrawData(Karasu_drawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_ENEMY_ITTANMOMEN_ANIM_MOVE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_ITTANMOMEN_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	//===================コライダー
	Collider* Karasu_collider = new Collider();
	ColliderData* Karasu_colliderData =
		new ColliderData(0, Karasu_collider, false, E_ColliderType::Collider_Rect, Karasu_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	Karasu_collider->AddColliderData(Karasu_colliderData);
	RegisterComponent(E_Component::Component_Collider, Karasu_collider, true);
}

void Karasu::DoUpdate(float deltatime)
{
	if (m_status == E_EnemyStatus::ENEMY_STS_NONE)return;

	BaseCell::DoUpdate(deltatime);

	vector2s movement = vector2s(0, 0);
	vector3s tempPos = this->GetTransform()->GetPos();

	BaseCell* tempCell = NULL;
	tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();

	vector3s playerPos;
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerPos = player->GetTransform()->GetPos();

	switch (m_status)
	{
	case ENEMY_STS_PATROL:
		Patrol(&movement, &tempPos, 900, 750);	//パトロール

		if (movement.x > 0) {
			this->transform->SetRot(vector3s(180.0f, 0.0f, 0.0f));
		}
		else if (movement.x < 0) {
			this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
		}

		Search(&tempPos, playerPos, m_searchRange);
		break;

	case ENEMY_STS_ATTACK:
		ContactAtk(tempPos, playerPos, &movement);
		GameMgr::GetInstance()->sceneObjMgr->CheckKarasuCollision(this);
		break;

	case ENEMY_STS_GONE:
		Gone(&tempPos, playerPos, &movement, this);
		break;
	default:
		this->SetState(false);
		return;
	}

	/*if (GetWithinRange() == false)							//範囲外にいる間は
	{
		Patrol(&movement, tempCell, &tempPos, 900, 750);	//パトロール
		if (playerPos.x > tempPos.x - 150)		//プレイヤーが範囲内に入ったら　（→今は範囲内を[カラスのポジション-100］のところに設定中）
			SetWithinRange(true);				//フラグをtrueにする
	}
	if (GetWithinRange() == true)							//範囲内に入ったら
	{
		contactAtk(tempPos, &movement);						//突進してくる
	}

	if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) // プレイヤーと当たる時
	{
		Gone(&tempPos, playerPos, &movement);
	}

	//movement = VecNormalize(movement)*ENEMY_MAX_MOVE_SPEED;

	if (movement.x > 0)
		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	else if (movement.x < 0)
		this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
	*/

	vector3s curPos = this->GetTransform()->GetPos();
	curPos.x += movement.x;
	curPos.y += movement.y;

	//AdjustScreenPoint(curPos, this->GetSize(true));
	this->SetPos(curPos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckKarasuCollision(this);

}

void Karasu::Reset()
{
	Enemy::Reset();

	//パトロールパラメータを更新
	m_defaultPos = this->GetTransform()->GetPos();
	m_partolRange.x = m_defaultPos.x - m_partolRangeRadius;
	m_partolRange.y = m_defaultPos.x + m_partolRangeRadius;
}

void Karasu::Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l)
{
	//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(*tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
	//if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {

	//}
	if (m_muki == false) //右向き
	{
		movement->x = ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x > m_partolRange.y)		//右の行動限界を超えたら
			m_muki = true;					//左向きにする
		return;
	}

	if (m_muki == true || GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision()) //左向き
	{
		movement->x = -ENEMY_MAX_MOVE_SPEED;
		if (tempPos->x < m_partolRange.x)		//左の行動限界を超えたら
			m_muki = false;					//右向きにする
		return;
	}
}

void Karasu::Search(vector3s* tempPos, vector3s playerPos, float searchRange)
{
	if (playerPos.x >= tempPos->x - searchRange)
	{
		m_status = ENEMY_STS_ATTACK;
	}
}


void Karasu::Gone(vector3s* tempPos, vector3s playerPos, vector2s* movement, BaseCell* karasu)
{
	movement->x = -ENEMY_MAX_MOVE_SPEED;
	movement->y = -ENEMY_MAX_MOVE_SPEED;

	if (tempPos->x <= playerPos.x - 200.0f)
	{
		this->m_useFlag = false;
		m_status = ENEMY_STS_NONE;
		//Temp
		//今ステージの中のすべてのものが強制的に表示するという処理があるので、StateをFalseにするではなく、画面の外に移動させる
		//karasu->SetState(false);
		vector3s curPos = this->GetTransform()->GetPos();
		curPos.y -= SCREEN_HEIGHT;
		this->SetPos(curPos, true);
	}
}

void Karasu::ContactAtk(vector3s tempPos, vector3s playerPos, vector2s* movement)	//突進処理
{
	vector3s temp;
	temp = playerPos - tempPos;
	VecNormalize(temp);
	temp *= 2.0f;
	movement->x = temp.x / 60;
	movement->y = temp.y / 60;

}

#pragma endregion karasu

#pragma nezumi
Nezumi::Nezumi():Enemy(E_EnemyType::Enemy_Nezumi)
{
	cellType = E_CellType::CellType_Npc;
	m_damage = 1;

	m_status = ENEMY_STS_NONE;
	m_movement = vector2s(0, 0);
	m_hp = 100;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(150.0f, 500.0f, 0.0f), true);
	SetRot(vector3s(180.0f, 0.0f, 0.0f));
	DrawData* nezumi_drawData = new DrawData();
	nezumi_drawData->shakeWithCamera = true;
	nezumi_drawData->SetSize(character_size);
	nezumi_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnemyAniSheet.png"));
	//nezumi_drawData->usePreComputeUV = true;
	//nezumi_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//nezumi_drawData->preComputeUVSize = character_size;

	nezumi_drawData->texIndex = vector2s(TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_X, TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_Y);
	nezumi_drawData->tex_w_count = TEX_ENEMY_CELL_W_COUNT;
	nezumi_drawData->tex_texel_size = enemy_cell_tex_size;
	SetDrawData(nezumi_drawData);

	//===================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENEMY_CORNET_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_Y * TEX_ENEMY_CELL_W_COUNT + TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_ENEMY_CORNET_ANIM_MOVE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, false);
	animation->DoInit();

	m_effect = GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Button, NULL, vector3s(0, 0, 0), vector3s(1, 0, 0));
	m_effect->SetState(false);

	//===================コライダー
	Collider* nezumi_collider = new Collider();
	ColliderData* nezumi_colliderData =
		new ColliderData(0, nezumi_collider, false, E_ColliderType::Collider_Rect, nezumi_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	nezumi_collider->AddColliderData(nezumi_colliderData);
	RegisterComponent(E_Component::Component_Collider, nezumi_collider, true);
}

void Nezumi::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);
	//return;
	//vector2s movement = vector2s(0, 0);
	vector3s tempPos = this->GetTransform()->GetPos();

	BaseCell* tempCell = NULL;
	tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();

	vector3s playerPos;
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerPos = player->GetTransform()->GetPos();
	vector3s effectPos = vector3s(playerPos.x, playerPos.y - 120.0f, playerPos.z);

	SwitchAnim(E_Anim::Anim_Move, true, true);

	switch (m_status)
	{
	case ENEMY_STS_NONE:
		if (playerPos.x - 90.0f > tempPos.x + ENEMY_NEZUMI_CHASERANGE)
		{

			m_status = ENEMY_STS_PATROL;
		}
		m_effect->SetState(false);
		break;

	case ENEMY_STS_PATROL:
		m_movement.x = ENEMY_NEZUMI_SPEED;		//追跡
		if (tempPos.y > playerPos.y)
		{
			m_movement.y = -1;
		}
		else if (tempPos.y < playerPos.y)
		{
			m_movement.y = 1;
		}
		else
		{
			m_movement.y = 0;
		}
		if (tempPos.x >= playerPos.x - 90.0f)
		{

			this->SetPos(playerPos, true);
			m_movement.x = 0;
			//m_hitWait = 10;
			m_status = ENEMY_STS_ATTACK;

			m_effect->SetPos(effectPos, true);
			m_effect->SetState(true);
		}
		break;

	case ENEMY_STS_ATTACK:

		this->SetPos(vector3s(player->GetTransform()->GetPos().x - 90.0f, player->GetTransform()->GetPos().y, 0.0f), true);
		m_effect->SetPos(vector3s(player->GetTransform()->GetPos().x, player->GetTransform()->GetPos().y - 140.0f, 0.0f), true);


		if (m_hitWait-- <= 0)
		{
			EventCenter::GetInstance()->EventTrigger<int>("EnemyContactDmg", this->GetDamage());
			m_hitWait = 60;

			if (player->GetHitAwayCounter() >= 10)
			{
				player->SetHitAwayCounyer(0);

				//this->SetPos(vector3s(player->GetTransform()->GetPos()), true);
				gone = true;
				m_effect->SetState(false);
				m_status = ENEMY_STS_GONE;
			}
		}

		break;

	case ENEMY_STS_GONE:
		//SetDrawLayer(E_DrawLayer::DrawLayer_Scene_1);

		vector3s temp = this->GetTransform()->GetPos();
		float tempX = abs(player->GetTransform()->GetPos().x - temp.x);
		/*this->SetPos(vector3s(temp.x,temp.y, 0.0f), true);
		if (goneTime <= 0)
		{
			gone = false;
		}*/
		m_movement.x = -10;


		if (tempX >= 1500)
		{
			m_movement.x = 0;

			m_status = ENEMY_STS_NONE;
		}
		break;
	}

	
	//Chase(tempPos, &movement);		//追跡
	//Gravity(&movement, tempCell);	//重力


	if (m_movement.x > 0)
		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	else if (m_movement.x < 0)
		this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));

	vector3s curPos = this->GetTransform()->GetPos();
	curPos.x += m_movement.x;
	curPos.y += m_movement.y;

	//AdjustScreenPoint(curPos, this->GetSize(true));
	this->SetPos(curPos, true);

	float tempX = abs(player->GetTransform()->GetPos().x - this->GetTransform()->GetPos().x);

	if (tempX > 1500)
	{
		this->SetPos(vector3s(player->GetTransform()->GetPos().x - 1500, player->GetTransform()->GetPos().y, player->GetTransform()->GetPos().z), true);
	}

	GameMgr::GetInstance()->sceneObjMgr->CheckNezumiCollision(this);

}

//void Nezumi::Chase(vector3s tempPos, vector2s* movement)
//{
//	vector3s temp;
//	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
//	temp = player->GetTransform()->GetPos() - this->GetTransform()->GetPos();
//	VecNormalize(temp);
//	temp *= 1.5f;
//	movement->x = temp.x / 60;
//	movement->y = temp.y / 60;
//}

//void Nezumi::Gravity(vector2s* movement, BaseCell* tempCell)
//{
//	movement->y += 5.0f;	//重力をかける（簡易的）
//}


#pragma endregion nezumi

//===========================================================
//	以下　当たり判定	＜GameLogic.h   126行～129行＞
//	void CheckMonkeyCollision(Monkey* monkey);	//猿
//	void CheckHiyokoCollision(Hiyoko* hiyoko);	//ひよこ
//	void CheckKarasuCollision(Karasu* karasu);	//カラス
//	void CheckNezumiCollision(Nezumi* nezumi);	//ネズミ

//===========================
//	猿の当たり判定
//===========================

void SceneObjMgr::CheckMonkeyCollision(Monkey* monkey)
{
	if (monkey == NULL) return;

	E_EnemyStatus status = (E_EnemyStatus)monkey->GetStatus();

	//障害物との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		bool tempSign = false;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(monkey, iter->second)) {
				Transform::AdjustPosition(monkey, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				//vector3s playerPos;
				//Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
				//playerPos = player->GetTransform()->GetPos();
				//vector3s tempPos = monkey->GetTransform()->GetPos();
				//if (iter->second->GetTransform()->GetPos().y - iter->second->GetSize(true).y / 2 > playerPos.y)
				//{
				//	monkey->SetIsGround(true);
				//}

				//switch (status)
				//{
				//case ENEMY_STS_JUMP:
				//	if (VecDistS(tempPos, playerPos) > monkey->GetSearchRange()) {
				//		monkey->SetStatus(E_EnemyStatus::ENEMY_STS_GONE);
				//	}
				//	else {
				//		monkey->SetStatus(E_EnemyStatus::ENEMY_STS_MOVETOPLAYER);
				//	}
				//	break;
				//case ENEMY_STS_MOVETOPLAYER:
				//	break;
				//default:
				//	break;
				//}

				vector2s obstacleSize = iter->second->GetSize(true);
				vector3s obstaclePos = iter->second->GetTransform()->GetPos();
				if (fabs(obstaclePos.x - monkey->GetTransform()->GetPos().x) < obstacleSize.x * 0.5f &&
					monkey->GetTransform()->GetPos().y < obstaclePos.y) {
					tempSign = true;
				}
			}
		}
		monkey->SetIsGround(tempSign);
		//if (tempSign == true) {
		//	monkey->Jump();
		//}
	}

	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Enemy)continue;
			if (((Player*)(iter->second))->CheckCanBeAttacked() == false)continue;

			if (Collider::CollisionDetect(monkey, iter->second)) {
				//Transform::AdjustPosition(monkey, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				EventCenter::GetInstance()->EventTrigger<int>("EnemyContactDmg", monkey->GetDamage());
				//monkey->SetStatus(E_EnemyStatus::ENEMY_STS_GONE);
			}
		}

	}
}

//===========================
//	ひよこの当たり判定
//===========================
void SceneObjMgr::CheckHiyokoCollision(Hiyoko* hiyoko) {
	if (hiyoko == NULL)return;

	//障害物との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {
		hiyoko->SetIsGround(false);

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(hiyoko, iter->second)) {
				Transform::AdjustPosition(hiyoko, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				hiyoko->SetIsGround(true);
			}
		}
	}
	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (((Player*)(iter->second))->CheckCanBeAttacked() == false)continue;

			if (Collider::CollisionDetect(hiyoko, iter->second)) {
				//Transform::AdjustPosition(iter->second, hiyoko->GetTransform()->GetPos(), hiyoko->GetColliderSize(true, 0), false);
				EventCenter::GetInstance()->EventTrigger<int>("EnemyContactDmg", hiyoko->GetDamage());
			}
		}

	}
}

//===========================
//	カラスの当たり判定
//===========================
void SceneObjMgr::CheckKarasuCollision(Karasu* karasu) {
	if (karasu == NULL) return;

	//障害物との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(karasu, iter->second)) {
				Transform::AdjustPosition(karasu, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
			}
		}

	}
	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (((Player*)(iter->second))->CheckCanBeAttacked() == false)continue;

			if (Collider::CollisionDetect(karasu, iter->second)) {
				//Transform::AdjustPosition(karasu, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				karasu->SetStatus(E_EnemyStatus::ENEMY_STS_GONE);
			}
		}

	}
}

//===========================
//	ねずみの当たり判定
//===========================
void  SceneObjMgr::CheckNezumiCollision(Nezumi* nezumi) {
	if (nezumi == NULL)return;

	//障害物との当たり判定
	/*
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(nezumi, iter->second)) {
				Transform::AdjustPosition(nezumi, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
			}
		}

	}
	*/
	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (((Player*)(iter->second))->CheckCanBeAttacked() == false)continue;

			if (Collider::CollisionDetect(nezumi, iter->second)) {
				//Transform::AdjustPosition(nezumi, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);

			}
		}

	}
}

//=====================================================================================================
//以下　松尾担当
#pragma region boss
Boss::Boss():Enemy(E_EnemyType::Enemy_Boss)
{
	//===============
	// 初期化
	//===============
	EventCenter::GetInstance()->AddEventListener(this, "BossContactBullet", &Boss::Hurt);
	cellType = E_CellType::CellType_Boss;
	m_hp = BOSS_HP;

	movement = vector2s(0, 0);

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(0.0f, 0.0f, 0.0f), true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));
	DrawData* boss_drawData = new DrawData();
	boss_drawData->shakeWithCamera = true;
	boss_drawData->SetSize(boss_size);
	boss_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\BossAniSheet.png"));

	boss_drawData->texIndex = vector2s(TEX_BOSS_ANIM_IDLE_START_INDEX_X, TEX_BOSS_ANIM_IDLE_START_INDEX_Y);
	boss_drawData->tex_w_count = TEX_BOSS_W_COUNT;
	boss_drawData->tex_texel_size = boss_cell_tex_size;
	SetDrawData(boss_drawData);

	//===================
	// アニメーション
	//===================
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_BOSS_ANIM_IDLE_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_BOSS_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_BOSS_ANIM_MOVE_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_BOSS_ANIM_MOVE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	AnimClip* flyAnim = new AnimClip();
	startTexIndex = TEX_BOSS_ANIM_FLY_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_FLY_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_BOSS_ANIM_FLY_FRAME_COUNT;
	flyAnim->SetAnimParam(E_Anim::Anim_Fly, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Fly, flyAnim, true);

	//===================
	// コライダー
	//===================
	vector2s size = boss_drawData->GetSize();
	Collider* boss_collider = new Collider();
	ColliderData* boss_colliderData =
		new ColliderData(0, boss_collider, false, E_ColliderType::Collider_Rect, vector2s(size.x,size.y*0.7f), vector3s(0.0f, size.y*0.3f, 0.0f));
	boss_collider->AddColliderData(boss_colliderData);
	RegisterComponent(E_Component::Component_Collider, boss_collider, true);

	//=========================
	// サウンドデータのロード
	//=========================
	char filename_DragonAtk[] = "data\\SE\\dragonAtk.wav";
	DragonAtkSoundNo = LoadSound(filename_DragonAtk);

	char filename_DragonFly[] = "data\\SE\\dragonFly.wav";
	DragonFlySoundNo = LoadSound(filename_DragonFly);

	char filename_DragonShot[] = "data\\SE\\dragonShot.wav";
	DragonShotSoundNo = LoadSound(filename_DragonShot);
}

Boss::~Boss()
{
	EventCenter::GetInstance()->RemoveEventListener<Enemy, void, int>(this, "BossContactBullet");
}

//================
// ダメージ処理
//================
void Boss::Hurt(int damage)	
{
	m_hp -= damage;
}

//==================
// 更新処理
//==================
void Boss::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);

	tempPos = this->GetTransform()->GetPos();

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if(player!=NULL){
	playerPos = player->GetTransform()->GetPrePos();
	}
	BaseCell* tempCell = NULL;
	tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();


	//===================
	// 敵の状態と処理
	//===================
	switch (m_status)
	{
	//突進で攻撃
	case BOSS_STS_ATTACK:
		ContactAtk(tempPos, &movement);
		break;
	//ジャンプで攻撃
	case BOSS_STS_JUMP:
		JumpAtk(tempPos, &movement, playerPos);
		break;
	//重力処理
	case BOSS_STS_FALL:
		Gravity(&movement);
		break;
	//飛行＆弾を撃って攻撃
	case BOSS_STS_FLY:
		SwitchAnim(E_Anim::Anim_Fly, false, false);
		FlyAtk(tempPos, &movement);
		break;
	//弾を撃って攻撃
	case BOSS_STS_SHOT:
		ShotAtk();
		break;
	//飛行の後、地面に戻る（降下）
	case BOSS_STS_FALLOUT:
		SwitchAnim(E_Anim::Anim_Fly, false, false);
		Fallout(&movement);	
		break;
	//ジャンプ＋弾を撃って攻撃
	case BOSS_STS_JUMPSHOT:
		JumpShot(tempPos, &movement, playerPos);
		break;
	//弱体化（頭を踏める状態）
	case BOSS_STS_WEAKENING:
		Weaking(tempPos, &movement);
		break;
	default:
		break;
	}


	if (GetStatus() == BOSS_STS_WEAKENING)	//もしボスの状態が弱っていたら
	{
		movement.y = 5;											//地面に落ちる
		this->GetTransform()->SetScale(vector3s(0.9, 0.9, 0));	//少し小さくする
	}
	else
	{
		this->transform->SetRot(vector3s(0, 0, 0));
		this->GetTransform()->SetScale(vector3s(1, 1, 0));
	}

	//=================
	// ＨＰ処理
	//=================
	if (m_hp <= 0)	//HPが0になったら
	{
		m_hp = 0;
		SetStatus(BOSS_STS_WEAKENING);	//状態を 弱体化 にする
	}

	//==========================
	// 頭を踏んだときの処理
	//==========================
	if (HitHeadNum >= BOSS_HITHEAD_NUM)	//BOSS_HITHEAD_NUMの回数分、頭を踏んだら（現在　BOSS_HITHEAD_NUM = ３）
		this->Die();	//ボスは倒れる

	vector3s curPos = this->GetTransform()->GetPos();

	//==============
	// 向きの処理
	//==============
	if (movement.x > 0  && GetStatus()!=BOSS_STS_WEAKENING|| tempPos.x < playerPos.x && GetStatus()!= BOSS_STS_WEAKENING)
	{
		this->transform->SetRot(vector3s(180.0f, 0.0f, 0.0f));
		firePos = vector3s(curPos.x + GetSize(false).x * 0.5f, curPos.y, 0.0f);
	}
	else if (movement.x < 0 && GetStatus() !=BOSS_STS_WEAKENING|| tempPos.x > playerPos.x && GetStatus()!= BOSS_STS_WEAKENING)
	{
		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
		firePos = vector3s(curPos.x - GetSize(false).x * 0.5f, curPos.y, 0.0f);
	}
	curPos.x += movement.x;
	curPos.y += movement.y;

	//==================
	// 画面限界処理
	//==================
	if (curPos.y > 800) {
		curPos.y = 800;
	}
	if (curPos.x > SCREEN_WIDTH - 80) {
		curPos.x = SCREEN_WIDTH - 80;
	}

	//============================
	// アニメーション　サウンド
	//============================
	if (curPos.y > 750.0f) {
		if (VecLength(movement) < 0.00001f) {
			SwitchAnim(E_Anim::Anim_Idle, false, false);
		}
		else {
			SwitchAnim(E_Anim::Anim_Move, false, false);
		}
		m_flySoundTimePass = SOUND_BOSS_FLY_TIME + 1.0f;
		StopSound(DragonFlySoundNo);
	}
	else {
		SwitchAnim(E_Anim::Anim_Fly, false, false);
		if (m_flySoundTimePass <= SOUND_BOSS_FLY_TIME) {
			m_flySoundTimePass += deltatime;
		}
		else {
			m_flySoundTimePass = 0.0f;
			PlaySound(DragonFlySoundNo, 0);
		}
	}



	AdjustScreenPoint(curPos, this->GetSize(true));
	this->SetPos(curPos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckBossCollision(this);
}

//==================
// 突進攻撃
//==================
void Boss::ContactAtk(vector3s tempPos, vector2s* movement)	
{

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player == NULL)return;
	player_Pos = player->GetTransform()->GetPos();

	if (moveWaittime > 60 * 2)	//２秒後に突進
	{
		//プレイヤーの方向に突進
		PlaySound(DragonAtkSoundNo, 0);
		temp.x = player_Pos.x - tempPos.x;
		VecNormalize(temp);
		temp.x *= 0.02f;
		movement->x = temp.x;

		if (setWaittime > 60 * 0.8f)		//突進の数秒後に
		{
			movement->x = BOSS_STOP_MOVEMENT;	//一度止まる
			
			SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次のステータスはランダム（突進orジャンプアタックor飛行）
			moveWaittime = setWaittime = 0.0f;
		}
		setWaittime++;
	}

	moveWaittime++;


}

//===================
// ジャンプアタック
//===================
void Boss::JumpAtk(vector3s tempPos, vector2s* movement, vector3s playerPos)
{

	if (moveWaittime > 60 * 2)	//２秒後にジャンプアタック
	{
		PlaySound(DragonAtkSoundNo, 0);

		//プレイヤーの方向にジャンプアタック
		temp.x = playerPos.x - tempPos.x;
		VecNormalize(temp);
		temp *= 0.05f;
		movement->x = temp.x;
		movement->y = m_jumpPower;

		if (tempPos.y < SCREEN_HEIGHT / 2 - 250.0f)
		{
			moveWaittime = 0.0f;
			SetStatus(BOSS_STS_FALL);	//重力処理
		}
	}

	moveWaittime++;


}

//=====================
// ジャンプ＆ショット
//=====================
void Boss::JumpShot(vector3s tempPos, vector2s* movement, vector3s playerPos)
{

	if (moveWaittime > 60 * 2)	//２秒後にジャンプ
		movement->y = -7.0f;

	if (tempPos.y < 400)
		movement->y = 0;

	if (moveWaittime == 60 * 3)	
	{
		//弾を生成
		enemybulletCell = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet_Fireball);
		enemybullet = (EnemyBullet*)enemybulletCell;
		enemybullet->GetTransform()->SetScale(vector3s(1.5, 1.5, 0));
		enemybullet->GetTransform()->SetPos(tempPos);
		//弾　プレイヤーに向かって飛んでくる
		vector3s temp = enemybullet->GetTransform()->GetPos() - playerPos;
		VecNormalize(temp);
		temp *= 0.002;
		enemybullet->SetDir(vector2s(-temp.x, -temp.y));

	}
	if (moveWaittime > 60 * 5)	//５秒後に重力処理
	{
		movement->y = (m_gravity - 5.0f);
	}
	if (moveWaittime == 60 * 6)	//６秒後に状態をセット
	{
		SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次の状態をランダムにセット
		moveWaittime = 0;
	}
	moveWaittime++;


}

//=========================
// 重力処理（ジャンプ時用）
//=========================
void Boss::Gravity(vector2s* movement)
{
	BreakBlockFlag = true;
	movement->y = m_gravity;//重力処理

	if (setWaittime > 60 * 0.7) //ジャンプ後一度動きを止める
	{
		movement->x = movement->y = BOSS_STOP_MOVEMENT;
		SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次の状態をランダムにセット
		setWaittime = 0.0f;
	}
	setWaittime++;
}

//==========================
// ショット（飛行のとき用）
//==========================
void Boss::FlyShotAtk()
{
	if (frame > 60 * 0.9)
	{
		PlaySound(DragonShotSoundNo, 0);
		//弾を生成
		enemybulletCell = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet_Fireball);
		enemybulletCell2 = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet_Fireball);
		enemybulletCell3 = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet_Fireball);
		enemybullet = (EnemyBullet*)enemybulletCell;	enemybullet2 = (EnemyBullet*)enemybulletCell2;	enemybullet3 = (EnemyBullet*)enemybulletCell3;

		enemybullet->GetTransform()->SetPos(tempPos);	enemybullet2->GetTransform()->SetPos(tempPos);	enemybullet3->GetTransform()->SetPos(tempPos);

		//弾　プレイヤーに向かって飛んでくる
		vector3s temp = enemybullet->GetTransform()->GetPos() - playerPos;
		VecNormalize(temp);
		temp *= 0.002;
		enemybullet->SetDir(vector2s(-temp.x, -temp.y));
		enemybullet2->SetDir(vector2s(-0.7, 1.0));
		enemybullet3->SetDir(vector2s(0.7, 1.0));

		frame = 0;

	}

	frame++;
}

//=====================
// ショット
//=====================
void Boss::ShotAtk()
{
	if (frame > 3.0)
	{
		//PlaySound(DragonShotSoundNo, 0);

		//弾を生成
		enemybulletCell = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet_Fireball);
		enemybullet = (EnemyBullet*)enemybulletCell;
		enemybullet->GetTransform()->SetScale(vector3s(1.5, 1.5, 0));
		enemybullet->GetTransform()->SetPos(firePos);
		enemybullet->SwitchAnim(E_Anim::Anim_Idle, true, false);

		//弾　プレイヤーに向かって飛んでくる
		Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
		playerPos = player->GetTransform()->GetPos();
		vector3s temp = enemybullet->GetTransform()->GetPos() - playerPos;
		VecNormalize(temp);
		temp *= 0.002;
		enemybullet->SetDir(vector2s(-temp.x, -temp.y));

		if (frame > 4.0)
		{
			frame = 0;
			SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次の状態をランダムにセット
		}

	}

	frame++;

}

//============================
// 飛行 ＆ 追尾 ＆ ショット
//============================
void Boss::FlyAtk(vector3s tempPos, vector2s* movement)	
{

	if (moveWaittime > 60 * 2)	//２秒後に飛ぶ
	{

		if (tempPos.y < BOSS_FLYLIMIT_UP)	//BOSS_FLYLIMIT_UPまで行ったら
		{
			flyspeed = 0;					//Y軸の動きはストップ

			//プレイヤーを追尾
			temp.x = playerPos.x - tempPos.x;
			VecNormalize(temp);
			temp *= 0.02f;
			movement->x = temp.x;
			FlyShotAtk();	//弾を撃ってくる
		}

		movement->y = flyspeed;	//飛ばせる
	}

	if (moveWaittime > 60 * 9)	//９秒後に降下
	{
		SetStatus(BOSS_STS_FALLOUT);
		moveWaittime = 0;
	}

	moveWaittime++;
}

//==========================
// 降下処理（飛行のとき用）
//==========================
void Boss::Fallout(vector2s* movement)
{
	movement->y = m_gravity + 1.0f;	//重力処理
	BreakBlockFlag = true;
	if (setWaittime > 60 * 1.1)
	{
		BreakBlockFlag = false;
		movement->x = movement->y = BOSS_STOP_MOVEMENT;
		movespeed = BOSS_MOVEMENT_SPEED;
		flyspeed = -8.0f;
		SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次の状態をランダムにセット
		setWaittime = 0.0f;
	}
	setWaittime++;
}

//=====================
// 弱体化
//=====================
void Boss::Weaking(vector3s tempPos, vector2s* movement)
{
	movement->x = movement->y = BOSS_STOP_MOVEMENT;
	//=============================================
	//頭を踏める状態だと分かりやすくするための処理（現在はひっくり返るように処理）
	this->GetTransform()->SetScale(vector3s(0.8, 0.8, 0));
	this->transform->SetRot(vector3s(0, 90, 0));
	//-----------------------------------------------------------------------

	if (setWaittime > 60 * 10)	//踏めなかったらボス回復
	{
		m_hp = BOSS_HP;			//HP回復
		SetStatus((E_BossStatus)(rand() % BOSS_ATK_NUM));	//次の状態をランダムにセット
		setWaittime = 0;
	}
	setWaittime++;

}

//=====================
// 倒れる処理
//=====================
void Boss::Die()
{
	Enemy::Die();
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player != nullptr) {
		//player->inputLock = true;
		//player->m_winSign = true;
		player->SetWinSign(true);
		vector3s rot = player->GetTransform()->GetRot();
		player->SetRot(vector3s(rot.x, 0.0f, 0.0f));
		player->SwitchAnim(E_Anim::Anim_Win, false, true);
		StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GamePlay);
		PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_GameClear, -1);
	}
}

#pragma region boss


//===========================
//	ボスの当たり判定
//===========================
void  SceneObjMgr::CheckBossCollision(Boss* boss)
{
	if (boss == NULL)return;

	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {
		Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
		if (player == NULL)return;
		boss->player_Pos = player->GetTransform()->GetPos();
		if (player->CheckCanBeAttacked() == false)return;
		if (Collider::CollisionDetect(boss, player)) {
			Transform::AdjustPosition(player, boss->GetTransform()->GetPos(), boss->GetColliderSize(true, 0), false);
			if (boss->GetStatus() != BOSS_STS_WEAKENING)
			{
				player->Hurted(1);
			}

			if ((player->GetColliderSize(true, 0).y / 2) + (boss->player_Pos.y + 35) < (boss->GetTransform()->GetPos().y) && boss->m_hp == 0)
			{
				boss->SetHitHead(true);	//頭を踏まれた
			}
			if (boss->GetHitHead() == true)	//頭を踏まれたら
			{
				boss->m_hp = BOSS_HP;	//１回頭を踏まれたらボスのHPはBOSS_HP（現在は３０に設定）に初期化
				boss->HitHeadNum++;		//HitHeadNum　>=　BOSS_HITHEAD_NUM　になったらボスは倒れる（現在　BOSS_HITHEAD_NUM = ３）
				boss->SetHitHead(false);
				player->SetvelocityY(PLAYER_JUMP_POWER);	//踏んだ時にプレイヤーが跳ねる
				boss->SetStatus(BOSS_STS_ATTACK);
			}
		}
	}

}

#pragma endregion boss
