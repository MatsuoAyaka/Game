#include "Player.h"
#include "Scene.h"
#include "MapGenerator.h"
#include "Item.h"
#include "GameLogic.h"
#include "EventCenter.h"
#include "Bullet.h"
#include "Effect.h"
#include "Enemy.h"

#define PLAYER_JUMP_POWER (-9.6f)
#define PLAYER_HP 100
#define GRAVITY_ACCELERATION_DEGREES (0.28f)
#define PLAYER_TUMBLE_ANGLE (45)

Player::Player()
{
	hp = PLAYER_HP;
	m_LifeValue = PLAYER_HP;
	cellType = E_CellType::CellType_Player;
	//==================================================
	//�������ǉ�
	SetaccelerationY(GRAVITY_ACCELERATION_DEGREES);
	std::srand(time(NULL));
	//�������ǉ�
	//====================================================
	curGround = NULL;

	EventCenter::GetInstance()->AddEventListener(this, "EnemyContactDmg", &Player::Hurted);
	EventCenter::GetInstance()->AddEventListener(this, "WindContactPlayer", &Player::SwayByWind);
	EventCenter::GetInstance()->AddEventListener(this, "SpringContactPlayer", &Player::JumpBySpring);
	EventCenter::GetInstance()->AddEventListener(this, "BeltConveyorContactPlayer", &Player::MoveByBelt);

	//23.1
	m_sweatEffect = new EffectSweat();
	m_sweatEffect->SetState(false);
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_sweatEffect, true);

	//�T�E���h�f�[�^�̃��[�h
	char filename_Bullet[] = "data\\SE\\Bullet_01.wav";
	BulletSoundNo = LoadSound(filename_Bullet);

	char filename_CollectItem[] = "data\\SE\\�A�C�e���擾_01.wav";
	CollectItemSoundNo = LoadSound(filename_CollectItem);

	char filename_Jump[] = "data\\SE\\�W�����v_01.wav";
	JumpSoundNo = LoadSound(filename_Jump);

	char filename_JumpBySpring[] = "data\\SE\\�W�����v�u���b�N_01.wav";
	JumpBySpringSoundNo = LoadSound(filename_JumpBySpring);

	char filename_MoveByBelt[] = "data\\SE\\�x���g�R���x�A_01.wav";
	MoveByBeltSoundNo = LoadSound(filename_MoveByBelt);

	char filename_SwayByWind[] = "data\\SE\\��_01.wav";
	SwayByWindSoundNo = LoadSound(filename_SwayByWind);

	char filename_PlayerDamage[] = "data\\SE\\playerDamage2.wav";
	PlayerDamageSoundNo = LoadSound(filename_PlayerDamage);
}

Player::~Player()
{
	curGround = NULL;
	EventCenter::GetInstance()->RemoveEventListener<void, int>("EnemyContactDmg");
	EventCenter::GetInstance()->RemoveEventListener<void>("WindContactPlayer");
	EventCenter::GetInstance()->RemoveEventListener<void, BaseCell*>("SpringContactPlayer");
	EventCenter::GetInstance()->RemoveEventListener<void, float, bool>("BeltConveyorContactPlayer");
	m_sweatEffect = NULL;
}

void Player::DoUpdate(float deltatime) {

	if (enable == true);

	if (curGround!=NULL && m_curGroundCounter < m_groundCounter) {
		m_curGroundCounter++;
		if (m_curGroundCounter >= m_groundCounter) {
			SetCurGround(NULL);
		}
	}

	if (m_canBeAttacked == false) {
		m_beAttackedRecoveryTimePass += deltatime;
		UpdateEffect();
		if (m_beAttackedRecoveryTimePass >= BE_ATTACKED_RECOVERY_TIME) {
			m_beAttackedRecoveryTimePass = 0.0f;
			m_canBeAttacked = true;
		}
	}
	else {
		if (m_sweatEffect != NULL) {
			m_sweatEffect->SetState(false);
		}
	}

	if (m_winSign == true) {
		m_winTimePass += deltatime;
		if (m_winTimePass > WIN_ANIMATION_TIME) {
			SwitchAnim(E_Anim::Anim_Idle, true, true);
			m_winTimePass = 0.0f;
			m_winSign = false;
		}
	}

	BaseCell::DoUpdate(deltatime);

}

