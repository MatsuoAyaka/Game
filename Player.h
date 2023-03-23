#pragma once
#include "main.h"

using namespace std;

#define PLAYER_TUMBLE_TIME 60
#define PLAYER_INAIRJUMP_LIMIT 1
#define PLAYER_ROTATION_SPEED (3.5f)
#define PLAYER_MOVEMENT_SPEED (280.0f)
#define PLAYER_JUMP_POWER (-12)
#define PLAYER_HP 100
#define GRAVITY_ACCELERATION_DEGREES 1
#define WIND_RANDOMANGLE (30)
#define WIND_RANDOMMOVE (20)
#define SPRING_JUMPPOWER (-16)
#define SPRING_COUNTER (20)
#define SPRING_COUNTER_SECOND (3)
#define WIND_COUNTER (20)
#define DAMAGE_COUNTER (100)

#define GROUND_TIME_COUNTER 3

//23.1
#define MAX_VELOCITY 20
#define BE_ATTACKED_RECOVERY_TIME 1.5f

#define RUSH_EFFECT_INTERVAL 0.08f

//23.1.25
//�ǉ��� - �����A�j���[�V�����n�܂������b��ɃQ�[���N���A��ʂɈڍs
#define WIN_ANIMATION_TIME 3.0f

class Ground;
class Onigiri;
#pragma region player
class Player :public BaseCell {
private:
	vector3s preMouseMovement = vector3s();
	//================================================
	//�������ǉ�����

	float velocityY = 0;	//Y�����̑��x,��������+
	float accelerationY;	//Y�����̉����x
	bool inAir = false;		//�󒆂ɂ��邩
	bool shoot = false;		//�e�����u�Ԃ���true
	bool inAirDeceleration = false;	//�󒆂ɂ���Ƃ��ɕǂɂԂ�������ړ��X�s�[�h��������
	int m_LifeValue;		//�v���C���[HP�@�@�@by���E
	//bool onIce = false;		//�A�C�X�ɂ��邩
	bool swayFlag = false;	//���h��Ă��邩
	//float swaySpeed = 3;	//�h��̃X�s�[�h
	//int swayDirection;		//�h��̕���(1 or -1)
	bool oldInAir = false;	//�O��󒆂ɂ�����

	float tumbleTime = PLAYER_TUMBLE_TIME;		//�|��Ă��鎞�Ԃ��͂���
	bool newTumbleFlag = false;	//���|��Ă��邩
	bool oldTumbleFlag = false;	//�O��|��Ă�����
	bool canMove = true;		//�v���C���[�������邩,�@���͎g���Ă��Ȃ�

	int inAirJumpPoint = PLAYER_INAIRJUMP_LIMIT;	//�󒆃W�����v�����Ɖ���ł��邩

	bool oldSpaceKey = false;
	bool newSpaceKey = false;
	bool spaceKeyTrigger = false;
	bool nowJumpStartFlag = false;
	bool InAirJumpFlag = false;	//��i�W�����v�������ォ

	//by ���E
	//==================================
	const int healpoint = 1;
	int hp = 100;
	int pepper_time = 10 * 60;		//�A�C�e�����Ԑ���
	int misosoup_time = 10 * 60;	//�A�C�e�����Ԑ���
	int pickledplums_time = 10 * 60;//�A�C�e�����Ԑ���
	float sway_speed = 0;				//�h��̉����x
	bool bPepper = false;			//�A�C�e���g�p�����ǂ���
	bool bMisosoup = false;			//�A�C�e���g�p�����ǂ���
	bool bPickledPlums = false;     //�A�C�e���g�p�����ǂ���
	bool onIce = false;		        //�A�C�X�ɂ��邩
	bool beStable = false;          //true��������o�����X�����Ȃ�
	bool beDush = false;            //true��������_�b�V������

	float randRotP = 60;			//���������p�v���X�p�x
	float randRotM = -60;			//���������p�}�C�i�X�p�x

