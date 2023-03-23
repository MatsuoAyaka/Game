#pragma once
#include "main.h"

#define UI_LIFE_NUM (5) //ライフ(おにぎり)の数
#define UI_LIFE_POS_X (50)
#define UI_LIFE_POS_Y (50)
#define UI_BULLET_NUM (5)
//23.1 - 仮
#define UI_NUM_COUNT 3
#define UI_SCORE_COUNT 5

#define NEXT_STAGE_ENEMY_COUNT 1 //どれくらい敵を倒せばボス戦に入るか

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
	//追加松尾===============
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
	//追加松尾===============
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
/// ゲーム内の全部のゲームオブジェクトを管理するマネージャクラス
/// </summary>
class SceneObjMgr {
private:

	ObjectPoolMgr* m_objectPoolMgr;

	int cellIdCounter;//IDカウンター（すべてのゲームオブジェクトのIDが被らないように使う変数

	map<int, map<int, BaseCell*>> drawCellDic;//ゲームオブジェクトを描画順序によって管理するリスト

	map<int, BaseCell*> cellDic;//ゲームオブジェクトをIDによって管理するリスト

	map<E_CellType, map<int, BaseCell*>> cellTypeDic;//ゲームオブジェクトを種類によって管理するリスト

	Player* m_player;//プレイヤーゲームオブジェクト
	Nezumi* m_nezumi;//ネズミゲームオブジェクト

	BaseCell* tempCell;//一時的なゲームオブジェクト（一部の当たり判定に使う

	Bullet* m_bullet[3];	//MAX_BULLET

	//23.1
	map<int,BaseCell*> m_deleteCellDic;

public:
	SceneObjMgr();
	~SceneObjMgr();
	void DoInit();
	void UnInit(bool deleteCell);//終了処理、すべてのゲームオブジェクトを削除する

	void DoInitCell();//すべてのゲームオブジェクトを初期化する（ゲームシーンをロードするときに呼び出される
	/// <summary>
	/// 基本関数
	/// </summary>
	/// <param name="deltatime"></param>
	void DoUpdate(float deltatime);//すべてのゲームオブジェクトのアップデート処理
	void DoLateUpdate(float deltatime);//二番目のアップデート処理
	void DoDraw();//すべてのゲームオブジェクトの描画処理
	
	int GetID() //新しいゲームオブジェクトに唯一のIDを与える
	{
		return cellIdCounter++;
	}

	void RegisterCell(int id, BaseCell* cell,bool recordDefaultState);//新しいゲームオブジェクトを配列の中に保存する　‐　recordDefaultState : ゲームオブジェクトの初期の状態を保存するかどうか
	void RegisterCell(BaseCell* cell, bool recordDefaultState);

	BaseCell* GetCell(int id);

	/// <summary>
	/// 当たり判定関数
	/// </summary>
	/// <param name="baseObj"></param>
	void CheckPlayerCollision(Player* player);
	bool CheckTempCollision();

	void CheckBulletCollision(Bullet* pbullet);
	void CheckWindCollision(Wind* wind);
	void CheckSpringGroundCollision(SpringGround* springGround);
	void CheckBeltConveyorCollision(BeltConveyor* beltConveyor);
	//===================================
	//追加松尾
	void CheckBossCollision(Boss* boss);
	void CheckEnemyBulletCollision(EnemyBullet* penemybullet);
	//===================================

	Player* GetPlayer();//プレイヤーゲームオブジェクトを取得する
	Nezumi* GetNezumi();

	void InitPlayerObjState(vector3s defaultPos);//プレイヤーゲームオブジェクトを初期化する
	void UninitPlayerObjState();

	/// <summary>
	/// 一時的なゲームオブジェクトに関する関数
	/// </summary>
	/// <returns></returns>
	BaseCell* GetTempCell();
	void SetTempCellParam(vector3s pos, vector3s rot, vector3s scale, vector2s colliderSize,int colliderDataIndex);

	void CheckMonkeyCollision(Monkey* monkey);
	void CheckKarasuCollision(Karasu* karasu);
	void CheckHiyokoCollision(Hiyoko* hiyoko);
	void CheckNezumiCollision(Nezumi* nezumi);

	//---------メモリプール
	//---------chin2022.11.12
	BaseCell* CreateCell(E_CellType _cellType,int _count, ...);
	void RecycleCell(BaseCell* _cell);

	//23.1
	void DeleteCell(BaseCell* _cell);//すぐオブジェクトを削除するではなく、まず配列の中に保存して、全てのアップデート処理が終わったあと一気に削除する

	//23.1.25
	//陳　ヤドカリ初期化
	void InitCornetState(vector3s defaultPos);//オブジェクトを初期化する
	void UninitCornetState();
private:
	void UnRegisterCell(int id);//ゲームオブジェクトを削除する
	void UnRegisterCell(BaseCell* cell);
	void DeleteCell();
};

