#pragma once
#include "main.h"
using namespace std;

#define UI_WORD_CHILD_COUNT 5
#define DEFAULT_FONT_SIZE 16
#pragma region base_cell

enum E_Component;
enum E_MovementType;
enum E_DrawLayer;
enum E_ObjCellType;
enum E_Anim;

class Component;
class Transform;
class Movement;

class DrawData;
class Player;
enum E_CellType {
	CellType_None = -1,
	CellType_Enemy = 0,//オブジェクトの種類に敵種類を追加する
	CellType_Player = 1,
	CellType_Obstacle = 2,
	CellType_UI = 3,
	CellType_Bullet = 4,

	CellType_Ice = 5,
	CellType_Wind = 6,
	CellType_BeltConveyor = 7,
	CellType_SpringGround = 8,
	CellType_MoveGround = 9,
	CellType_AppearGround = 10,

	//other
	CellType_TargetPoint = 12,
	CellType_BG = 13,
	CellType_Menu = 14,

	//effect
	CellType_Effect_Hit = 15,
	CellType_Effect_Jump = 16,
	CellType_Effect_Rush = 17,
	CellType_Effect_Sweat = 18,
	CellType_Effect_Recover = 19,
	CellType_Effect_Sakura = 101,
	CellType_Effect_Result = 102,
	CellType_Effect_Button = 103,

	CellType_Enemy_Dumpling = 21,
	CellType_Enemy_Pasta = 22,
	CellType_Enemy_Ittanmomen = 23,
	CellType_Enemy_Cornet = 24,

	CellType_Onigiri = 31,
	CellType_Sugar = 32,
	CellType_Pepper = 33,
	CellType_Pickled_Plums = 34,
	CellType_Miso_Soup = 35,

	CellType_GameOver = 100,

	CellType_Npc = 997,
	CellType_Bullet_Fireball = 998,
	CellType_Boss = 999,
};

enum class E_Obstacle : int{
	Obstacle_Const = 1,
	Obstacle_ChargeMovement = 2,
	Obstacle_Breakable = 3,
	Obstacle_Mirror = 4,

	Obstacle_Ground =5,
	Obstacle_Ice =6,
	Obstacle_Stone =7,
};

enum E_MovementType {
	MovementType_None,
	MovementType_Axis,
	MovementType_Appear,
};


class BaseCell {
protected:

	map<E_Component, Component*> componentDic;//コンポーネントリスト

	map<E_MovementType, Movement*> movementDic;//移動などのアニメションリスト

protected:

	Transform* transform;

	E_DrawLayer drawLayer;//描画処理の順序を決める変数
	E_CellType cellType;//ゲームオブジェクト種類
	int cellId;//ゲームオブジェクトID

	bool enable;//ゲームオブジェクトの状態フラグ：今使っているかどうか
	DrawData* drawData;//描画処理のパラメータ（テクスチャ―座標やサイズなど

	BaseCell* m_parent;
	vector<BaseCell*> m_childArray;

private:
	D3DXCOLOR defaultColor;
	float affectColorChangeSign;

public:
	E_ObjCellType objCellType;
public:

	BaseCell();
	virtual ~BaseCell();

	/// <summary>
	/// life cycle
	/// </summary>
	/// <param name="deltatime"></param>
	virtual void DoInit();//初期化
	virtual void UnInit();//終了処理
	virtual void DoUpdate(float deltatime);//アップデート
	virtual void DoLateUpdate(float deltatime);//普通のアップデート完了した後　実行される二番目のアップデート関数

	/// <summary>
	/// ゲッターセッター
	/// </summary>
	/// <returns></returns>
	int GetID() const {
		return cellId;
	}
	void SetID(int id) {
		cellId = id;
	}
	virtual void SetState(bool state);
	bool CheckState() const {
		return enable;
	}
	void SetCellType(E_CellType cType) {
		cellType = cType;
	}
	E_CellType GetCellType() const {
		return cellType;
	}

	void SetDrawLayer(E_DrawLayer layer);
	E_DrawLayer GetDrawLayer();
	void SetDrawData(DrawData* data) {
		//save pointer
		drawData = data;
	}
	DrawData* GetDrawData() {
		return drawData;
	}

	/// <summary>
	/// コンポーネント関数
	/// </summary>
	/// <param name="componentType"></param>
	/// <returns></returns>
	bool RegisterComponent(E_Component componentType, Component* componentEntity, bool replace = false);//新しいコンポーネントをリストの中に追加する