void Player::Input(float deltatime) {
	if (enable == false)return;

	if (GetKeyboardTrigger(DIK_P)) {
		inputLock = !inputLock;
	}

	if (inputLock == true) {
		return;
	}
	if (onIce == false)
	{
		movement = vector2s(0, 0);
		//tempSpeed = 0;
	}
	else {
		movement.y = 0.0f;
	}
	playerDeltaTime = deltatime;
	tempPos = this->GetTransform()->GetPos();
	tempRotZ = (int)(this->GetTransform()->GetRot().z) % 360;
	playerMovementSign = false;
	
	tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();
	
	//=====================================================
	//�������ǉ�����
	nowJumpStartFlag = false;	//�W�����v���n�߂��u�Ԃ�

	//SetParam(deltatime);

//�X�y�[�X�L�[�̃g���K�[�𒲂ׂ�
	SetSpceKeyTrigger();
	//�󒆂ɂ��邩����
	checkInAir(tempCell, &movement);

	//�󒆃W�����v
	inAirJump(&movement, tempCell, &tempPos);

	//�W�����v�J�n����
	jump(&movement, this, &tempPos);

	//�󒆂ɂ�����d�͂̉e�����󂯂鏈��
	recieveGravity(&movement, this);

	//���n����Ƃ��̏���
	landing(&tempRotZ);
	//�]�ԏ���
	tumble(&tempRotZ);
	//�e��������
	shootBullet();

	//����
	reactionShoot();
	//�n��ł̈ړ�
	//move();

	//�󒆂ŕǂɂԂ�������X�s�[�h��������
	if (inAirDeceleration != 0)movement.x *= 0.1f;

	//�h��鏈��
	sway();

	//�X����ς���
	changeRot();



	//�ŏI�I�ɍ��W���Z�b�g
	//changePos();
	//�o�l�ŃW�����v���鏈��
	JumpBySpringUpdate(&movement);
	//���G��Ԃ̃J�E���g�X�V
	DamageStutasUpdate();


	MovingGroundUpdate(deltatime);

	//�A�C�e������
	CollectPepper();
	CollectPickledPlums();
	CollectMisoSoup();

	
	//�������ǉ�����
	//=====================================================
	if (canMove)
	{
		Nezumi* nezumi = GameMgr::GetInstance()->sceneObjMgr->GetNezumi();

		if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT)) {
			if (onIce == true ) {
				tempSpeed -= PLAYER_MOVEMENT_SPEED * deltatime;
				if (tempSpeed < min_speed)
				{
					tempSpeed = min_speed;
				}
				
				movement.x += tempSpeed * deltatime;
			}
			else {
				if (!springJumpedSec)
				{
					movement.x -= PLAYER_MOVEMENT_SPEED * deltatime;
				}
			}

			if (tempCell != NULL) {
				//tempPos.x += movement.x;
				tempPos.x = this->GetTransform()->GetPos().x + movement.x;
				//vector2s DrawSize = GetDrawData()->GetSize();
				//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos + vector3s(0.0f, DrawSize.y * 0.48f, 0.0f), vector3s(0.0f, 0.0f, 0.0f), this->GetTransform()->GetScale(), vector2s(DrawSize.x * 0.8f, 20.0f), 0);
				//if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
				//	//movement.x = 0;
				//	//this->SetPos(tempCell->GetTransform()->GetPos(), true);
					playerMovementSign = true;

				//	if (GetinAir())SetInAirDeceleration(false);
				//}
				//else {
				//	playerMovementSign = true;
				//}
			}
			else {
				playerMovementSign = true;
			}

			//if (nezumi->GetStatus() == ENEMY_STS_ATTACK)
			//{
			//	movement.x = 0;
			//	hitAwayCounter++;
			//}
		}
		else if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT)) {
			if (onIce == true ) {
				tempSpeed += PLAYER_MOVEMENT_SPEED * deltatime;
				if (tempSpeed > max_speed)
				{
					tempSpeed = max_speed;
				}
				movement.x += tempSpeed * deltatime;
			}
			else {
				if (!springJumpedSec)
				{
					movement.x += PLAYER_MOVEMENT_SPEED * deltatime + tempSpeed * deltatime;
				}
			}
			if (tempCell != NULL) {
				//tempPos.x += movement.x;
				tempPos.x = this->GetTransform()->GetPos().x + movement.x;
				playerMovementSign = true;
				//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
				/*if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
					movement.x = 0;
					this->SetPos(tempCell->GetTransform()->GetPos(), true);

					if (GetinAir())SetInAirDeceleration(true);
				}
				else {
					playerMovementSign = true;
				}*/
			}
			else {
				playerMovementSign = true;
			}

			//if (nezumi->GetStatus() == ENEMY_STS_ATTACK)
			//{
			//	hitAwayCounter++;
			//}
		}
		//=================================23.1.20 �e�X�g
		//if (GetKeyboardTrigger(DIK_C)) {
		//	inputLock = true;
		//	SwitchAnim(E_Anim::Anim_Win, true, true);
		//	return;
		//}
		//===================================
		//else if (GetKeyboardPress(DIK_W)) {
		//	movement.y -= PLAYER_MOVEMENT_SPEED * deltatime;
		//}
		//else if (GetKeyboardPress(DIK_S)) {
		//	movement.y += PLAYER_MOVEMENT_SPEED * deltatime;
		//}
	}
	
	if (GetInAirDeceleration())
		movement.x *= 0.1f;

	float xAxisRot = xAxisRot = this->GetTransform()->GetRot().x;
	if (GetinAir()==false) {
		if (movement.x > 0) {
			//this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
			xAxisRot = 0.0f;
		}
		else if (movement.x < 0) {
			//this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
			xAxisRot = -180.0f;
		}
	}

	//�h��鏈��
	//sway(&tempRotZ, movement.x);
	//temprotz = ((int)temprotz % 360);
	//this->transform->setrot(vector3s(xaxisrot, 0.0f, temprotz));
	this->SetRot(vector3s(xAxisRot, 0, tempRotZ));


	//anim
	//�A�j���[�V�����̃Z�b�g
	//SetAnim();
	Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
	if (anim != NULL && m_winSign == false) {
		if (m_canBeAttacked == true) {
			if (canMove == true) {
				if (onIce == false) {
					if (playerMovementSign) {
						anim->SwitchAnim(E_Anim::Anim_Move, false, true);
					}
					else if (playerMovementSign == false && newTumbleFlag == true) {
						anim->SwitchAnim(E_Anim::Anim_Tumble, false, true);
					}
					else
					{
						anim->SwitchAnim(E_Anim::Anim_Idle, false, true);
					}
				}
				else {
					anim->SwitchAnim(E_Anim::Anim_Slide, false, true);
				}
			}
			else {
				if (newTumbleFlag == true) {
					SwitchAnim(E_Anim::Anim_Tumble, true, false);
				}
			}
		}
		else {
			if (newTumbleFlag == false) {
				SwitchAnim(E_Anim::Anim_Hurt, false, false);
			}
			else {
				SwitchAnim(E_Anim::Anim_Tumble, false, false);
			}
		}
	}

	if (playerMovementSign == false) {
		tempSpeed *= 0.95f;
	}
	//�_�b�V���ɂ�鑬�x�A�b�v
	dush();
	Nezumi* nezumi = GameMgr::GetInstance()->sceneObjMgr->GetNezumi();

	if (nezumi->GetStatus() == ENEMY_STS_ATTACK && (IsButtonTriggered(0, BUTTON_B) || GetKeyboardTrigger(DIK_U)))
	{
		hitAwayCounter++;
	}
	//============================================================
	//�������폜����
	//movement = VecNormalize(movement) * PALYER_MAX_MOVE_SPEED;
	//�������폜����
	//==========================
	if (this->GetOnIce() && this->GetinAir()==false)
	{
		this->Slide();
		movement.x *= 0.98;
	}
	vector3s curPos = this->GetTransform()->GetPos();

	curPos.x += movement.x;
	curPos.y += movement.y;

	//�����蔻��
	if ((GameMgr::GetInstance()->GetCurGameState()== E_GameState::GameState_Game_Normal && AdjustScreenPoint(curPos, this->GetSize(true))) || hp <= 0) {
		//if (curGround != NULL) {
		//	curPos = curGround->GetTransform()->GetPos();
		//	curPos.y = curPos.y - curGround->GetSize(true).y - 5.0f;
		//}
		//else {
		//	curPos.y = SCREEN_HEIGHT * 0.5f;
		//	movement.y = 0.0f;
		//	SetvelocityY(0.0f);
		//}


		GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Lose, false);

		/*if (GameMgr::GetInstance()->sceneMgr->switchSceneSign == false) {
			GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_End, true, true);
		}*/
		return;
	}

	this->SetPos(curPos, true);

	GameMgr::GetInstance()->sceneObjMgr->CheckPlayerCollision(this);

}

