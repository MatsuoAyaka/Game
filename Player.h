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
//追加陳 - 勝利アニメーション始まった何秒後にゲームクリア画面に移行
#define WIN_ANIMATION_TIME 3.0f

class Ground;
class Onigiri;
#pragma region player
class Player :public BaseCell {
private:
	vector3s preMouseMovement = vector3s();
	//================================================
	//自分が追加した

	float velocityY = 0;	//Y方向の速度,下方向が+
	float accelerationY;	//Y方向の加速度
	bool inAir = false;		//空中にいるか
	bool shoot = false;		//弾を撃つ瞬間だけtrue
	bool inAirDeceleration = false;	//空中にいるときに壁にぶつかったら移動スピードが落ちる
	int m_LifeValue;		//プレイヤーHP　　　byヨウ
	//bool onIce = false;		//アイスにいるか
	bool swayFlag = false;	//今揺れているか
	//float swaySpeed = 3;	//揺れのスピード
	//int swayDirection;		//揺れの方向(1 or -1)
	bool oldInAir = false;	//前回空中にいたか

	float tumbleTime = PLAYER_TUMBLE_TIME;		//倒れている時間をはかる
	bool newTumbleFlag = false;	//今倒れているか
	bool oldTumbleFlag = false;	//前回倒れていたか
	bool canMove = true;		//プレイヤーが動けるか,　今は使っていない

	int inAirJumpPoint = PLAYER_INAIRJUMP_LIMIT;	//空中ジャンプをあと何回できるか

	bool oldSpaceKey = false;
	bool newSpaceKey = false;
	bool spaceKeyTrigger = false;
	bool nowJumpStartFlag = false;
	bool InAirJumpFlag = false;	//二段ジャンプをした後か

	//by ヨウ
	//==================================
	const int healpoint = 1;
	int hp = 100;
	int pepper_time = 10 * 60;		//アイテム時間制限
	int misosoup_time = 10 * 60;	//アイテム時間制限
	int pickledplums_time = 10 * 60;//アイテム時間制限
	float sway_speed = 0;				//揺れの加速度
	bool bPepper = false;			//アイテム使用中かどうか
	bool bMisosoup = false;			//アイテム使用中かどうか
	bool bPickledPlums = false;     //アイテム使用中かどうか
	bool onIce = false;		        //アイスにいるか
	bool beStable = false;          //trueだったらバランス崩さない
	bool beDush = false;            //trueだったらダッシュする

	float randRotP = 60;			//乱数発生用プラス角度
	float randRotM = -60;			//乱数発生用マイナス角度

	//温追加===================
	int hitAwayCounter = 0;				// ネズミに攻撃する時逃げるためのカウンター
	//=========================================
	//自分が追加した
	//===============================================
	vector2s movement;//移動量
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
	BaseCell* tempCell = NULL;					//一時的なプレイヤー
	vector3s tempPos = vector3s(0, 0, 0);		//一時的な座標
	vector3s curPos = vector3s(0, 0, 0);		//最終的な移動の大きさ
	float tempRotZ = 0;							//一時的なZ軸回転
	bool playerMovementSign = false;			//プレイヤーが動いたか	
	int direction = 1;

	//空中に関する変数
	float slopeLimit = PLAYER_SLOPE_LIMIT;		//これ以上傾いていたら転倒する

	//弾の発射に関する変数
	float defaultReactionRotZ = PLAYER_DEFAULT_REACTION_ROTZ;	//弾の反動で回転する角度
	float remainReactionRotZ = 0.0f;			//あとどのくらい弾の反動で回転するかを示す角度
	int shootDirection = 0;						//弾を撃った時のプレイヤーの向き

		//ダッシュ
	float dushCorrection = 0.0f;				//ダッシュによるスピード補正

	//仮
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
	// //自分が追加した
	//ゲッター

