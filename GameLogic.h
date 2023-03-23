#pragma once
#include "main.h"

#define UI_LIFE_NUM (5) //���C�t(���ɂ���)�̐�
#define UI_LIFE_POS_X (50)
#define UI_LIFE_POS_Y (50)
#define UI_BULLET_NUM (5)
//23.1 - ��
#define UI_NUM_COUNT 3
#define UI_SCORE_COUNT 5

#define NEXT_STAGE_ENEMY_COUNT 1 //�ǂꂭ�炢�G��|���΃{�X��ɓ��邩

using namespace std;

class DataManager;
class StageManager;

class ObjectPoolMgr;
class BaseCell;

class BaseCell;
class Player;
class Ray;
class Wind;
class SpringGround;
class BeltConveyor;
class Effect;
class EnemyBullet;

class GameState;
class Scene;
class Timer;
class UIMgr;

class FSMSystem;
class FSMState;
class FSMTriggerGroup;
class FSMStateTrigger;
struct FSMStateTriggerAction;
struct FSMStateAction;

class Bullet;
class Boss;
class Monkey;
class Nezumi;
class Hiyoko;
class Karasu;

struct Anim_Effect_Normal_CB;

enum E_CellType;
enum E_Scene;
enum E_Anim;

enum E_DrawLayer {
	DrawLayer_Bottom = 0,
	DrawLayer_Scene_3 = 1,
	DrawLayer_Scene_2 = 2,
	DrawLayer_Scene_1 = 3,
	DrawLayer_UI = 4,
	DrawLayer_Top = 5,
};

enum E_GameState {
	GameState_None,
	//�ǉ�����===============
	GameState_Game_Logo,
	//=======================
	GameState_Game_Title,
	GameState_Game_Normal,
	GameState_Game_Menu,
	GameState_Game_Lose,
	GameState_Game_End,
};

enum E_InputMode {
	InputMode_None,
	//�ǉ�����===============
	InputMode_Logo,
	//=======================
	InputMode_Title,
	InputMode_Game,
	InputMode_Game_Menu,
	InputMode_Game_Lose_Menu,
	InputMode_Game_Win_Menu,
};

enum E_Timer {
	Timer_Game_1,
	Timer_Game_2,
	Timer_UI,
};

//23.1
enum E_Effect {
	Effect_Hit,
	Effect_Jump,
	Effect_Rush,
	Effect_Recover,
	Effect_Sweat,
	Effect_Sakura,
	Effect_Button,
};

#pragma region manager
/// <summary>
/// �Q�[�����̑S���̃Q�[���I�u�W�F�N�g���Ǘ�����}�l�[�W���N���X
/// </summary>
class SceneObjMgr {
private:

	ObjectPoolMgr* m_objectPoolMgr;

	int cellIdCounter;//ID�J�E���^�[�i���ׂẴQ�[���I�u�W�F�N�g��ID�����Ȃ��悤�Ɏg���ϐ�

	map<int, map<int, BaseCell*>> drawCellDic;//�Q�[���I�u�W�F�N�g��`�揇���ɂ���ĊǗ����郊�X�g

	map<int, BaseCell*> cellDic;//�Q�[���I�u�W�F�N�g��ID�ɂ���ĊǗ����郊�X�g

	map<E_CellType, map<int, BaseCell*>> cellTypeDic;//�Q�[���I�u�W�F�N�g����ނɂ���ĊǗ����郊�X�g

	Player* m_player;//�v���C���[�Q�[���I�u�W�F�N�g
	Nezumi* m_nezumi;//�l�Y�~�Q�[���I�u�W�F�N�g

	BaseCell* tempCell;//�ꎞ�I�ȃQ�[���I�u�W�F�N�g�i�ꕔ�̓����蔻��Ɏg��

	Bullet* m_bullet[3];	//MAX_BULLET