void Player::SetState(bool state)
{
	if (state == false) {

	}
	else {

	}
	BaseCell::SetState(state);
}

//======================================================
//�������ǉ�����
void Player::checkInAir(BaseCell* tempCell, vector2s* pmovement)
{
	oldInAir = GetinAir();
	//�󒆂ɂ���̂��n��ɂ���̂����`�F�b�N
	vector3s tempPos = this->GetTransform()->GetPos();
	tempPos.y = this->GetTransform()->GetPos().y + 2.0f;
	vector2s DrawSize = GetDrawData()->GetSize();
	GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos + vector3s(0.0f, DrawSize.y * 0.47f, 0.0f), 
		vector3s(0.0f, 0.0f, 0.0f), this->GetTransform()->GetScale(), vector2s(DrawSize.x * 0.7f, 10.0f), 0);

	if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() && GetvelocityY() >= 0)
	{
		//�n�ʂƐڂ��Ă��āA���W�����v�J�n��ԂłȂ����
		SetinAir(false);
		SetInAirDeceleration(false);
		//pmovement->y = tempCell->GetTransform()->GetPos().y - this->GetTransform()->GetPos().y;	//�������̕������R�ɓ���
	}
	else
	{
		SetinAir(true);
	}
	//tempPos = this->GetTransform()->GetPos();	//tempPos���v���C���[�̍��W�Ɠ����ɖ߂�
}

