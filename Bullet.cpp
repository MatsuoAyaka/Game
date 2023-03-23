#include "Bullet.h"
#include "EventCenter.h"

Bullet::Bullet() 
{
	bulletSpeed = 20.0f;
	moveDir = vector2s(1.0f, 0.0f);
	cellType = E_CellType::CellType_Bullet;
}
Bullet::Bullet(vector3s pos)
{
	bulletSpeed = 10.0f;
	moveDir = vector2s(1.0f, 0.0f);
	cellType = E_CellType::CellType_Bullet;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(pos, true);
	SetRot(vector3s(0.0f, 0.0f, -90.0f));

	DrawData* bulletDrawData = new DrawData();
	bulletDrawData->shakeWithCamera = true;
	bulletDrawData->SetSize(bullet_size);
	bulletDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	bulletDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_RICE_START_INDEX_X, TEX_ENVIRONMENT_ITEM_RICE_START_INDEX_Y);
	bulletDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	bulletDrawData->tex_texel_size = environment_item_cell_tex_size;
	SetDrawData(bulletDrawData);

	Collider* bulletCollider = new Collider();
	ColliderData* bulletColliderData = new ColliderData(0, bulletCollider, true, E_ColliderType::Collider_Rect,
		bulletDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	bulletCollider->AddColliderData(bulletColliderData);

	RegisterComponent(E_Component::Component_Collider, bulletCollider, true);
}
;

void Bullet::DoUpdate(float deltatime) {
	if (enable == false)return;
	BaseCell::DoUpdate(deltatime);

	vector3s pos = transform->GetPos();
	movement = moveDir * bulletSpeed;
	pos.x += movement.x;
	pos.y += movement.y;

	SetPos(pos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckBulletCollision(this);
}

void Bullet::SetDir(vector2s _dir)
{
	moveDir = _dir;
}

void Bullet::SetSpeed(float _speed)
{
	bulletSpeed = _speed;
}

//=========================================================
//追加松尾　ベータ版

EnemyBullet::EnemyBullet()
{
	bulletSpeed = 5.0f;
	moveDir = vector2s(1.0f, 0.0f);
	cellType = E_CellType::CellType_Bullet;
}

EnemyBullet::EnemyBullet(vector3s pos)
{
	bulletSpeed = 5.0f;
	moveDir = vector2s(1.0f, 0.0f);
	cellType = E_CellType::CellType_Bullet;

	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(pos, true);
	SetRot(vector3s(0.0f, 0.0f, 0.0f));

	DrawData* bulletDrawData = new DrawData();
	bulletDrawData->shakeWithCamera = true;
	bulletDrawData->SetSize(vector2s(80.0f, 80.0f));
	bulletDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fireball.png"));
	bulletDrawData->texIndex = vector2s(TEX_FIREBALL_START_INDEX_X, TEX_FIREBALL_START_INDEX_Y);
	bulletDrawData->tex_w_count = TEX_FIREBALL_COUNT_W;
	bulletDrawData->tex_texel_size = fire_ball_tex_size;
	SetDrawData(bulletDrawData);

	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_FIREBALL_START_INDEX_Y * TEX_FIREBALL_COUNT_W + TEX_FIREBALL_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_FIREBALL_ANIM_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	Collider* bulletCollider = new Collider();
	ColliderData* bulletColliderData = new ColliderData(0, bulletCollider, true, E_ColliderType::Collider_Rect,
		bulletDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	bulletCollider->AddColliderData(bulletColliderData);

	RegisterComponent(E_Component::Component_Collider, bulletCollider, true);
}
;

void EnemyBullet::DoUpdate(float deltatime) {
	if (enable == false)return;
	BaseCell::DoUpdate(deltatime);

	vector3s pos = transform->GetPos();
	movement = moveDir * bulletSpeed;
	pos.x += movement.x;
	pos.y += movement.y;

	SetPos(pos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckEnemyBulletCollision(this);
}

void EnemyBullet::SetDir(vector2s _dir)
{
	moveDir = _dir;
	float angle = (atan2(downVector.x, downVector.y) - atan2(moveDir.x, moveDir.y)) * RAD2DEG;
	this->GetTransform()->SetRot(vector3s(0, 0, angle));
}

void EnemyBullet::SetSpeed(float _speed)
{
	bulletSpeed = _speed;
}



void SceneObjMgr::CheckEnemyBulletCollision(EnemyBullet* enemybullet)
{
	if (enemybullet == NULL)return;

	//障害物との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(enemybullet, iter->second)) {
				//enemybullet->SetState(false);
				enemybullet->Recycle();
				return;
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
			if (iter->second->GetCellType() == E_CellType::CellType_Enemy)continue;

			if (Collider::CollisionDetect(enemybullet, iter->second)) {
				EventCenter::GetInstance()->EventTrigger<int>("EnemyContactDmg", enemybullet->GetDamage());
				//enemybullet->SetState(false);
				enemybullet->Recycle();
				return;
			}
		}
	}
}

void SceneObjMgr::CheckBulletCollision(Bullet* bullet)
{
	if (bullet == NULL)return;

	//障害物との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (Collider::CollisionDetect(bullet, iter->second)) {
				//bullet->SetState(false);
				//エフェクトを生成
				GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Hit, NULL, bullet->GetTransform()->GetPos(), vector3s(0, 0, 0));
				bullet->Recycle();
				return;
			}
		}

	}
	//敵との当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Enemy) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Enemy];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(bullet, iter->second)) {
				//Transform::AdjustPosition(boss, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				EventCenter::GetInstance()->EventTrigger<BaseCell, int>(iter->second, "EnemyContactBullet", bullet->GetDamage());
				//bullet->SetState(false);
				//エフェクトを生成
				GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Hit, NULL, bullet->GetTransform()->GetPos(), vector3s(0, 0, 0));
				bullet->Recycle();
				return;
			}
		}
	}

	//ボスとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Boss) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Boss];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(bullet, iter->second)) {
				//Transform::AdjustPosition(boss, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				EventCenter::GetInstance()->EventTrigger<BaseCell, int>(iter->second, "BossContactBullet", bullet->GetDamage());
				//bullet->SetState(false);
				//エフェクトを生成
				GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Hit, NULL, bullet->GetTransform()->GetPos(), vector3s(0, 0, 0));
				bullet->Recycle();
				return;
			}
		}
	}
}