	//23.1
	map<int,BaseCell*> m_deleteCellDic;

public:
	SceneObjMgr();
	~SceneObjMgr();
	void DoInit();
	void UnInit(bool deleteCell);//�I�������A���ׂẴQ�[���I�u�W�F�N�g���폜����

	void DoInitCell();//���ׂẴQ�[���I�u�W�F�N�g������������i�Q�[���V�[�������[�h����Ƃ��ɌĂяo�����
	/// <summary>
	/// ��{�֐�
	/// </summary>
	/// <param name="deltatime"></param>
	void DoUpdate(float deltatime);//���ׂẴQ�[���I�u�W�F�N�g�̃A�b�v�f�[�g����
	void DoLateUpdate(float deltatime);//��Ԗڂ̃A�b�v�f�[�g����
	void DoDraw();//���ׂẴQ�[���I�u�W�F�N�g�̕`�揈��
	
	int GetID() //�V�����Q�[���I�u�W�F�N�g�ɗB���ID��^����
	{
		return cellIdCounter++;
	}

	void RegisterCell(int id, BaseCell* cell,bool recordDefaultState);//�V�����Q�[���I�u�W�F�N�g��z��̒��ɕۑ�����@�]�@recordDefaultState : �Q�[���I�u�W�F�N�g�̏����̏�Ԃ�ۑ����邩�ǂ���
	void RegisterCell(BaseCell* cell, bool recordDefaultState);

	BaseCell* GetCell(int id);

	/// <summary>
	/// �����蔻��֐�
	/// </summary>
	/// <param name="baseObj"></param>
	void CheckPlayerCollision(Player* player);
	bool CheckTempCollision();

	void CheckBulletCollision(Bullet* pbullet);
	void CheckWindCollision(Wind* wind);
	void CheckSpringGroundCollision(SpringGround* springGround);
	void CheckBeltConveyorCollision(BeltConveyor* beltConveyor);
	//===================================
	//�ǉ�����
	void CheckBossCollision(Boss* boss);
	void CheckEnemyBulletCollision(EnemyBullet* penemybullet);
	//===================================

	Player* GetPlayer();//�v���C���[�Q�[���I�u�W�F�N�g���擾����
	Nezumi* GetNezumi();

	void InitPlayerObjState(vector3s defaultPos);//�v���C���[�Q�[���I�u�W�F�N�g������������
	void UninitPlayerObjState();

	/// <summary>
	/// �ꎞ�I�ȃQ�[���I�u�W�F�N�g�Ɋւ���֐�
	/// </summary>
	/// <returns></returns>
	BaseCell* GetTempCell();
	void SetTempCellParam(vector3s pos, vector3s rot, vector3s scale, vector2s colliderSize,int colliderDataIndex);

	void CheckMonkeyCollision(Monkey* monkey);
	void CheckKarasuCollision(Karasu* karasu);
	void CheckHiyokoCollision(Hiyoko* hiyoko);
	void CheckNezumiCollision(Nezumi* nezumi);

	//---------�������v�[��
	//---------chin2022.11.12
	BaseCell* CreateCell(E_CellType _cellType,int _count, ...);
	void RecycleCell(BaseCell* _cell);

	//23.1
	void DeleteCell(BaseCell* _cell);//�����I�u�W�F�N�g���폜����ł͂Ȃ��A�܂��z��̒��ɕۑ����āA�S�ẴA�b�v�f�[�g�������I��������ƈ�C�ɍ폜����

	//23.1.25
	//�@���h�J��������
	void InitCornetState(vector3s defaultPos);//�I�u�W�F�N�g������������
	void UninitCornetState();
private:
	void UnRegisterCell(int id);//�Q�[���I�u�W�F�N�g���폜����
	void UnRegisterCell(BaseCell* cell);
	void DeleteCell();
};