void Player::sway(float* prot, float movementx)
{
	//�󒆃W�����v������ƃo�����X���悭�ł���
	if (InAirJumpFlag == true)
	{
		if (*prot > 0.0f)
		{
			if (movementx == 0.0f)
			{
				//*prot -= PLAYER_ROTATION_SPEED;
				*prot += (PLAYER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��A�A�C�e���ɂ���ĕω�����  by���E
				if (*prot < 0.0f)
				{
					*prot = 0.0f;
				}
			}
		}
		else if (*prot < 0.0f)
		{
			if (movementx == 0.0f)
			{
				//*prot += PLAYER_ROTATION_SPEED;
				*prot -= (PLAYER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��A�A�C�e���ɂ���ĕω�����  by���E
				if (*prot > 0.0f)
				{
					*prot = 0.0f;
				}
			}
		}
	}


	if (inAir)
	{
		if (movementx > 0.0f)
		{
			*prot += PLAYER_ROTATION_SPEED;
			if (*prot < 0.0f)
			{
				*prot = 0.0f;
			}
		}
		else if (movementx < 0.0f)
		{
			*prot -= PLAYER_ROTATION_SPEED;
			if (*prot > 0.0f)
			{
				*prot = 0.0f;
			}
		}
	}

	if (inAir == false && newTumbleFlag == false)
	{
		if ((*prot > 0.0f && *prot < 180.0f) ||(*prot < -180.0f))
		{
			*prot -= PLAYER_ROTATION_SPEED;
			if (*prot < 0.0f)
			{
				*prot = 0.0f;
			}
		}
		else if ((*prot < 0.0f && *prot > -180.0f) || (*prot > 180.0f))
		{
			*prot += PLAYER_ROTATION_SPEED;
			if (*prot > 0.0f)
			{
				*prot = 0.0f;
			}
		}
	}
}

void Player::landing(float* prot)
{
	//if (oldInAir == true && inAir == false)
	if(curGround!=NULL)
	{
		inAirJumpPoint = PLAYER_INAIRJUMP_LIMIT;
		InAirJumpFlag = false;
	}
}

void Player::recieveGravity(vector2s* pmovement, BaseCell* tempCell)
{
	//�󒆂ɂ�����d�͂��͂��炭
	if (GetinAir())SetvelocityY(GetvelocityY() + GetaccelerationY());	//�󒆂ɂ����ԂȂ�A���x�ɉ����x�𑫂�
	else SetvelocityY(1.0f);												//�n��ɂ����ԂȂ�AY�����̑��x��0�ɂ���	

	pmovement->y = GetvelocityY();	//���x�Ɠ����傫����y�����ւ̈ړ����Z�b�g
}

void Player::tumble(float* prot)
{
	//�O��|��Ă��������Z�b�g
	oldTumbleFlag = newTumbleFlag;

	if (inAir == false && oldInAir == true)
	{
		//�v���C���[���n��ɂ��ā����X�����傫��������@�]��
		bool sign = *prot < 0 ? false : true;
		*prot = (int)(*prot) % 360;

		if (*prot >= PLAYER_TUMBLE_ANGLE && *prot<= 315)
		{
			*prot = 90;
			newTumbleFlag = true;
			hp--;
		}
		else if (*prot <= -PLAYER_TUMBLE_ANGLE && *prot>=-315)
		{
			*prot = -90;
			newTumbleFlag = true;
			hp--;
		}
		else
		{
			newTumbleFlag = false;
		}

		if (!beStable)
		{

			if ((*prot <= 15 && *prot <= 315) && (*prot >= -15 && *prot >= -315))
			{
				hp += healpoint;

				GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Recover, NULL, vector3s(this->GetTransform()->GetPos().x, this->GetTransform()->GetPos().y + 50, this->GetTransform()->GetPos().z), vector3s(1, 0, 0));
				PlaySound(CollectItemSoundNo, 0);
			}
		}


		if (newTumbleFlag == true && oldTumbleFlag == false)
		{
			//���]�񂾂Ȃ�v���C���[�𓮂��Ȃ�����
			canMove = false;
			m_canBeAttacked = false;
			if (m_sweatEffect != NULL) {
				m_sweatEffect->SetState(true);
			}
		}
	}

	//�v���C���[�������Ȃ��Ȃ�
	if (canMove == false)
	{
		//�����Ȃ����Ԃ����������炷
		tumbleTime--;
		//�����Ȃ����Ԃ��O�ȉ��ɂȂ�����
		if (tumbleTime <= 0.0f)
		{
			tumbleTime = PLAYER_TUMBLE_TIME;
			newTumbleFlag = false;
			canMove = true;
			m_canBeAttacked = true;
			*prot = 0.0f;
		}
		else {
			if (*prot < 0) {
				*prot = -135.0f;
			}
			else {
				*prot = 135.0f;
			}
		}
	}

}

//�������ǉ�����
//======================================================

//=========================================
	// �������ǉ�����
void Player::jump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos)
{

	if (canMove == false) return;

	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonPressed(0, BUTTON_A))
	{
		//�X�y�[�X�L�[�������ꂽ��W�����v
		//�n��ɂ��鎞�����W�����v�ł���
		if (!GetinAir())
		{
			if (!beStable)
			{
				tempRotZ = GetRandomRot();
			}

			//�G�t�F�N�g�𐶐�
			vector3s effectPos = this->GetTransform()->GetPos();
			//effectPos.y -= 10.0f;
			GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Jump, NULL, effectPos, vector3s(1, 0, 0));
			SetvelocityY(PLAYER_JUMP_POWER);	//�����x���Z�b�g
			SetOnIce(false);
			//if (onIce == false) {
			//	SetvelocityY(PLAYER_JUMP_POWER);	//�����x���Z�b�g
			//}
			//else {
			//	SetvelocityY(PLAYER_JUMP_POWER * 10);	//�����x���Z�b�g
			//}
			pmovement->y = 0.0f;
			SetinAir(true);

			PlaySound(JumpSoundNo, 0);

		}
	}
}

void Player::inAirJump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos)
{
	if (canMove == false)return;

	if (spaceKeyTrigger == true)
	{
		//�X�y�[�X�L�[�������ꂽ��󒆃W�����v

		//�󒆂ɂ��ā����󒆃W�����v�̎c��񐔂��O�łȂ������W�����v�J�n���łȂ��Ȃ�W�����v�ł���
		if (inAir == true && inAirJumpPoint >= 1 && nowJumpStartFlag == false)
		{
			if (!beStable)
			{
				tempRotZ = GetRandomRotAir();
			}
			SetvelocityY(PLAYER_JUMP_POWER);	//�����x���Z�b�g
			SetinAir(true);
			inAirJumpPoint--;
			InAirJumpFlag = true;	//�󒆃W�����v�����t���O��true�ɂ���
		}
	}
}
// �������ǉ�����
	//================================

void Player::SetSpceKeyTrigger()
{
	oldSpaceKey = newSpaceKey;
	if (GetKeyboardPress(DIK_SPACE) || IsButtonPressed(0, BUTTON_A))newSpaceKey = true;
	else newSpaceKey = false;

	if (newSpaceKey == true && oldSpaceKey == false) spaceKeyTrigger = true;
	else spaceKeyTrigger = false;
}


//=========================================
//�������ǉ�
void Player::shootBullet()
{
	if (canMove == false)return;
	if (GetKeyboardTrigger(DIK_I) || IsButtonTriggered(0, BUTTON_X))
	{
		SetShoot(true);
		//Test20221121
		//Bullet* bullet = new Bullet(transform->GetPos());
		//GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(bullet);
		BaseCell* bulletCell = MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Bullet);
		if (bulletCell != NULL) {
			Bullet* bullet = (Bullet*)bulletCell;
			bullet->SetPos(vector3s(this->GetTransform()->GetPos().x + 50, this->GetTransform()->GetPos().y, this->GetTransform()->GetPos().z), true);
			if ((int)(this->GetTransform()->GetRot().x) != 0) {
				bullet->SetDir(vector2s(-1, 0));
			}
			else {
				bullet->SetDir(vector2s(1, 0));
			}

			bulletCell->SetState(true);

			//�T�E���h�Đ�
			PlaySound(BulletSoundNo, 0);
		}
		hp -= 1;
	}
}