/// <summary>
/// エフェクトを管理するクラス
/// </summary>
class EffectMgr {
private:
	//カメラ揺れ効果のパラメータ
	float cameraShakeFactor;//揺れの強さ
	float cameraShakeTime;//持続時間
	float cameraShakeTimePass;//経過時間
	vector3s shakeTargetPos;//揺れの目標の位置(アップデート関数の中にランダムで決める

	//シーン切り替えのアニメーションのパラメータ
	BaseCell* sceneTransformBg;
	BaseCell* sceneTransformBg_2;
	BaseCell* sakuraFubuki;

public:
	bool cameraShakeSwitch;//カメラ揺れスイッチ
	vector3s shakePosOffset;//実際なカメラの新しい位置

	Anim_Effect_Normal_CB* hitEffect_end_cb;

public:
	EffectMgr();
	~EffectMgr();

	void DoInit();
	void UnInit();

	void DoUpdate(float deltatime);

	void SetCameraShake(float shakeTime, float shakeFactor);//カメラ揺れのパラメータを設定する

	//シーン切り替えのアニメーション
	void ShowSceneTransformBG(bool show, bool swapIn);//アニメーションを使う
	bool CheckSceneTransformBGSwapInAnimDone();//切り替えのアニメーションの状態をチェック
	bool CheckSceneTransformBGSwapOutAnimDone();//切り替えのアニメーションの状態をチェック

	void ShowSakura();
	void HideSakura();
	//23.1
	Effect* CreateEffect(E_Effect effectType, BaseCell* followObj, vector3s pos, vector3s dir);
};

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class SceneMgr {
private:


	map<E_Scene, Scene*> sceneDic;//シーンのインスタンスリスト

	E_Scene defaultSceneType;
	E_Scene curSceneType;//今のシーン
	Scene* curScene;//今のシーンのインスタンス

	E_Scene targetScene;//今から切り替えていく目標のシーン
	bool reloadTargetScene;

	/// <summary>
	/// 切り替えのアニメーションのパラメータ
	/// </summary>
	bool switchSceneSign_1;
	bool switchSceneSign_2;
	bool switchSceneSign_3;

public:
	//2023.01.26
	//追加陳-BGM
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

	void SwitchScene(E_Scene sceneType, bool reload,bool activeTransformAnim);//シーンを切り替える　‐activeTransformAnim : 切り替えのアニメーションを使うかどうか
	void SwitchScene_2(E_Scene sceneType, bool reload);//実際にシーンを切り替える処理
	void SwitchNextScene();//次のシーンに切り替える

	void ResetCurScene();//今のシーンを初期の状態に戻す

	void ClearSceneDic();

	E_Scene GetCurSceneType();

	vector3s GetSceneCameraPos();

	Scene* GetCurScene();
};

/// <summary>
/// ゲーム入力を管理するクラス
/// </summary>
class InputMgr {
private:
	bool inputState;//入力スイッチ（入力できるかどうか
	E_InputMode curInputMode;//今の入力モード（ゲームモードやメニューモードなど

public:

	InputMgr();
	~InputMgr();

	void DoUpdate(float deltatime);
	void SwitchInputMode(E_InputMode targetInputMode);//入力モードを切り替える

	void SetInputState(bool state);//入力スイッチ
	E_InputMode GetCurInputMode();//今の入力モードを取得する
};


class UIMgr {
private:

	//map<E_UIContent, vector<UI*>> uiPools;

private:

	BaseCell* titleMenu;//タイトル
	BaseCell* m_title;
	BaseCell* endMenu;//ゲームクリア
	BaseCell* resultEff;
	BaseCell* gameOver;//ゲームオーバー
	BaseCell* scoreText;
	//===============================
	//追加松尾
	BaseCell* logoMenu; //ロゴ画面
	DrawData* teamLogo_drawData;	//ロゴ自体

	enum FadeStatus
	{
		FadeIn,
		FadeOut
	};

	int fadeStatus = FadeIn;
	//=====
		//bar　　　　　　byヨウ
	//============================
	BaseCell* bar;	//バー
	BaseCell* pIcon;//プレイヤーアイコン
	BaseCell* eIcon;//敵アイコン
	//============================


	//早川追加=========================
	BaseCell* uiLife[UI_LIFE_NUM];   //HPのUI(仮)
	int       playerLife = 0;  //プレイヤーのライフ保存用
	int       playerLifeMax;   //プレイヤーのライフ最大値保存用
	int       uiLife_gap = 130;  //ライフの表示をずらす
	bool      Life_Drawflag[UI_LIFE_NUM]; //ライフ表示フラグ
	vector2s* campos;

	BaseCell* uiBullet[UI_BULLET_NUM];
	bool      Bullet_Drawflag[UI_BULLET_NUM];
	int       uiBullet_gap = 0;
	int       HpDecrease;
	int       PlyerLife_before;
	float     bulletPos;  // 01/23 早川追加
	//=================================

