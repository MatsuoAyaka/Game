#pragma onece 

#include "GameObject.h"

#define ENEMY_JUMPPOWER			(-10.0f)
#define	ENEMY_MAX_MOVE_SPEED	(3.0f)
#define	ENEMY_MOVEMENT_SPEED	(2.0f)
#define ENEMY_GRAVITY			(0.05f)
#define ENEMY_SEARCHRANGE		(200.0f)
#define ENEMY_PASTA_SEARCHRANGE		(400.0f * 400.0f)
#define ENEMY_HIYOKO_PATROLRANGE (100.0f * 100.0f)
#define ENEMY_MONKEY_PATROLRANGE (100.0f * 100.0f)
#define ENEMY_CORNET_CHASESPEED (20.0f)
#define ENEMY_CORNET_CHASESPEED (20.0f)

//============================================
//以下　松尾担当
//#define BOSS_JUMPPOWER			(-18.0f)
#define BOSS_JUMPPOWER			(-27.0f)
#define BOSS_MOVEMENT_SPEED		(4.0f)
#define BOSS_STOP_MOVEMENT		(0.0f)
#define BOSS_GRAVITY			(11.0f)

#define BOSS_FLYLIMIT_RIGHT		(1200)
#define BOSS_FLYLIMIT_LEFT		(100)
#define BOSS_FLYLIMIT_UP		(100)
#define BOSS_HITHEAD_NUM		(3)
#define BOSS_HP					(15)
#define BOSS_ATK_NUM			(5)
//
//============================================


#define ENEMY_NEZUMI_SPEED (3.5f)
#define ENEMY_NEZUMI_CHASERANGE (500.0f)


class BaseCell;
class Effect;

enum E_EnemyType {
	Enemy_None = -1,
	Enemy_Monkey = 0,
	Enemy_Karasu = 1,
	Enemy_Nezumi = 2,
	Enemy_Hiyoko = 3,
	Enemy_Boss = 4,
};

enum E_EnemyStatus
{
	ENEMY_STS_PATROL = 0,		//パトロール中
	ENEMY_STS_JUMP = 1,			//ジャンプ中
	ENEMY_STS_MOVETOPLAYER = 2,	//ジャンプの間にプレイヤーへ移動
	ENEMY_STS_FALL = 3,			//落ちる
	ENEMY_STS_ATTACK = 4,		//攻撃中
	ENEMY_STS_GONE = 5,			//退去する
	ENEMY_STS_NONE = 6,			//終了処理

	STS_NUM = 7,
};

//============================================
//以下　松尾担当
enum E_BossStatus
{
	BOSS_STS_ATTACK = 0,	//突進攻撃
	BOSS_STS_JUMP = 1,		//ジャンプ攻撃
	BOSS_STS_FLY = 2,		//飛行
	BOSS_STS_SHOT = 3,		//ショット
	BOSS_STS_JUMPSHOT = 4,	//ジャンプ+ショット
	BOSS_STS_FALL = 5,		//重力処理（ジャンプのとき用）
	BOSS_STS_FALLOUT = 6,	//降下処理（飛行のとき用）
	BOSS_STS_WEAKENING = 7,	//弱っている状態（頭を踏める状態）

	BOSS_STS_NUM,
};
//============================================

#pragma region 

class Enemy :public BaseCell {
private:
	E_EnemyType enemyType;//敵の種類
	Enemy() = delete;

public:
	int		m_hp;
	int		m_damage;
	bool	m_useFlag;			//生きているか
	bool	m_useGravityFlag;	//重力フラグ
	bool	m_isGround;
	E_EnemyStatus	m_status = ENEMY_STS_NONE;	//状態

	vector2s m_movement;
	vector3s m_createPoint;
	Ground* m_ground = nullptr;

	Enemy(E_EnemyType eType);//インスタンス化するとき敵の種類を設定する必要がある
	
	~Enemy();

	virtual void Move();
	virtual void Hurt(int _damage);	//ダメージ
	virtual void Die();