//��]����
void Player::sway()
{
	
	//�󒆃W�����v������ƃo�����X���悭�ł���
	if (InAirJumpFlag == true)
	{
		if (tempRotZ > 0.0f)
		{
			//if (movement.x == 0.0f)
			//{
			//	tempRotZ -= PLAYER_ROTATION_SPEED;
			//	if (tempRotZ < 0.0f)
			//	{
			//		tempRotZ = 0.0f;
			//	}
			//}
		}
		else if (tempRotZ < 0.0f)
		{
			//if (movement.x == 0.0f)
			//{
			//	tempRotZ += PLAYER_ROTATION_SPEED;
			//	if (tempRotZ > 0.0f)
			//	{
			//		tempRotZ = 0.0f;
			//	}
			//}
		}
	}

	
	//�󒆂œ����Ă�������ɂ���ĉ�]����������ς��
	if (inAir && beStable == false)
	{
		if (movement.x > 0.0f)
		{
			//tempRotZ += PLAYER_ROTATION_SPEED;
			tempRotZ += (PLAYER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��A�A�C�e���ɂ���ĕω�����  by���E
			//if (tempRotZ < 0.0f)
			//{
			//	tempRotZ = 0.0f;
			//}
		}
		else if (movement.x < 0.0f)
		{
			//tempRotZ -= PLAYER_ROTATION_SPEED;
			tempRotZ -= (PLAYER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��A�A�C�e���ɂ���ĕω�����  by���E

			//if (tempRotZ > 0.0f)
			//{
			//	tempRotZ = 0.0f;
			//}

		}
	}
	

	//�󒆂Ŗ��L�[�������Ɖ�]����
	if (inAir && beStable == false)
	{
		if (GetKeyboardPress(DIK_L) || IsButtonPressed(0, BUTTON_R))
		{
			//tempRotZ += PLAYER_ROTATION_SPEED;
			//tempRotZ += PLAYER_RECOVER_ROTATION_SPEED;
			tempRotZ += (PLAYER_RECOVER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��A�A�C�e���ɂ���ĕω�����  by���E
			/*
			if (tempRotZ < 0.0f)
			{
				tempRotZ = 0.0f;
			}
			*/
		}
		else if (GetKeyboardPress(DIK_J) || IsButtonPressed(0, BUTTON_L))
		{
			//tempRotZ -= PLAYER_ROTATION_SPEED;
			//tempRotZ -= PLAYER_RECOVER_ROTATION_SPEED;
			tempRotZ -= (PLAYER_RECOVER_ROTATION_SPEED + sway_speed);//sway_speed�͉�]�����x�A�A�C�e�����Ȃ��Ƃ��͂O�A�A�C�e���ɂ���ĕω�����  by���E
			/*if (tempRotZ > 0.0f)
			{
				tempRotZ = 0.0f;
			}*/
		}
	}

	//�n��ŁA�]�|���Ă��Ȃ�����ǌX���Ă���ꍇ�X�������ɖ߂�
	if (inAir == false && newTumbleFlag == false)
	{
		//tempRotZ = 0;
		if (abs(tempRotZ)> PLAYER_ROTATION_SPEED) {
			if ((tempRotZ > 0.0f && tempRotZ < 180.0f) || tempRotZ < PLAYER_TUMBLE_ANGLE - 360)
			{
				tempRotZ -= PLAYER_ROTATION_SPEED;
				//if (tempRotZ < 0.0f)
				//{
				//	tempRotZ = 0.0f;
				//}
			}
			else if ((tempRotZ < 0.0f && tempRotZ > -180.0f) || tempRotZ > 360 - PLAYER_TUMBLE_ANGLE)
			{
				tempRotZ += PLAYER_ROTATION_SPEED;
				//if (tempRotZ > 0.0f)
				//{
				//	tempRotZ = 0.0f;
				//}
			}
		}
		else {
			tempRotZ = 0.0f;
		}
	}
}


//���ɐ�����鏈���@by�V���E
void Player::SwayByWind()
{
	if (--windCounter > 0)
		return;
	else
		windCounter = WIND_COUNTER;
	//range:-WIND_RANDOMANGLE~WIND_RANDOMANGLE
	float rotation = rand() % (2 * WIND_RANDOMANGLE) - WIND_RANDOMANGLE;
	float offsetMove = rand() % (2 * WIND_RANDOMMOVE) - WIND_RANDOMMOVE;

	vector3s tempRot = transform->GetRot();
	tempRot.z += rotation;
	transform->SetRot(tempRot);

	vector3s tempPos = transform->GetPos();
	tempPos.x += offsetMove;
	transform->SetPos(tempPos);

	PlaySound(SwayByWindSoundNo, 0);
}

void Player::JumpBySpring(BaseCell* springGround)
{
	inAirJumpPoint = 1;
	springJumped = true;
	springJumpedSec = true;
	springTime = SPRING_COUNTER;

	//vector3s groundPos = springGround->GetTransform()->GetPos();
	//vector2s groundSize = springGround->GetSize(true);

	vector3s groundPos = springGround->GetTransform()->GetPos();
	vector2s groundSize = springGround->GetSize(true);
	vector3s scale = springGround->GetTransform()->GetScale();
	Collider* groundCollider = (Collider*)springGround->GetComponent(E_Component::Component_Collider);
	if (groundCollider != NULL) {
		ColliderData* data = groundCollider->GetColliderDataArray()[0];
		groundSize.x = scale.x * data->size.x;
		groundSize.y = scale.y * data->size.y;

		groundPos.x += scale.x * data->offset.x;
		groundPos.y += scale.y * data->offset.y;
	}
	vector3s playerPos = transform->GetPrePos();
	vector2s playerSize = GetSize(true);
	if (abs(playerPos.x - groundPos.x) > (groundSize.x + playerSize.x + 5.0f) * 0.5f)
	{
		velocityJumpX = SPRING_JUMPPOWER * (playerPos.x < groundPos.x ? 1 : -1);
		//Transform::AdjustPosition(this, groundPos, groundSize, false);
		SetvelocityY(1.0f);
		velocityJumpY = 0.0f;
	}
	else if(abs(playerPos.y - groundPos.y) > (groundSize.y + playerSize.y) * 0.5f)
	{
		velocityJumpY = SPRING_JUMPPOWER * (playerPos.y < groundPos.y ? 1 : -1);
		//Transform::AdjustPosition(this, groundPos, groundSize, false);
		SetvelocityY(1.0f);
		velocityJumpX = 0.0f;
	}
}

void Player::JumpBySpringUpdate(vector2s* pmovement)
{
	if (springJumped)
	{
		if(abs(velocityJumpY) > 1.0f)
			pmovement->y += velocityJumpY;
		else
 			pmovement->x += velocityJumpX;

		if (abs(velocityJumpY) < 1.0f)
			velocityJumpY = 0.0f;
		else
			velocityJumpY -= (velocityJumpY > 0 ? 0.2f : -0.2f);
		if(abs(velocityJumpX) < 1.0f)
			velocityJumpX = 0.0f;
		else
			velocityJumpX -= (velocityJumpX > 0 ? 0.2f : -0.2f);

		springTime--;
		springTimeSec--;
	}

	if (springJumped && springTime < 0)
	{
		springTime = SPRING_COUNTER;
		springJumped = false;
	}

	if (springTimeSec < 0)
	{
		springJumpedSec = false;
	}
}

void Player::MoveByBelt(float beltSpeed, bool dir)
{
	inAirJumpPoint = 1;

	SetinAir(false);

	SetPos(transform->GetPos() + vector3s(dir ? beltSpeed : -beltSpeed, 0.0f, 0.0f), true);

	//�x���g�R���x�A����~�肽��T�E���h���~�߂�
	PlaySound(MoveByBeltSoundNo, -1);
}

void Player::MovingGroundUpdate(float deltatime)
{
	if (onMovingGround && curGround != NULL)
	{

		//SetvelocityY(0.0f);
		//if (GetinAir())SetInAirDeceleration(true);
		//SetinAir(false);
		vector3s vec = ((MoveGround*)curGround)->GetVec();
		vec.z = 0.0f;
		float speed = ((MoveGround*)curGround)->GetSpeed() * deltatime;
		movement.x += vec.x;
		movement.y += vec.y;
	}
}

void Player::DamageStutasUpdate()
{
	if (beingDamaged)
	{
		if (--damagedTime < 0)
		{
			beingDamaged = false;
			damagedTime = DAMAGE_COUNTER;
		}
	}
}

//�������ǉ�
//==========================================

//�U�����ꂽ�����@�@by���E
void Player::Hurted(int power)
{
	Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
	hp -= power;
	if (m_winSign == false) {
		anim->SwitchAnim(E_Anim::Anim_Hurt, false, true);
	}
	m_canBeAttacked = false;

	if (m_sweatEffect != NULL) {
		m_sweatEffect->SetState(true);
	}

	PlaySound(PlayerDamageSoundNo, 0);

}



//������E����    �@by���E
void Player::CollectOnigiri(Onigiri* onigiri)
{
	hp += onigiri->GetHealth();
	GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Recover, NULL, vector3s(this->GetTransform()->GetPos().x, this->GetTransform()->GetPos().y + 50, this->GetTransform()->GetPos().z), vector3s(1, 0, 0));
	PlaySound(CollectItemSoundNo, 0);
}

//���h�q���E���������@��]�X�s�[�h�v���X    �@by���E
void Player::CollectPepper()
{

	if (bPepper)
	{
		sway_speed = 2.5;

		//�A�C�e�����Ԑ���
		pepper_time--;
		if (pepper_time <= 0)
		{
			sway_speed = 0;
			bPepper = false;
		}
		PlaySound(CollectItemSoundNo, 0);
	}
}

//���X�`���E���������@�o�����X������Ȃ�    �@by���E
void Player::CollectMisoSoup()
{
	if (bMisosoup)
	{

		beStable = true;
		//�A�C�e�����Ԑ���
		misosoup_time--;
		if (misosoup_time <= 0)
		{
			beStable = false;
			bMisosoup = false;
		}
		PlaySound(CollectItemSoundNo, 0);
	}

}

//�~�������E���������@�ړ��X�s�[�h�A�b�v    �@by���E
void Player::CollectPickledPlums()
{
	if (bPickledPlums)
	{
		beDush = true;

		//�A�C�e�����Ԑ���
		pickledplums_time--;
		if (pickledplums_time <= 0)
		{
			beDush = false;
			bPickledPlums = false;
		}
		PlaySound(CollectItemSoundNo, 0);
	}

}



//���鏈��    	   by���E	
void Player::Slide()
{
	if (m_winSign == false) {
		Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
		if (newTumbleFlag == false) {
			anim->SwitchAnim(E_Anim::Anim_Slide, true, true);
		}
		else {
			anim->SwitchAnim(E_Anim::Anim_Tumble, true, true);
		}
	}
}


/// <summary>
/// �v���C���[�̓����蔻��
/// </summary>
/// <param name="player"></param>
void SceneObjMgr::CheckPlayerCollision(Player* player) {
	if (player == NULL)return;
	vector3s playerPos = player->GetTransform()->GetPos();
	vector2s playerSize = player->GetSize(true);

	//��Q���Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);

				vector3s tempPos = iter->second->GetTransform()->GetPos();
				vector2s tempSize = iter->second->GetSize(true);

				vector3s tempVec = playerPos - tempPos;
				if (tempVec.y < 0 && abs(tempVec.x) < (tempSize.x + playerSize.x) * 0.5f) {
					player->SetCurGround(iter->second);
				}
			}
		}
	}
	//�A�C�X�Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Ice) != cellTypeDic.end()) {

		bool iceCollision = false;

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Ice];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->SetOnIce(true);
				iceCollision = true;
				break;
			}
		}
		if (iceCollision == false) {
			player->SetOnIce(false);
		}
	}
	//�ړ��n�ʂ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_MoveGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_MoveGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				//�ړ��n�ʂɏ���Ă邩�ǂ���
				player->SetOnMovingGround(true);

				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);

				vector3s tempPos = iter->second->GetTransform()->GetPos();
				vector2s tempSize = iter->second->GetSize(true);

				vector3s tempVec = playerPos - tempPos;
				if (tempVec.y < 0 && abs(tempVec.x) < (tempSize.x + playerSize.x) * 0.5f) {
					player->SetCurGround(iter->second);
				}
			}
		}
		////�S�[���Ƃ̓����蔻��
		//if (cellTypeDic.find(E_CellType::CellType_TargetPoint) != cellTypeDic.end()) {
		//	map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_TargetPoint];
		//	map<int, BaseCell*>::iterator iter;
		//	for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
		//		if (iter->second == NULL || iter->second->CheckState() == false) {
		//			continue;
		//		}
		//		if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

		//		if (Collider::CollisionDetect(player, iter->second)) {
		//			//clear
		//			//���̃X�e�[�W�i�V�[���j�ɐ؂�ւ�
		//			GameMgr::GetInstance()->sceneMgr->SwitchNextScene();
		//			break;
		//		}
		//	}
		//}
	}
	if (cellTypeDic.find(E_CellType::CellType_AppearGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_AppearGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				//�ړ��n�ʂɏ���Ă邩�ǂ���
				//player->SetOnMovingGround(true);

				//Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(player, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);

				vector3s tempPos = iter->second->GetTransform()->GetPos();
				vector2s tempSize = iter->second->GetSize(true);

				vector3s tempVec = playerPos - tempPos;
				if (tempVec.y < 0 && abs(tempVec.x) < (tempSize.x + playerSize.x) * 0.5f) {
					player->SetCurGround(iter->second);
				}
			}
		}
	}
	if (cellTypeDic.find(E_CellType::CellType_SpringGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_SpringGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				//�ړ��n�ʂɏ���Ă邩�ǂ���
				//player->SetOnMovingGround(true);


				EventCenter::GetInstance()->EventTrigger<void, BaseCell*>("SpringContactPlayer", iter->second);
				//Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				//Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);
				//((Player*)iter->second)->SetCurGround(iter->second);


				//Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);

				//vector3s tempPos = iter->second->GetTransform()->GetPos();
				//vector2s tempSize = iter->second->GetSize(true);

				//vector3s tempVec = playerPos - tempPos;
				//if (tempVec.y < 0 && abs(tempVec.x) < (tempSize.x + playerSize.x) * 0.5f) {
				//	player->SetCurGround(iter->second);
				//}
			}
		}
	}
	//�G�Ƃ̓����蔻��
	/*if (cellTypeDic.find(E_CellType::CellType_Enemy) != cellTypeDic.end()) {
		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Enemy];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->Hurted(15);
			}
		}
	}*/

	//���ɂ���Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Onigiri) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Onigiri];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->CollectOnigiri(((Onigiri*)iter->second));
				((Onigiri*)iter->second)->DelOnigiri();
				//iter->second->SetState(false);

			}
		}

	}

	//���h�q�Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Pepper) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Pepper];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->SetPepperUse(true);
				player->SetPepperTime(10 * 60);
				((Pepper*)iter->second)->DelPepper();
				//iter->second->SetState(false);

			}
		}

	}

	//���X�`�Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Miso_Soup) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Miso_Soup];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->SetMisosoupUse(true);
				player->SetMisosoupTime(10 * 60);
				((MisoSoup*)iter->second)->DelMisoSoup();
				//iter->second->SetState(false);

			}
		}

	}

	//�~�����Ƃ̓����蔻��
	if (cellTypeDic.find(E_CellType::CellType_Pickled_Plums) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Pickled_Plums];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;

			if (Collider::CollisionDetect(player, iter->second)) {
				player->SetPickledPlumsUse(true);
				player->SetPickledPlumsTime(10 * 60);
				((PickledPlums*)iter->second)->DelPickledPlums();
				//iter->second->SetState(false);

			}
		}

	}
}
//60~80���邢��-60~-80�x�𗐐������@by���E
float Player::GetRandomRot()
{

	if (rand() % 2)
	{
		return randRotM - rand() % 20;
	}
	else
	{
		return randRotP + rand() % 20;
	}
}