	void RemoveComponent(E_Component componentType) //コンポーネントを削除する
	{
		map<E_Component, Component*>::iterator iter = componentDic.find(componentType);
		if (iter == componentDic.end()) {
			return;
		}
		
		componentDic.erase(componentType);
	}

	Component* GetComponent(E_Component componentType) //種類によってコンポーネントのインスタンスを取得する
	{
		map<E_Component, Component*>::iterator iter = componentDic.find(componentType);
		if (iter == componentDic.end()) {
			return NULL;
		}
		else {
			return iter->second;
		}
	}
	Transform* GetTransform();

	/// <summary>
	/// トランスフォーマーコンポーネントに関する関数
	/// </summary>
	/// <param name="pos"></param>
	virtual void SetPos(vector3s pos, bool updatePrePos);//位置を設定する
	void SetRot(vector3s rot);//回転角度を設定する
	void SetScale(vector3s scale);//スケールを設定する
	void SetTransform(const Transform& t);//位置、回転角度、スケールを共に設定する
	void ResetPos(bool x, bool y);//最初の位置に戻す
	vector2s GetSize(bool withScale) const ;//ゲームオブジェクトのサイズを取得する　‐　withScale : スケールに応じて拡大したり縮小したりしたサイズかどうか
	vector2s GetColliderSize(bool withScale,int colliderDataIndex);//コライダーボックスのサイズを取得する　‐　withScale :スケールに応じて拡大したり縮小したりしたサイズかどうか
	vector3s GetColliderPos(bool _withScale, int colliderDataIndex);

	/// <summary>
	/// 指定された軌道に沿って移動したり不透明度を変えたりするとかに使う特殊なアニメーションクラスの関数
	/// </summary>
	/// <param name="movement"></param>
	void AddMovement(Movement* movement);
	void RemoveMovement(E_MovementType movementType);
	void StartMovement(E_MovementType targetMovement,bool reset,bool allMovement);
	void StopMovement(E_MovementType targetMovement,bool stopForward,bool allMovement);
	void SetMovementState(E_MovementType targetMovement,bool state,bool allMovement);
	map<E_MovementType, Movement*>& GetMovementDic();
	bool CheckCurMovementDoneState(E_MovementType movementType);
	void UpdateMovementData();//2022.11.25

	virtual void RecordCurState();//ゲームオブジェクトの状態を保存する
	virtual void Reset();//ゲームオブジェクトの状態を最初に戻す

	virtual void Recycle();
	virtual void OnCellInit();
	virtual void OnCellRecycle();
	virtual void DeleteCell();

	virtual void SetParent(BaseCell* _parent);
	virtual void AddChild(BaseCell* _child);
	virtual void RemoveChild(BaseCell* _child);

	static bool CheckInScreenRect(BaseCell* cell, bool checkWholeCell);

	//2023.1
	virtual void SwitchAnim(E_Anim _anim, bool _resetAnim, bool _lastClipEndCB);
	virtual void SetDrawDataSize(vector2s _size);

};

#pragma endregion base_cell

#pragma region scene_obj
//
//class Player :public BaseCell {
//private:
//	vector3s preMouseMovement = vector3s();
//public:
//	bool inputLock = false;
//public:
//	Player();
//	~Player();
//
//	void DoUpdate(float deltatime);
//
//	void Input(float deltatime);
//
//	void SetState(bool state);
//
//};

class Obstacle :public BaseCell {
private:
	E_Obstacle obstacleType;

public:
	Obstacle();
	virtual ~Obstacle();

	void DoUpdate(float deltatime);
	void SetObstacleType(E_Obstacle obstacleT);
	E_Obstacle GetObstacleType();

	virtual void Recycle();
	virtual void OnCellInit();
	virtual void OnCellRecycle();
	virtual void DeleteCell();

	virtual void Reset();
	virtual void SetParent(BaseCell* _parent);
};
//
//class Bullet :public BaseCell {
//	Bullet() {
//		cellType = E_CellType::CellType_Bullet;
//	}
//	~Bullet() {
//
//	}
//
//	void DoUpdate(float deltatime) {
//		BaseCell::DoUpdate(deltatime);
//	}
//
//	void PlayerTest(Player* player);
//};

#pragma endregion scene_obj