	//セッター
	void SetHP(int hp) { m_hp = hp; }
	void SetCreatePoint(vector3s cPos) { m_createPoint = cPos; }
	void SetIsGround(bool _isGround) { m_isGround = _isGround; }
	void SetGroundBinded(Ground* _g) { m_ground = _g; }
	void SetStatus(E_EnemyStatus status) { m_status = status; }

	//ゲッター
	int	GetStatus() { return m_status; }
	int GetHP() { return m_hp; }
	int GetDamage() { return m_damage; }
	bool GetUseGravity() { return m_useGravityFlag; }	//重力が加わるものか

	void DoUpdate(float deltatime);

	E_EnemyType GetEnemyType() {
		return enemyType;
	}


	virtual void SetParent(BaseCell* _parent);
	virtual void Reset();
	virtual void Resize();
};

#pragma endregion enemy


#pragma region monkey
class Monkey :public Enemy {
private:
	float	m_fallSpeed = 0;			//重力
	float	m_fallAcc = ENEMY_GRAVITY * 3;			//重力加速度
	//E_EnemyStatus	m_status = ENEMY_STS_PATROL;	//猿の状態
	float	m_jumpPower = ENEMY_JUMPPOWER;		//Y方向の速度,下方向が+
	float	m_searchRange = ENEMY_PASTA_SEARCHRANGE;		//索敵範囲
	bool	m_muki = false;			//false:右向き true:左向き
	float	m_patrolRange = ENEMY_MONKEY_PATROLRANGE;

	//23.1 - 仮
	

public:
	Monkey();
	~Monkey() {};

	void DoUpdate(float deltatime);
	void PatrolRangeUpdate();
	//セッター
	//void SetStatus(E_EnemyStatus status) { m_status = status; }

	//ゲッター
	//int	GetStatus() { return m_status; }
	float GetSearchRange() { return m_searchRange; }

	//パトロール
	//→ actRange_r：右の行動限界　actRange_l:左の行動限界
	void Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l);
	void Patrol();

	void Search(vector3s* tempPos, vector3s playerPos, float searchRange);	//索敵処理
	bool Search(vector3s playerPos);	//索敵処理


	//void Jump(vector3s* tempPos, vector2s* movement);						//ジャンプ処理
	void Jump();						//ジャンプ処理
	void Gone(vector3s* tempPos, vector3s playerPos, vector2s* movement, BaseCell* monkey);	//退去処理
	void Gravity();					//重力処理

	void AdjustGroundPos(vector3s pos);			//今いるブロックから離れないように
};
#pragma endregion monkey

#pragma region hiyoko
class Hiyoko :public Enemy {
private:
	bool m_muki = false; //false:右向き true:左向き
	float m_patrolRange = ENEMY_HIYOKO_PATROLRANGE;
	float m_fallSpeed = 0.0f;
	float m_fallAcc = ENEMY_GRAVITY;
	//23.1 - temp
	int m_dirFactor = 1;
public:
	Hiyoko();
	~Hiyoko() {}

	//23.1 - temp
	Hiyoko(int _tempMethod);
	//セッター
	
	//ゲッター
	

	//パトロール
	//→ actRange_r：右の行動限界　actRange_l:左の行動限界
	void Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l);
	void Patrol();
	void PatrolRangeUpdate();

	void Gravity(vector2s* movement);
	void Gravity();
	void DoUpdate(float deltatime);
	void Reset();

};
#pragma endregion hiyoko

#pragma region karasu
class Karasu :public Enemy {
private:
	bool	m_muki = false;						//false:右向き true:左向き
	float	m_searchRange = 0;					//索敵範囲
	//E_EnemyStatus	m_status = ENEMY_STS_NONE;	//ステータス

	//23.1
	vector3s m_defaultPos = vector3s(0,0,0);
	float m_partolRangeRadius = SCREEN_WIDTH * 0.5f;
	vector2s m_partolRange = vector2s(0.0f, 0.0f);
public:
	Karasu();
	~Karasu() {}

	//セッター
	//void SetStatus(E_EnemyStatus status) { m_status = status; }
	//ゲッター
	//int GetStatus() { return m_status; }