//��i�W�����v�̂Ƃ����݂̊p�x����X�Ƀo�����X����� by���E
float Player::GetRandomRotAir()
{

	if (tempRotZ >= 0)
	{
		return tempRotZ + 30 + rand() % 40;
	}
	else
	{
		return  tempRotZ - 30 - rand() % 40;
	}
}



void Player::SetParam(float deltatime)
{
	nowJumpStartFlag = false;	//�W�����v���n�߂��u�Ԃ�
	playerDeltaTime = deltatime;
	movement = vector2s(0, 0);
	tempPos = this->GetTransform()->GetPos();
	tempRotZ = this->GetTransform()->GetRot().z;
	playerMovementSign = false;

	tempCell = NULL;
	tempCell = GameMgr::GetInstance()->sceneObjMgr->GetTempCell();
}

void Player::changeRot()
{
	//�ړ������ɍ��킹�Č�����ς���
	if (GetinAir() == false) {
		if (movement.x > 0) {
			this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
		}
		else if (movement.x < 0) {
			this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
		}
	}
	//�v���C���[���X���̕�������]������
	this->transform->SetRot(vector3s(this->transform->GetRot().x, 0.0f, tempRotZ));
}

void Player::changePos()
{
	curPos = this->GetTransform()->GetPos();
	curPos.x += movement.x;
	curPos.y += movement.y;
}