/// <summary>
/// �G�t�F�N�g���Ǘ�����N���X
/// </summary>
class EffectMgr {
private:
	//�J�����h����ʂ̃p�����[�^
	float cameraShakeFactor;//�h��̋���
	float cameraShakeTime;//��������
	float cameraShakeTimePass;//�o�ߎ���
	vector3s shakeTargetPos;//�h��̖ڕW�̈ʒu(�A�b�v�f�[�g�֐��̒��Ƀ����_���Ō��߂�

	//�V�[���؂�ւ��̃A�j���[�V�����̃p�����[�^
	BaseCell* sceneTransformBg;
	BaseCell* sceneTransformBg_2;
	BaseCell* sakuraFubuki;

public:
	bool cameraShakeSwitch;//�J�����h��X�C�b�`
	vector3s shakePosOffset;//���ۂȃJ�����̐V�����ʒu

	Anim_Effect_Normal_CB* hitEffect_end_cb;

public:
	EffectMgr();
	~EffectMgr();

	void DoInit();
	void UnInit();

	void DoUpdate(float deltatime);

	void SetCameraShake(float shakeTime, float shakeFactor);//�J�����h��̃p�����[�^��ݒ肷��

	//�V�[���؂�ւ��̃A�j���[�V����
	void ShowSceneTransformBG(bool show, bool swapIn);//�A�j���[�V�������g��
	bool CheckSceneTransformBGSwapInAnimDone();//�؂�ւ��̃A�j���[�V�����̏�Ԃ��`�F�b�N
	bool CheckSceneTransformBGSwapOutAnimDone();//�؂�ւ��̃A�j���[�V�����̏�Ԃ��`�F�b�N

	void ShowSakura();
	void HideSakura();
	//23.1
	Effect* CreateEffect(E_Effect effectType, BaseCell* followObj, vector3s pos, vector3s dir);
};

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class SceneMgr {
private:


	map<E_Scene, Scene*> sceneDic;//�V�[���̃C���X�^���X���X�g

	E_Scene defaultSceneType;
	E_Scene curSceneType;//���̃V�[��
	Scene* curScene;//���̃V�[���̃C���X�^���X

	E_Scene targetScene;//������؂�ւ��Ă����ڕW�̃V�[��
	bool reloadTargetScene;

	/// <summary>
	/// �؂�ւ��̃A�j���[�V�����̃p�����[�^
	/// </summary>
	bool switchSceneSign_1;
	bool switchSceneSign_2;
	bool switchSceneSign_3;

public:
	//2023.01.26
	//�ǉ���-BGM
	int m_sound_GameClear;
	int m_sound_GameOver;
	int m_sound_GamePlay;
	int m_sound_title;
public:

	StageManager* stageMgr;

	bool switchSceneSign;
public:
	SceneMgr();
	~SceneMgr();

	void DoInit();
	void UnInit();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void RegisterScene(Scene* s, bool setDefaultScene);
	void UnregisterScene(Scene* s);

	void SwitchScene(E_Scene sceneType, bool reload,bool activeTransformAnim);//�V�[����؂�ւ���@�]activeTransformAnim : �؂�ւ��̃A�j���[�V�������g�����ǂ���
	void SwitchScene_2(E_Scene sceneType, bool reload);//���ۂɃV�[����؂�ւ��鏈��
	void SwitchNextScene();//���̃V�[���ɐ؂�ւ���

	void ResetCurScene();//���̃V�[���������̏�Ԃɖ߂�

	void ClearSceneDic();

	E_Scene GetCurSceneType();

	vector3s GetSceneCameraPos();

	Scene* GetCurScene();
};

/// <summary>
/// �Q�[�����͂��Ǘ�����N���X
/// </summary>
class InputMgr {
private:
	bool inputState;//���̓X�C�b�`�i���͂ł��邩�ǂ���
	E_InputMode curInputMode;//���̓��̓��[�h�i�Q�[�����[�h�⃁�j���[���[�h�Ȃ�

public:

	InputMgr();
	~InputMgr();

	void DoUpdate(float deltatime);
	void SwitchInputMode(E_InputMode targetInputMode);//���̓��[�h��؂�ւ���

	void SetInputState(bool state);//���̓X�C�b�`
	E_InputMode GetCurInputMode();//���̓��̓��[�h���擾����
};


class UIMgr {
private:

	//map<E_UIContent, vector<UI*>> uiPools;

private:

	BaseCell* titleMenu;//�^�C�g��
	BaseCell* m_title;
	BaseCell* endMenu;//�Q�[���N���A
	BaseCell* resultEff;
	BaseCell* gameOver;//�Q�[���I�[�o�[
	BaseCell* scoreText;
	//===============================
	//�ǉ�����
	BaseCell* logoMenu; //���S���
	DrawData* teamLogo_drawData;	//���S����

	enum FadeStatus
	{
		FadeIn,
		FadeOut
	};

	int fadeStatus = FadeIn;
	//=====
		//bar�@�@�@�@�@�@by���E
	//============================
	BaseCell* bar;	//�o�[
	BaseCell* pIcon;//�v���C���[�A�C�R��
	BaseCell* eIcon;//�G�A�C�R��
	//============================


	//����ǉ�=========================
	BaseCell* uiLife[UI_LIFE_NUM];   //HP��UI(��)
	int       playerLife = 0;  //�v���C���[�̃��C�t�ۑ��p
	int       playerLifeMax;   //�v���C���[�̃��C�t�ő�l�ۑ��p
	int       uiLife_gap = 130;  //���C�t�̕\�������炷
	bool      Life_Drawflag[UI_LIFE_NUM]; //���C�t�\���t���O
	vector2s* campos;

	BaseCell* uiBullet[UI_BULLET_NUM];
	bool      Bullet_Drawflag[UI_BULLET_NUM];
	int       uiBullet_gap = 0;
	int       HpDecrease;
	int       PlyerLife_before;
	float     bulletPos;  // 01/23 ����ǉ�
	//=================================

	//23.1 - ��
	BaseCell* m_numArray[UI_NUM_COUNT];
	bool m_HUDState = false;
	BaseCell* m_icon;
	//���ǉ� score=========================
	BaseCell* m_score[UI_SCORE_COUNT];
	int m_enemyCounter = 0; //�|�����G�l�~�[�̃J�E���^�[
	int m_number = 0; // ��̓I�X�R�A���l
	int playerMaxPos = 0;
	int       score_gap = 80;  //�\�������炷
	float bar_y_offset = 300.0f;
public:
	UIMgr();
	~UIMgr();

	void DoInit();

	void ShowTitle(bool show);//�^�C�g����ʂ̕\��
	void ShowEndMenu(bool show);//�Q�[���N���A��ʂ̕\��
	void ShowGameOver(bool show);//�Q�[���I�[�o�[��ʂ̕\��

		//=========================================
	//�ǉ�����
	BaseCell* teamLogo;
	float alphateamLogo = 0.0;
	void ShowLogoMenu(bool show);	//���S��ʂ̕\��
	void DrawTeamLogo(bool show);	//���S��\��
	void DoUpdate(float deltatime);
	//�Z�b�^�[
	void SetFadeStatus(int status) { fadeStatus = status; }
	//=========================================
	//����ǉ�=========================
	void DoUpdate();
	void DrawLife(); //���̕\��
	void DrawUIBullet();
	//=================================
		//=====================
	//bar����      by���E
	void UpdateBar();
	void DrawBar(bool show);
	//========================
	//23.1 
	void ShowHUD(bool _show);

	// ���ǉ� score============
	void UpdateScore();
	void DrawScore(bool show);
	void ShowScore();

	void SetScoreNum(int pos) { playerMaxPos = pos; }