	//パトロール
	//→ actRange_r：右の行動限界　actRange_l:左の行動限界
	void Patrol(vector2s* movement, vector3s* tempPos, float actRange_r, float actRange_l);
	void Search(vector3s* tempPos, vector3s playerPos, float searchRange);	//索敵処理
	void Gone(vector3s* tempPos, vector3s playerPos, vector2s* movement, BaseCell* karasu);	//退去処理
	void ContactAtk(vector3s tempPos, vector3s playerPos, vector2s* movement);		//突進してくる処理
	void DoUpdate(float deltatime);

	//23.1 - temp
	void Reset();
};
#pragma endregion karasu

#pragma region nezumi
class Nezumi :public Enemy {
private:
	int m_hitWait = 10;
	float m_default_y_offset = 500.0f;
	bool gone = false;
	int  goneTime = 180;
	Effect* m_effect = nullptr;
public:
	Nezumi();
	~Nezumi(){}

	void DoUpdate(float deltatime);
	//void Chase(vector3s tempPos, vector2s* movement);			//追跡してくる処理
	//void Gravity(vector2s* movement, BaseCell* tempCell);
	void Bite();
	//void MoveToNextBlock();
};
#pragma endregion nezumi

//=====================================================================================================
//以下　松尾担当
#pragma region boss
class Boss :public Enemy
{
private:
	BaseCell* enemybulletCell;
	BaseCell* enemybulletCell2;
	BaseCell* enemybulletCell3;
	EnemyBullet* enemybullet;
	EnemyBullet* enemybullet2;
	EnemyBullet* enemybullet3;
public:
	E_BossStatus	m_status = BOSS_STS_FLY;	//最初の攻撃はアタック
	float			m_gravity = BOSS_GRAVITY;	//重力
	float			m_jumpPower = BOSS_JUMPPOWER + m_gravity;	//ジャンプ力

	float			movespeed = BOSS_MOVEMENT_SPEED;			//移動スピード
	float			flyspeed = -8.0f;							//飛行スピード

	float			frame = 0;					//ショットのときのカウント用
	float			moveWaittime = 0;			//敵が動き出すまでのカウント用
	float			setWaittime = 0;			//ステータスを変えるまでのカウント用
	
	vector2s		movement;
	vector3s		tempPos;
	vector3s		playerPos;
	vector3s		player_Pos; //一時的
	vector3s		temp; //一時的

	
	bool			HitHead = false;			//プレイヤーに頭を踏まれたか
	bool			BreakBlockFlag = false;		//ブロック壊した時のフラグ（現在は未使用）
	int				HitHeadNum = 0;				//頭を踏まれた回数（ BOSS_HITHEAD_NUMの回数分、頭を踏んだら（現在　BOSS_HITHEAD_NUM = ３））
	
	//サウンド用
	int DragonAtkSoundNo;
	int DragonFlySoundNo;
	int DragonShotSoundNo;
	float m_flySoundTimePass = 0.0f;


	Boss();
	~Boss();

	//セッター 
	void SetStatus(E_BossStatus status) { m_status = status; }
	void SetHitHead(bool hithead) { HitHead = hithead; }
	void SetBreakBlock(bool breakblock) { BreakBlockFlag = breakblock; }
	
	//ゲッター
	int GetStatus() { return m_status; }
	bool GetHitHead() { return HitHead; }
	bool GetBreakBlock() { return BreakBlockFlag; }

	//各処理
	void DoUpdate(float deltatime)override;										//更新処理
	void ContactAtk(vector3s tempPos, vector2s* movement);						//突進攻撃
	void JumpAtk(vector3s tempPos, vector2s* movement, vector3s playerPos);		//ジャンプアタック
	void JumpShot(vector3s tempPos, vector2s* movement, vector3s playerPos);	//ジャンプショット
	void Gravity(vector2s* movement);					//重力処理
	void ShotAtk();										//弾を撃ってくる
	void FlyShotAtk();									//弾を撃ってくる（飛行のとき用）
	void FlyAtk(vector3s tempPos, vector2s* movement);	//空を飛ぶ
	void Fallout(vector2s* movement);					//降下
	void Weaking(vector3s tempPos, vector2s* movement);	//弱っている状態（頭を踏める）
	void Hurt(int damage);
	
	vector3s firePos;
	void Die();

};

#pragma endregion boss