void Player::SetAnim()
{
	//anim
	Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
	if (anim != NULL) {
		if (playerMovementSign) {
			anim->SwitchAnim(E_Anim::Anim_Move, false, true);
		}
		else if (playerMovementSign == false && newTumbleFlag == true) {
			anim->SwitchAnim(E_Anim::Anim_Slide, false, true);
		}
		else
		{
			anim->SwitchAnim(E_Anim::Anim_Idle, false, true);
		}
	}
}

void Player::reactionShoot()
{
	//�����ŉ�]����
	if (remainReactionRotZ > 0.0f && shootDirection == -1)
	{
		remainReactionRotZ -= PLAYER_REACTION_ROTZ_SPEED;
		tempRotZ -= PLAYER_REACTION_ROTZ_SPEED;

		if (remainReactionRotZ <= 0.0f)
		{
			tempRotZ += -remainReactionRotZ;
			remainReactionRotZ = 0.0f;
		}
	}


	if (remainReactionRotZ > 0.0f && shootDirection == 1)
	{
		remainReactionRotZ -= PLAYER_REACTION_ROTZ_SPEED;
		tempRotZ += PLAYER_REACTION_ROTZ_SPEED;

		if (remainReactionRotZ <= 0.0f)
		{
			tempRotZ += -remainReactionRotZ;
			remainReactionRotZ = 0.0f;
		}
	}
}