	void EnemyCounterAdd() { m_enemyCounter++; }
	int GetEnemyCounter() { return m_enemyCounter; }
	void SetEnemyCounter(int _count) {
		m_enemyCounter = _count;
	}

	//23.1.25��
	void ResetLifeUI();

};

/// <summary>
/// �Q�[���v���Z�X�Ǘ�����N���X�i�V���O���g���j
/// 
/// �]�O���[�o���V���O���g���ł���΁@�v���O�����̂ǂ̏ꏊ�ł��Ăяo�����B�@�g����F GameMgr::GetInstance()->�u�����o�[�ϐ����@OR�@�����o�[�֐����v
/// </summary>
class GameMgr {

private:
	static GameMgr* instance;

private:
	/// <summary>
	/// �Q�[���X�e�[�g�ϐ�
	/// </summary>
	map<E_GameState, GameState*> gameStateDic;//�Q�[���X�e�[�g���X�g�i�^�C�g���A�Q�[���X�^�[�g�A�Q�[���I�[�o�[�Ȃ�
	map<E_GameState, GameState*> subGameStateDic;//�T�u�Q�[���X�e�[�g���X�g�i�Q�[�����j���[�Ȃ�

	E_GameState defaultGameStateType;//�f�t�H���g�Q�[���X�e�[�g�i��ʓI�ɂ̓^�C�g����ݒ肷��
	E_GameState defaultSubGameStateType;//�f�t�H���g�@�T�u�Q�[���X�e�[�g

	E_GameState curGameStateType;//���̃X�e�[�g
	E_GameState curSubGameStateType;//���̃T�u�X�e�[�g
	GameState* curGameState;//���̃X�e�[�g�̃C���X�^���X
	GameState* curSubGameState;//���̃T�u�X�e�[�g�̃C���X�^���X
public:

	/// <summary>
	/// �}�l�[�W���̃C���X�^���X
	/// </summary>
	SceneMgr* sceneMgr;//�Q�[���V�[�����Ǘ�����}�l�[�W���̃C���X�^���X
	InputMgr* inputMgr;//�Q�[�����͂��Ǘ�����}�l�[�W���̃C���X�^���X
	SceneObjMgr* sceneObjMgr;//�Q�[���I�u�W�F�N�g���Ǘ�����}�l�[�W���̃C���X�^���X
	EffectMgr* effectMgr;//�G�t�F�N�g���Ǘ�����}�l�[�W���̃C���X�^���X
	UIMgr* uiMgr;//UI���Ǘ�����}�l�[�W���̃C���X�^���X
	Timer* timer;//�Q�[�����Ԃ��Ǘ�����N���X�̃C���X�^���X
	DataManager* dataMgr;
private:
	GameMgr();
	GameMgr(const GameMgr&) = delete;
	GameMgr& operator=(const GameMgr&) = delete;
public:
	~GameMgr();

	void DoInit();
	void DoUninit();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatine);

	void RegisterGameState(E_GameState gameStateType, GameState* gameState, bool setDefault);//�Q�[���X�e�[�g�C���X�^���X�����X�g���ɒǉ�����@- setDefault : �f�t�H���g�X�e�[�g�A�v���O���������s�����Ƃ������I�ɂ��̃X�e�[�g�ɐ؂�ւ���
	void RegisterSubGameState(E_GameState gameStateType, GameState* gameState, bool setDefault);

	void SwitchGameState(E_GameState gameState, bool reset);//�Q�[���X�e�[�g�C���X�^���X�����X�g���ɒǉ�����@- setDefault : �f�t�H���g�X�e�[�g�A�v���O���������s�����Ƃ������I�ɂ��̃X�e�[�g�ɐ؂�ւ���
	void SwitchSubGameState(E_GameState subGameState, bool reset);

	E_GameState GetCurGameState();//���̃X�e�[�g��ނ��擾����
	E_GameState GetCurSubGameState();//���̃T�u�X�e�[�g��ނ��擾����

public:
	//�V���O���g���������I�ɍ폜����邽�߂Ɏg���N���X
	class Deletor {
	public:
		~Deletor() {
			if (GameMgr::instance != NULL) {
				delete GameMgr::instance;
			}
		}
	};
	static Deletor deletor;
public:
	static GameMgr* GetInstance() //GameMgr�C���X�^���X���擾����
	{
		if (instance == NULL) {
			instance = new GameMgr();
		}

		return instance;
	}
};

#pragma endregion manager


#pragma region game_state
/// <summary>
/// �Q�[���X�e�[�g�e�N���X�i���ۃN���X�j
/// </summary>
class GameState {
protected:
	E_GameState gameStateType;
public:
	GameState();
	GameState(E_GameState state);
	virtual ~GameState();

	virtual void DoInit() = 0;
	virtual void Uninit() = 0;

	virtual void DoUpdate(float deltatime);
};

//==========================================
//�ǉ�����

class GameState_Logo :public GameState {
private:

public:
	GameState_Logo();
	~GameState_Logo();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};
//==========================================

class GameState_Title :public GameState {
private:

public:
	GameState_Title();
	~GameState_Title();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};

class GameState_Game :public GameState {
private:

public:
	GameState_Game();
	~GameState_Game();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};


class GameState_Menu :public GameState {
private:

public:
	GameState_Menu();
	~GameState_Menu();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};

class GameState_Lose :public GameState {
private:

public:
	GameState_Lose();
	~GameState_Lose();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};

class GameState_End :public GameState {
private:

public:
	GameState_End();
	~GameState_End();

	void DoUpdate(float deltatime);

	void DoInit();
	void Uninit();
};

#pragma endregion game_state

class Timer {
public:
	float gameTimer_1;//game�Aplayer
	float gameTimer_2;
	float uiTimer;//ui timer
public:
	Timer();
	~Timer();

	void SetTimer(E_Timer timerType, bool limitForwar);
	void SetTimer(E_Timer timerType, float value);

	void DoUpdate(float deltatime);

	void Reset();
};

#pragma region fsm
enum E_FSMState {
	FSMState_Idle,
	FSMState_MoveToPlayer,
	FSMState_Attack,
	FSMState_Dmg,
};

enum E_CheckTriggerType {
	CheckTriggerType_None,
	CheckTriggerType_AllTrue,
	CheckTriggerType_AllFalse,
	CheckTriggerType_OneTrue,
	CheckTriggerType_OneFalse,

	CheckTriggerType_AlwaysTrue,
	CheckTriggerType_AlwaysFalse,
};

class FSMSystem {
private:
	//ai component
	//AI* parent;
	BaseCell* parent;
private:

	map<E_FSMState, FSMState*> stateDic;

	E_FSMState curStateType;
	FSMState* curState;

	E_FSMState defaultStateType;

public:
	FSMSystem();
	FSMSystem(BaseCell* p);

	~FSMSystem();

	void DoInit();

	void RegisterState(E_FSMState stateType, FSMState* state, bool setDefaultState);

	void DoUpdate(float deltatime);

	void SwitchState(E_FSMState targetStateType, bool resetState);

	void SetParent(BaseCell* p);
	BaseCell* GetParent();
};

class FSMState {
private:

	bool enable;

	FSMSystem* parentSystem;

	E_FSMState stateType;
	FSMStateAction* action;

private:
	map<E_FSMState, FSMTriggerGroup*> triggerDic;

private:
	FSMState();

	void CheckStateTrigger(float deltatime);


public:
	FSMState(FSMSystem* parent);


	~FSMState();

	void DoInit();

	void DoUpdate(float deltatime);

	void SetAction(FSMStateAction* a);

	void DoAction(float deltatime);

	void StartState();

	void EndState();

	void PauseState(bool state);

	BaseCell* GetParentCell();

	void RegisterTrigger(E_FSMState targetState, FSMTriggerGroup* triggerGroup);
};