	float GetvelocityY() { return velocityY; }
	float GetaccelerationY() { return accelerationY; }
	bool GetinAir() { return inAir; }
	bool GetShoot() { return shoot; }
	int GetInAirDeceleration() { return inAirDeceleration; }
	bool GetswayFlag() { return swayFlag; }
	void SetSpceKeyTrigger();
	//セッター
	void SetvelocityY(float argVelocityy) { 
		velocityY = F_Clamp(argVelocityy, -MAX_VELOCITY, MAX_VELOCITY);
	}
	void SetaccelerationY(float argAccelerationy) { accelerationY = argAccelerationy; }
	void SetinAir(bool argInAir) { inAir = argInAir; }
	void SetShoot(bool argShoot) { shoot = argShoot; }
	void SetInAirDeceleration(bool argInAirDeceleration) { inAirDeceleration = argInAirDeceleration; }
	void SetswayFlag(bool argSwayFlag) { swayFlag = argSwayFlag; }
	//
	void jump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos);	//ジャンプ
	void inAirJump(vector2s* pmovement, BaseCell* tempCell, vector3s* ptempPos);	//空中ジャンプ
	void recieveGravity(vector2s* pmovement, BaseCell* tempCell);			//重力の力を受ける処理
	void shootBullet();														//弾を撃つ
	void checkInAir(BaseCell* tempCell, vector2s* pmovement);
	void sway(float* prot, float movementx);	//揺れる
	void sway();	//揺れる
	void landing(float* prot);	//着地するときの処理
	void tumble(float* prot);	//転ぶ			

	//自分が追加した
	//=============================-------------------------
	//===早川追加(仮)==================================
	//プレイヤーのHPのゲッター・セッター
	int  GetPlayerLifeValue() { return hp; }
	void SetPlayerLifeValue(int hp) { this->hp = hp; }
	//=================================================

	void SwayByWind();								//風に吹かれる処理　				byシュウ
	void JumpBySpring(BaseCell* springGround);		//バネに跳ね返られる処理　			byシュウ
	void JumpBySpringUpdate(vector2s* pmovement);
	void MoveByBelt(float beltSpeed, bool dir);		//ベルトコンベア移動　				byシュウ
	void DamageStutasUpdate();						//無敵状態の更新					byシュウ

	bool GetOnMovingGround() { return onMovingGround; }
	void SetOnMovingGround(bool _b) { onMovingGround = _b; }
	void MovingGroundUpdate(float deltatime);						//移動地面に立つ時一緒に動く

	//by　ヨウ
	//=============================================================================
	void Hurted(int power);							                  //攻撃された処理　　
	void CollectOnigiri(Onigiri*);				                      //道具を拾った    
	void CollectPepper();				                              //道具を拾った    　
	void CollectMisoSoup();				                              //道具を拾った    　
	void CollectPickledPlums();			                              //道具を拾った    
	void Slide();   					                              //滑る処理 		
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

	float GetRandomRot();											  //60~80あるいは-60~-80度を乱数発生
	float GetRandomRotAir();										  //二段ジャンプのとき現在の角度から更にバランスを崩す

	//==========================================================
	//2022.11.10
	void SetCurGround(BaseCell* ground) {
		m_curGroundCounter = 0;
		curGround = ground;
	}

	void SetParam(float deltatime);	//色々な数値をセットする
	void changeRot();		//回転を最終的に変える
	void changePos();		//座標を最終的に変える
	void SetAnim();			//アニメーションのセット
	void reactionShoot();	//弾を発射した反動で回転
	void dush();			//ダッシュ
	void move();			//横方向の移動

	//23.1
	bool CheckCanBeAttacked();

	//温追加==========
	void SetHitAwayCounyer(int hitAway) { hitAwayCounter = hitAway; }
	int GetHitAwayCounter() { return hitAwayCounter; }


	//23.1.25
	void Reset();
	void SetWinSign(bool _winSign);
private:
	void UpdateEffect();
};
#pragma endregion player