void Player::dush()
{
	//P�L�[�Ń_�b�V��
	if (/*GetKeyboardPress(DIK_LSHIFT) || beDush || IsButtonPressed(0, BUTTON_A)*/beDush)
	{
		dushCorrection += PLAYER_DUSH_ACCELERATION;
		if (dushCorrection >= PLAYER_DUSH_MAX_SPEED)
		{
			dushCorrection = PLAYER_DUSH_MAX_SPEED;
		}
		m_rushEffectTimePass += playerDeltaTime;
		if (m_rushEffectTimePass >= RUSH_EFFECT_INTERVAL) {
			m_rushEffectTimePass = 0.0f;
			Effect* effect = GameMgr::GetInstance()->effectMgr->CreateEffect(E_Effect::Effect_Rush, NULL, this->GetTransform()->GetPos(), vector3s(1, 0, 0));
			if (effect != NULL) {
				effect->SetRot(vector3s(this->GetTransform()->GetRot().x, 0.0f,this->GetTransform()->GetRot().z));
			}
		}
	}
	else
	{
		//P�L�[��������Ă��Ȃ���΃_�b�V�����Ȃ�
		dushCorrection = 0.0f;
		m_rushEffectTimePass = 0.0f;
	}

	//movement.x�Ƀ_�b�V���̕␳��������
	//movement.x *= (1.0f + dushCorrection);
	if (movement.x > 0) {
		SetPos(this->transform->GetPos() + vector3s((dushCorrection), 0, 0), false);
	}
	else if (movement.x < 0)
	{
		SetPos(this->transform->GetPos() + vector3s((-dushCorrection), 0, 0), false);

	}

}


void Player::move()
{
	if (canMove == false) return;

	if (GetKeyboardPress(DIK_A)) {
		movement.x -= PLAYER_MOVEMENT_SPEED * playerDeltaTime;

		if (tempCell != NULL) {
			//tempPos.x += movement.x;
			tempPos.x = this->GetTransform()->GetPos().x + movement.x;
			//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
				movement.x = 0;
				this->SetPos(tempCell->GetTransform()->GetPos(), true);

				if (GetinAir())SetInAirDeceleration(-1);
			}
			else {
				playerMovementSign = true;
			}
		}
		else {
			playerMovementSign = true;
		}

		direction = 1;
	}
	else if (GetKeyboardPress(DIK_D)) {
		movement.x += PLAYER_MOVEMENT_SPEED * playerDeltaTime;

		if (tempCell != NULL) {
			//tempPos.x += movement.x;
			tempPos.x = this->GetTransform()->GetPos().x + movement.x;
			playerMovementSign = true;
			//GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true, 0), 0);
			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
				movement.x = 0;
				this->SetPos(tempCell->GetTransform()->GetPos(), true);

				if (inAir) inAirDeceleration = 1;
			}
			else {
				playerMovementSign = true;
			}
		}
		else {
			playerMovementSign = true;
		}

		direction = -1;
	}
}

bool Player::CheckCanBeAttacked()
{
	return m_canBeAttacked;
}

void Player::Reset()
{
	SetPlayerLifeValue(100);
	sway_speed = 0;
	beStable = false;
	beDush = false;
	BaseCell::Reset();
}

void Player::SetWinSign(bool _winSign)
{
	m_winSign = _winSign;
	m_winTimePass = 0.0f;
}

void Player::UpdateEffect()
{
	if (m_sweatEffect != NULL) {
		vector3s rot = this->GetTransform()->GetRot();

		vector3s targetOffset = m_sweatEffectOffset;
		targetOffset.x = m_sweatEffectOffset.x * cosf(D3DXToRadian(rot.z)) - m_sweatEffectOffset.y * sinf(D3DXToRadian(rot.z));
		targetOffset.y = m_sweatEffectOffset.x * sinf(D3DXToRadian(rot.z)) + m_sweatEffectOffset.y * cosf(D3DXToRadian(rot.z));
		
		m_sweatEffect->SetRot(vector3s(rot.x, 0, rot.z));
		m_sweatEffect->SetPos(this->GetTransform()->GetPos() + targetOffset,true);
	}
}