class FSMTriggerGroup {
private:
	FSMSystem* parentSystem;
	//priority :TODO

	E_CheckTriggerType checkTriggerType;
	vector<FSMStateTrigger*> triggerArray;
private:

	FSMTriggerGroup();
public:

	FSMTriggerGroup(E_CheckTriggerType checkType, FSMSystem* parent);

	~FSMTriggerGroup();

	E_CheckTriggerType GetCheckType();

	vector<FSMStateTrigger*>& GetTriggerArray();

	void AddTrigger(FSMStateTrigger* trigger);

	bool CheckTrigger(float deltatime);

	void ResetTriggerState();
};

class FSMStateTrigger {
private:
	FSMState* parentState;
	FSMStateTriggerAction* triggerAction;
public:
	FSMStateTrigger();
	~FSMStateTrigger();

	void SetTriggerParam(FSMState* parent, FSMStateTriggerAction* action);

	bool Check(FSMSystem* parentSystem, float deltatime);
	void ResetTriggerState();
};


#pragma region action_method

#pragma region enemy_base_action

struct FSMStateAction {
	bool init = false;
	E_Anim setAnim;
	bool actionDone;
	FSMStateAction();
	~FSMStateAction() {

	}
	virtual void operator()(BaseCell* parentCell, float deltatime) = 0;
	virtual void Reset();
};

struct FSMState_Base_Action_Move2Player :public FSMStateAction {

	float moveSpeed;
	FSMState_Base_Action_Move2Player() :moveSpeed(1.0f) {

	}
	void operator()(BaseCell* parentCell, float deltatime);
};

struct FSMState_Base_Action_Idle :public FSMStateAction {
	void operator()(BaseCell* parentCell, float deltatime);
};

struct FSMState_Base_Action_Dmg :public FSMStateAction {
	void operator()(BaseCell* parentCell, float delattime);
};

struct FSMState_Base_Action_Partrol : public FSMStateAction {
	void operator()(BaseCell* parentCell, float deltatime);
};


#pragma endregion enemy_base_action

#pragma region enemy_skirmisher_action

#pragma endregion enemy_skirmisher_action

#pragma endregion action_method

#pragma region trigger_method

struct FSMStateTriggerAction {
	bool init = false;
	virtual bool operator()(BaseCell* parentCell, float deltatime) = 0;
	virtual void Reset();
};

struct FSMStateTrigger_Base_Trigger_IdleTime :public FSMStateTriggerAction {
	float idleTime;
	float timePass = 0;
	FSMStateTrigger_Base_Trigger_IdleTime() :idleTime(0.0f), timePass(0.0f) {

	}
	FSMStateTrigger_Base_Trigger_IdleTime(const FSMStateTrigger_Base_Trigger_IdleTime& src) {
		this->idleTime = src.idleTime;
		this->timePass = src.timePass;
	}

	bool operator()(BaseCell* parentCell, float deltatime);
};

struct FSMStateTrigger_Base_Trigger_Dist2Player :public FSMStateTriggerAction {
	float dist;
	FSMStateTrigger_Base_Trigger_Dist2Player() :dist(0.0f) {

	}
	FSMStateTrigger_Base_Trigger_Dist2Player(const FSMStateTrigger_Base_Trigger_Dist2Player& src) {
		this->dist = src.dist;
	}
	bool operator()(BaseCell* parentCell, float deltatime);
};

struct FSMStateTrigger_Base_Trigger_TakeDmg :public FSMStateTriggerAction {

	FSMStateTrigger_Base_Trigger_TakeDmg() {

	}
	FSMStateTrigger_Base_Trigger_TakeDmg(const FSMStateTrigger_Base_Trigger_TakeDmg& src) {

	}

	bool operator()(BaseCell* parentCell, float deltatime);
};

#pragma endregion trigger_method


#pragma endregion fsm