	//���ǉ�===================
	int hitAwayCounter = 0;				// �l�Y�~�ɍU�����鎞�����邽�߂̃J�E���^�[
	//=========================================
	//�������ǉ�����
	//===============================================
	vector2s movement;//�ړ���
	float tempSpeed=0;
	const float max_speed = 100;
	const float min_speed = -100;
	bool springJumped = false;
	bool springJumpedSec = false;
	int springTime = SPRING_COUNTER;
	int springTimeSec = SPRING_COUNTER_SECOND;
	//2022.11.10
	BaseCell* curGround;

	//2022.11.21
	bool beingDamaged = false;
	int damagedTime = DAMAGE_COUNTER;
	bool onMovingGround = false;

	//2022.11.25
	int m_groundCounter = GROUND_TIME_COUNTER;
	int m_curGroundCounter = 0;

	//2022.11.26
	int windCounter = WIND_COUNTER;

	//2022.12.01
	float velocityJumpX = 0.0f;
	float velocityJumpY = 0.0f;
	float playerDeltaTime = 0;
	BaseCell* tempCell = NULL;					//�ꎞ�I�ȃv���C���[
	vector3s tempPos = vector3s(0, 0, 0);		//�ꎞ�I�ȍ��W
	vector3s curPos = vector3s(0, 0, 0);		//�ŏI�I�Ȉړ��̑傫��
	float tempRotZ = 0;							//�ꎞ�I��Z����]
	bool playerMovementSign = false;			//�v���C���[����������	
	int direction = 1;

	//�󒆂Ɋւ���ϐ�
	float slopeLimit = PLAYER_SLOPE_LIMIT;		//����ȏ�X���Ă�����]�|����

	//�e�̔��˂Ɋւ���ϐ�
	float defaultReactionRotZ = PLAYER_DEFAULT_REACTION_ROTZ;	//�e�̔����ŉ�]����p�x
	float remainReactionRotZ = 0.0f;			//���Ƃǂ̂��炢�e�̔����ŉ�]���邩�������p�x
	int shootDirection = 0;						//�e�����������̃v���C���[�̌���

		//�_�b�V��
	float dushCorrection = 0.0f;				//�_�b�V���ɂ��X�s�[�h�␳

	//��
	vector3s bulletRot = vector3s(0, 0, 0);
	vector3s bulletStPos = vector3s(0, 0, 0);
	vector3s bulletEdPos = vector3s(0, 0, 0);

	//23.1
	bool m_canBeAttacked = true;
	float m_beAttackedRecoveryTimePass = 0.0f;
	BaseCell* m_sweatEffect;
	vector3s m_sweatEffectOffset = vector3s(0.0f, -10.0f, 0.0f);
	float m_rushEffectTimePass = 0.0f;
	int BulletSoundNo;
	int CollectItemSoundNo;
	int JumpSoundNo;
	int JumpBySpringSoundNo;
	int MoveByBeltSoundNo;
	int SwayByWindSoundNo;
	int PlayerDamageSoundNo;

public:
	bool inputLock = false;
	bool start = false;
	//23.01.25
	bool m_winSign = false;
	float m_winTimePass = 0.0f;
public:
	Player();
	~Player();

	void DoUpdate(float deltatime);

	void Input(float deltatime);

	void SetState(bool state);

	//========================================================
	// //�������ǉ�����
	//�Q�b�^�[