	//23.1 - 仮
	BaseCell* m_numArray[UI_NUM_COUNT];
	bool m_HUDState = false;
	BaseCell* m_icon;
	//温追加 score=========================
	BaseCell* m_score[UI_SCORE_COUNT];
	int m_enemyCounter = 0; //倒したエネミーのカウンター
	int m_number = 0; // 具体的スコア数値
	int playerMaxPos = 0;
	int       score_gap = 80;  //表示をずらす
	float bar_y_offset = 300.0f;
public:
	UIMgr();
	~UIMgr();

	void DoInit();

	void ShowTitle(bool show);//タイトル画面の表示
	void ShowEndMenu(bool show);//ゲームクリア画面の表示
	void ShowGameOver(bool show);//ゲームオーバー画面の表示

		//=========================================
	//追加松尾
	BaseCell* teamLogo;
	float alphateamLogo = 0.0;
	void ShowLogoMenu(bool show);	//ロゴ画面の表示
	void DrawTeamLogo(bool show);	//ロゴを表示
	void DoUpdate(float deltatime);
	//セッター
	void SetFadeStatus(int status) { fadeStatus = status; }
	//=========================================
	//早川追加=========================
	void DoUpdate();
	void DrawLife(); //仮の表示
	void DrawUIBullet();
	//=================================
		//=====================
	//bar処理      byヨウ
	void UpdateBar();
	void DrawBar(bool show);
	//========================
	//23.1 
	void ShowHUD(bool _show);

	// 温追加 score============
	void UpdateScore();
	void DrawScore(bool show);
	void ShowScore();

	void SetScoreNum(int pos) { playerMaxPos = pos; }

	void EnemyCounterAdd() { m_enemyCounter++; }
	int GetEnemyCounter() { return m_enemyCounter; }
	void SetEnemyCounter(int _count) {
		m_enemyCounter = _count;
	}

	//23.1.25陳
	void ResetLifeUI();

};

/// <summary>
/// ゲームプロセス管理するクラス（シングルトン）
/// 
/// ‐グローバルシングルトンであれば　プログラムのどの場所でも呼び出される。　使い例： GameMgr::GetInstance()->「メンバー変数名　OR　メンバー関数名」
/// </summary>
class GameMgr {

private:
	static GameMgr* instance;

private:
	/// <summary>
	/// ゲームステート変数
	/// </summary>
	map<E_GameState, GameState*> gameStateDic;//ゲームステートリスト（タイトル、ゲームスタート、ゲームオーバーなど
	map<E_GameState, GameState*> subGameStateDic;//サブゲームステートリスト（ゲームメニューなど

	E_GameState defaultGameStateType;//デフォルトゲームステート（一般的にはタイトルを設定する
	E_GameState defaultSubGameStateType;//デフォルト　サブゲームステート

	E_GameState curGameStateType;//今のステート
	E_GameState curSubGameStateType;//今のサブステート
	GameState* curGameState;//今のステートのインスタンス
	GameState* curSubGameState;//今のサブステートのインスタンス
public:

	/// <summary>
	/// マネージャのインスタンス
	/// </summary>
	SceneMgr* sceneMgr;//ゲームシーンを管理するマネージャのインスタンス
	InputMgr* inputMgr;//ゲーム入力を管理するマネージャのインスタンス
	SceneObjMgr* sceneObjMgr;//ゲームオブジェクトを管理するマネージャのインスタンス
	EffectMgr* effectMgr;//エフェクトを管理するマネージャのインスタンス
	UIMgr* uiMgr;//UIを管理するマネージャのインスタンス
	Timer* timer;//ゲーム時間を管理するクラスのインスタンス
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

	void RegisterGameState(E_GameState gameStateType, GameState* gameState, bool setDefault);//ゲームステートインスタンスをリスト内に追加する　- setDefault : デフォルトステート、プログラムが実行されるとき自動的にこのステートに切り替える
	void RegisterSubGameState(E_GameState gameStateType, GameState* gameState, bool setDefault);

	void SwitchGameState(E_GameState gameState, bool reset);//ゲームステートインスタンスをリスト内に追加する　- setDefault : デフォルトステート、プログラムが実行されるとき自動的にこのステートに切り替える
	void SwitchSubGameState(E_GameState subGameState, bool reset);

	E_GameState GetCurGameState();//今のステート種類を取得する
	E_GameState GetCurSubGameState();//今のサブステート種類を取得する

public:
	//シングルトンが自動的に削除されるために使うクラス
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
	static GameMgr* GetInstance() //GameMgrインスタンスを取得する
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
/// ゲームステート親クラス（抽象クラス）
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
//追加松尾

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
	float gameTimer_1;//game、player
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