	float GetvelocityY() { return velocityY; }
	float GetaccelerationY() { return accelerationY; }
	bool GetinAir() { return inAir; }
	bool GetShoot() { return shoot; }
	int GetInAirDeceleration() { return inAirDeceleration; }
	bool GetswayFlag() { return swayFlag; }
	void SetSpceKeyTrigger();
	//�Z�b�^�[
	void SetvelocityY(float argVelocityy) { 
		velocityY = F_Clamp(argVelocityy, -MAX_VELOCITY, MAX_VELOCITY);
	}
	void SetaccelerationY(float argAccelerationy) { accelerationY = argAccelerationy; }
	void SetinAir(bool argInAir) { inAir = argInAir; }
	void SetShoot(bool argShoot) { shoot = argShoot; }
	void SetInAirDeceleration(bool argInAirDeceleration) { inAirDeceleration = argInAirDeceleration; }
	void SetswayFlag(bool argSwayFlag) { swayFlag = argSwayFlag; }
	//
	void jump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos);	//�W�����v
	void inAirJump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos);	//�󒆃W�����v
	void recieveGravity(vector2s* pmovement, BaseCell* tempCell);			//�d�̗͂͂��󂯂鏈��
	void shootBullet();														//�e������
	void checkInAir(BaseCell* tempCell, vector2s* pmovement);
	void sway(float* prot, float movementx);	//�h���
	void sway();	//�h���
	void landing(float* prot);	//���n����Ƃ��̏���
	void tumble(float* prot);	//�]��			

	//�������ǉ�����
	//=============================-------------------------
	//===����ǉ�(��)==================================
	//�v���C���[��HP�̃Q�b�^�[�E�Z�b�^�[
	int  GetPlayerLifeValue() { return hp; }
	void SetPlayerLifeValue(int hp) { this->hp = hp; }
	//=================================================

	void SwayByWind();								//���ɐ�����鏈���@				by�V���E
	void JumpBySpring(BaseCell* springGround);		//�o�l�ɒ��˕Ԃ��鏈���@			by�V���E
	void JumpBySpringUpdate(vector2s* pmovement);
	void MoveByBelt(float beltSpeed, bool dir);		//�x���g�R���x�A�ړ��@				by�V���E
	void DamageStutasUpdate();						//���G��Ԃ̍X�V					by�V���E

	bool GetOnMovingGround() { return onMovingGround; }
	void SetOnMovingGround(bool _b) { onMovingGround = _b; }
	void MovingGroundUpdate(float deltatime);						//�ړ��n�ʂɗ����ꏏ�ɓ���

	//by�@���E
	//=============================================================================
	void Hurted(int power);							                  //�U�����ꂽ�����@�@
	void CollectOnigiri(Onigiri*);				                      //������E����    
	void CollectPepper();				                              //������E����    �@
	void CollectMisoSoup();				                              //������E����    �@
	void CollectPickledPlums();			                              //������E����    
	void Slide();   					                              //���鏈�� 		
	void SetPepperUse(bool use) { bPepper = use; }
	void SetMisosoupUse(bool use) { bMisosoup = use; }
	void SetPickledPlumsUse(bool use) { bPickledPlums = use; }
	void SetPepperTime(int time) { pepper_time = time; }
	void SetMisosoupTime(int time) { misosoup_time = time; }
	void SetPickledPlumsTime(int time) { pickledplums_time = time; }
	void SetOnIce(bool onice) { onIce = onice; }
	bool GetOnIce() { return onIce; }
	int  GetHp() { return m_LifeValue; }
	void SetHp(int hp) { m_LifeValue = hp; }

	float GetRandomRot();											  //60~80���邢��-60~-80�x�𗐐�����
	float GetRandomRotAir();										  //��i�W�����v�̂Ƃ����݂̊p�x����X�Ƀo�����X�����

	//==========================================================
	//2022.11.10
	void SetCurGround(BaseCell* ground) {
		m_curGroundCounter = 0;
		curGround = ground;
	}

	void SetParam(float deltatime);	//�F�X�Ȑ��l���Z�b�g����
	void changeRot();		//��]���ŏI�I�ɕς���
	void changePos();		//���W���ŏI�I�ɕς���
	void SetAnim();			//�A�j���[�V�����̃Z�b�g
	void reactionShoot();	//�e�𔭎˂��������ŉ�]
	void dush();			//�_�b�V��
	void move();			//�������̈ړ�

	//23.1
	bool CheckCanBeAttacked();

	//���ǉ�==========
	void SetHitAwayCounyer(int hitAway) { hitAwayCounter = hitAway; }
	int GetHitAwayCounter() { return hitAwayCounter; }


	//23.1.25
	void Reset();
	void SetWinSign(bool _winSign);
private:
	void UpdateEffect();
};
#pragma endregion player