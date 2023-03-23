#pragma once
#include "main.h"
using namespace std;

class Collider;
class Transform;
class Component;
class Animation;
class ColliderData;

class Movement;
class Movement_Axis;
class Movement_Appear;

class BaseCell;

class AnimClip;
class FSMSystem;

enum E_MovementType;

enum E_Component {
	Component_None,
	Component_Transform,
	Component_Collider,
	Component_Animation,
	Component_AI,
};


enum E_Anim {
	Anim_None,
	Anim_Idle,
	Anim_Move,
	Anim_Run,
	Anim_Slide,
	Anim_Tumble,
	Anim_Hurt,
	Anim_Wind,
	Anim_Fly,

	Anim_Win,

	Anim_SakuraEff,
	Anim_LightTextEff,
	Anim_ResultEff,
	Anim_ButtonEff,
};

enum E_PatrolType {
	PatrolType_Const,
	PatrolType_EndInverse,
	PatrolType_LoopCircle,
};

class Component {
protected:
	E_Component componentType;//コンポーネントの種類

	BaseCell* parent;//コンポーネントの所有者（ゲームオブジェクト）
	bool enable;
public:
	Component();
	virtual ~Component();
	Component(BaseCell* p, E_Component cType);

	virtual void DoInit() {
	}

	virtual void UnInit() {

	}

	virtual void DoUpdate(float deltatime) {

	}

	virtual void DoLastUpdate(float deltatime) {

	}

	BaseCell* GetParent();

	E_Component GetComponentType() const  {
		return componentType;
	}

	virtual void SetParent(BaseCell* p);

	virtual void SetComponentState(bool state) {
		enable = state;
	}
	virtual bool CheckComponentState() const {
		return enable;
	}


	virtual void Copy(const Component& src) = 0;
	virtual void RecordCurState() = 0;//初期状態を保存する
	virtual void Reset() = 0;//初期の状態に戻す
};

class Transform :public Component {
private:

	vector3s defaultPos;//初期の位置
	vector3s defaultRot;//初期の回転角度
	vector3s defaultScale;//初期のスケール

	vector3s m_pos;//今の位置
	vector3s m_rot;//今の回転角度
	vector3s m_scale;//今のスケール

	bool tempPosSign = false;
	vector3s tempPos;
	vector3s prePos;

public:
	Transform();

	Transform(vector3s pos, vector3s rot, vector3s scale);

	void DoInit();

	void UnInit();

	vector3s GetPos() const {
		return m_pos;
	}

	vector3s GetPrePos()const {
		return prePos;
	}

	vector3s GetMovement() {
		return m_pos - prePos;
	}

	vector3s GetRot()const {
		return m_rot;
	}

	vector3s GetScale()const {
		return m_scale;
	}

	void SetPos(vector3s p, bool updatePrePos = true);

	void SetRot(vector3s r) {
		m_rot = r;
	}

	void SetScale(vector3s s) {
		m_scale = s;
	}

	void ResetPos(bool x, bool y) {
		if (x) {
			m_pos.x = prePos.x;
		}
		if (y) {
			m_pos.y = prePos.y;
		}
	}

	void Copy(const Component& src);

	void RecordCurState();
	void Reset();

	static bool AdjustPosition(BaseCell* cell, vector3s center, vector2s rect, bool inner);
};


enum E_ColliderType {
	Collider_None = -1,
	Collider_Rect = 0,
	Collider_Circle = 1,
};

struct ColliderData {
	int id;
	Collider* parent;//所有者ポインター
	bool isTrigger;

	E_ColliderType colliderType;//コライダーの種類　：円　OR　四角形
	vector2s size;//コライダーボックスのサイズ
	vector3s offset;//コライダーボックスの位置はオブジェクトの原点よりどれくらいずれてるか

	ColliderData();
	ColliderData(const ColliderData& src);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="i">　ID</param>
	/// <param name="p">　所有者ポインター（コライダーコンポーネント）</param>
	/// <param name="trigger"></param>
	/// <param name="cType">　コライダーの種類（円OR四角形）</param>
	/// <param name="s">　コライダーボックスのサイズ</param>
	/// <param name="o">	コライダーボックスの位置はオブジェクトの原点よりどれくらいずれてるか</param>
	ColliderData(int i, Collider* p, bool trigger, E_ColliderType cType, vector2s s, vector3s o);
	~ColliderData();

	void Copy(const ColliderData& src);
};

class Collider :public Component {
private:
	vector<ColliderData*> colliderDataArray;//コライダーボックスのデータのリスト（一つのゲームオブジェクトに複数のコライダーを設定することは可能
public:
	Collider();
	Collider(const Collider& collider);

	~Collider();

	void DoInit();
	void UnInit();

	ColliderData* GetColliderData(int id);
	void AddColliderData(ColliderData* data);

	vector<ColliderData*>& GetColliderDataArray() 
	{
		return colliderDataArray;
	}

	int GetColliderDataArraySize() 
	{
		return colliderDataArray.size();
	}

	void ClearColliderDataArray();
	void SetColliderSize(vector2s size, int colliderDataIndex);
	void SetColliderOffset(vector3s _offset, int _colliderDataIndex);

	void Copy(const Component& dest);
	 void RecordCurState() {
	}
	 void Reset() {
	}

	//static method
	//collision detect
	static bool CollisionDetect(BaseCell* cell_1, BaseCell* cell_2);//当たり判定：円と円、円と四角形、四角形と四角形
	static bool CollisionDetect(BaseCell* cell_1, Transform* transform_2, Collider* c_2);
	static bool RectCircleCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2);
	static bool RectCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2);
	static bool CircleCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2);

	static bool IsContainPoint(BaseCell* cell, vector3s point);
	static bool IsRectContainPoint(Transform* transform, ColliderData& data, vector3s point);
	static bool IsRectContainPoint(vector3s pos, vector2s size, vector3s rot, vector3s point);
	static bool IsCircleContainPoint(Transform* transform, ColliderData& data, vector3s point);
	static bool IsCircleContainPoint(vector3s pos, float radius, vector3s point);

	static bool SceneObjCollisionDetect(BaseCell* cell_1, Transform* transform_2, Collider* c_2);

	static bool LineCollisionDetect(BaseCell* cell_1, BaseCell* cell_2,vector3s& collisionPoint,vector3s& collisionNormal);
	static bool LineCollisionDetect(vector3s startPos, vector3s endPos, BaseCell* cell,vector3s& collisionPoint,vector3s& collisionNormal);
	static bool LineCollisionDetect(vector3s startPos_1, vector3s endPos_1, vector3s startPos_2, vector3s endPos_2, vector3s& collisionPos);
};

#pragma region anim

#pragma region anim_callback

struct Anim_CallBack {

	virtual void Func(AnimClip* baseClip) = 0;
};

struct Anim_Effect_Normal_CB : public Anim_CallBack {
	void Func(AnimClip* baseClip);
};

struct Anim_Player_Win_CB : public Anim_CallBack {
	void Func(AnimClip* _baseClip);
};

#pragma endregion anim_callback


class AnimClip {
private:
	BaseCell* parent;//アニメーションの所有者

	E_Anim animType;//アニメーションの種類

	//テクスチャ―パラメータ
	vector2s frameIndexRange;
	int curIndex;

	float animTime;//アニメーションの持続時間
	float animTime_I;//	1.0f/animTime
	float animTimePass;// 経過時間
	bool animState;//	アニメーションの状態
	bool dir;	//アニメーションの方向

	bool loop;	//繰り返しフラグ

public:
	//コールバック関数
	Anim_CallBack* animStart_Callback;//スタートコールバック
	Anim_CallBack* animEnd_Callback;//エンドコールバック

public:
	AnimClip() :animType(E_Anim::Anim_None), frameIndexRange(vector2s(0, 0)), curIndex(0),animState(false),
		animTime(0.0f),animTime_I(0.0f), animTimePass(0.0f), loop(true), dir(true),
	animStart_Callback(NULL),animEnd_Callback(NULL),parent(NULL){

	}

	AnimClip(const AnimClip& src) {
		this->animType = src.animType;
		this->frameIndexRange = src.frameIndexRange;
		this->curIndex = src.curIndex;
		this->animTime = src.animTime;
		this->animTime_I = src.animTime_I;
		this->animTimePass = src.animTimePass;
		this->animState = src.animState;
		this->dir = src.dir;
		this->loop = src.loop;

		//this->parent = src.parent;
		this->parent = NULL;
		this->animStart_Callback = src.animStart_Callback;
		this->animEnd_Callback = src.animEnd_Callback;
	}

	~AnimClip();


	void DoUpdate(float deltatime);

	void DoLateUpdate(float deltatime) {
		if (animState) {

		}
	}


	void SetParent(BaseCell* p) {
		parent = p;
	}
	BaseCell* GetParent() {
		return parent;
	}

	/// <summary>
	/// アニメーションのパラメータを設定する
	/// </summary>
	/// <param name="animT">　アニメーションの種類</param>
	/// <param name="indexRange">　テクスチャ―パラメータ</param>
	/// <param name="time">　持続時間</param>
	/// <param name="animDir">　アニメーションの方向</param>
	/// <param name="isLoop">　繰り返しフラグ</param>
	void SetAnimParam(E_Anim animT, vector2s indexRange, float time, bool animDir, bool isLoop) {
		animType = animT;
		frameIndexRange = indexRange;
		animTime = time;
		dir = animDir;
		loop = isLoop;

		animTime_I = animTime <= 0 ? 1 : 1.0f / animTime;

		animTimePass = 0.0f;
		animState = false;
	}

	E_Anim GetAnimType() {
		return animType;
	}

	/// <summary>
	/// アニメーションを再生するr
	/// </summary>
	void StartAnim() {
		if (dir) {//初期化、最初のアニメーションフレイムに設定する
			curIndex = frameIndexRange.x + 0.5f;
		}
		else {
			curIndex = frameIndexRange.y + 0.5f;
		}
		if (animTimePass >= animTime) {
			animTimePass = 0.0f;
		}
		animState = true;
		OnAnimStart();//スタートコールバック
	}

	/// <summary>
	/// アニメーションの状態を設定する（一時停止や再生など
	/// </summary>
	/// <param name="state"></param>
	void SetAnimState(bool state) {
		animState = state;
	}
	/// <summary>
	/// アニメーションの終了処理
	/// </summary>
	void StopAnim() {
		animState = false;
		if (dir) {//最後のアニメーションフレイムに設定する
			curIndex = frameIndexRange.x + 0.5f;
		}
		else {
			curIndex = frameIndexRange.y + 0.5f;
		}
		OnAnimEnd();//エンドコールバック
	}


	//callback
	void OnAnimStart() {
		if (animStart_Callback != NULL) {
			animStart_Callback->Func(this);
		}
	}

	void OnAnimEnd() {
		if (animEnd_Callback != NULL) {
			animEnd_Callback->Func(this);
		}
	}
};

class AnimFSM {
private:

	Animation* parent;//（アニメーションコンポーネント）所有者ポインター

	map<E_Anim, AnimClip*> animDic;//アニメーションリスト
	E_Anim defaultAnim;//デフォルトアニメーション
	E_Anim curAnim;//今のアニメーション
	AnimClip* curAnimClip;//今のアニメーションのインスタンス

	bool animState;
public:
	AnimFSM() :parent(NULL), curAnim(E_Anim::Anim_None), defaultAnim(E_Anim::Anim_None), animState(true),curAnimClip(NULL) {
		animDic.clear();
	}

	~AnimFSM() {
		//アニメーションを削除する
		for (auto& cur : animDic) {
			if (cur.second != NULL) {
				delete cur.second;
			}
		}

		animDic.clear();

		parent = NULL;
		curAnimClip = NULL;
	}

	void DoInit() {
		SwitchAnim(defaultAnim, true, false);//デフォルトアニメーションに切り替える
		SetAnimState(true);
	}

	/// <summary>
	/// 所有者を設定する
	/// </summary>
	/// <param name="p"></param>
	void SetParent(Animation* p) {
		parent = p;
	}

	/// <summary>
	/// アニメーションの状態を設定する（一時停止や再開など
	/// </summary>
	/// <param name="state"></param>
	void SetAnimState(bool state) {
		animState = state;
	}

	/// <summary>
	/// アニメーションの切り替えの処理
	/// </summary>
	/// <param name="animType">　次のアニメーション</param>
	/// <param name="resetAnim">　今のアニメーションと同じの場合、リセットするかどうか</param>
	/// <param name="lastClipEndCB">　今のアニメーションの終了処理関数を呼び出すかどうか</param>
	void SwitchAnim(E_Anim animType, bool resetAnim, bool lastClipEndCB) {

		if (animType == curAnim && resetAnim != true) {
			return;
		}

		map<E_Anim, AnimClip*>::iterator iter = animDic.find(animType);//
		AnimClip* targetAnimClip = NULL;
		if (iter != animDic.end()) {
			//switch
			targetAnimClip = animDic[animType];
		}

		if (curAnimClip != NULL && lastClipEndCB == true) {
			curAnimClip->StopAnim();//今のアニメーションの終了処理
		}
		curAnimClip = targetAnimClip;
		if (curAnimClip != NULL) {//次のアニメーションに切り替え
			curAnimClip->StartAnim();

			curAnim = animType;
		}
	}

	/// <summary>
	/// 新しいアニメーションを追加する
	/// </summary>
	/// <param name="animType">　アニメーションの種類</param>
	/// <param name="anim">　アニメーションのインスタンス</param>
	/// <param name="setDefaultAnim">　デフォルトアニメーションに設定するかどうか</param>
	void RegisterAnim(E_Anim animType, AnimClip* anim, bool setDefaultAnim = false);

	/// <summary>
	/// アップデート処理
	/// </summary>
	/// <param name="deltatime"></param>
	void DoUpdate(float deltatime) {
		if (animState == false)return;
		if (curAnimClip != NULL) {
			curAnimClip->DoUpdate(deltatime);
		}
	}

	void DoLateUpdate(float deltatime) {
		if (animState == false)return;
		if (curAnimClip != NULL) {
			curAnimClip->DoLateUpdate(deltatime);
		}
	}

};


class Animation : public Component {
private:
	AnimFSM animFSM;//アニメション有限オートマトン

public:
	Animation() {
		animFSM.SetParent(this);
		componentType = E_Component::Component_Animation;
	}
	~Animation() {

	}

	/// <summary>
	/// 新しいアニメーションを追加する
	/// </summary>
	/// <param name="animType">　アニメーションの種類</param>
	/// <param name="anim">　アニメーションのインスタンス</param>
	/// <param name="setDefaultAnim">　デフォルトアニメーションに設定するかどうか</param>
	void RegisterAnim(E_Anim anim, AnimClip* animClip, bool setDefault) {
		animFSM.RegisterAnim(anim, animClip, setDefault);
	}

	void DoInit();
	void UnInit();

	/// <summary>
	/// 所有者を設定する
	/// </summary>
	/// <param name="p"></param>
	void SetParent(BaseCell* p);

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void Copy(const Component& src);
	void RecordCurState() {
	}
	void Reset() {
	}
	void SwitchAnim(E_Anim targetAnim, bool resetAnim, bool lastClipEndCB);//アニメションを切り替え
	void SwitchToDefaultAnim();//デフォルトアニメションに切り替え
	void StopAnim();
	void PauseAnim(bool pause);
};


#pragma endregion anim

#pragma region ai_component

class SimplePatrolAI {
private:
	E_PatrolType patrolType;

	BaseCell* parent;

	vector<vector3s> patrolPoint;
	vector3s defaultPoint;
	vector3s curStartPoint;
	vector3s curTargetPoint;
	vector3s curPos;

	int curStartPointIndex;

	float nextPointInterval;
	float timePass;

	bool moveDone;
	float curMoveTime;
	float curMoveTimePass;

	bool defaultForward;
	bool forward;

	SimplePatrolAI();

public:
	SimplePatrolAI(BaseCell* p, E_PatrolType patrolT, vector3s defaultP, float nextInterval, bool defaultF);
	~SimplePatrolAI();

	void DoInit();
	void DoUpdate(float deltatime);
	void SwitchNextPoint(bool initState);
	void Reset();

	void AddPatrolPoint(vector3s point);

};

class AIComponent :public Component {
private:
	FSMSystem* fsmSystem;//AI有限オートマトン
	SimplePatrolAI* patrolAI;
public:
	AIComponent();
	~AIComponent();

	void DoInit();
	void UnInit();

	void DoUpdate(float deltatime);

	void SetAI(SimplePatrolAI* aiEntity);
	SimplePatrolAI* GetAI();

	FSMSystem* GetFSMSystem();

	void Copy(const Component& src) {

	}
	void Reset();
	void RecordCurState() {

	}
};


#pragma endregion ai_component


#pragma region movement

class Movement {
protected:

	BaseCell* parent;

	E_MovementType movementType;


	bool isEnterLight;

	bool isLoop;
	bool defaultForward;
	bool forward;

	float movementTime;
	float movementTime_I;
	float moveTimePass;

	bool defaultEnter;

	bool movementState;
	bool movementDone;


public:
	Movement();
	Movement(BaseCell* p, E_MovementType mT, bool movementDefaultDir, bool loop, bool defaultActive, float movementT);
	virtual ~Movement();

	virtual void DoInit();

	virtual void StartMovement(bool reset);
	virtual void SetMovementState(bool state);
	virtual void StopMovement(bool stopForward);
	virtual void Reset();
	virtual void ResetDefault();

	virtual void SetMovementTime(float time);
	//virtual void SetMovementDir(bool isForward);
	virtual void DoUpdate(float deltatime) {

	}

	virtual void DoLateUpdate(float deltatime) {

	}

	virtual void LightEffect(bool enterLight) {

	}

	virtual void ApplyMovement() = 0;

	void SetParent(BaseCell* p);

	bool CheckMovementState() {
		return movementState;
	}
	bool IsMovementDone();
	bool CheckDefaultForward();
	E_MovementType GetMovementType() {
		return movementType;
	}

	//other method
	virtual void SetVecTParam(int index, vector3s param) {

	}

};

class Movement_Axis :public Movement {
private:
	bool isRelatedPos;
	vector3s defaultPos;

	vector3s startPos;
	vector3s endPos;

	vector3s curPos;
public:
	Movement_Axis();
	Movement_Axis(BaseCell* p, bool isRelated, bool movementDefaultDir, bool isLoop, bool defaultActive, float movementT, vector3s defaultP, vector3s startP, vector3s endP);
	~Movement_Axis();

	void StartMovement(bool reset);
	void SetMovementState(bool state);
	void StopMovement(bool stopForward);
	void Reset();
	void ResetDefult();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void ApplyMovement();

	void SetVecTParam(int index, vector3s param);

	//2022.11.24
	void ResetStartEndPos(vector3s start, vector3s end);

	//2022.11.25
	void SetDefaultPos(vector3s _pos);
	void SetRelated(bool _isRelated);

	void SetStartPos(vector3s _pos);
	void SetEndPos(vector3s _pos);
};

class Movement_Appear :public Movement {
private:
	bool stateChange;
	bool curCellState;
	float waitTimePass;

public:
	vector4s forwardColor;
	vector4s backwardColor;
	vector4s curColor;
	float waitTime;
public:

	Movement_Appear();
	Movement_Appear(BaseCell* p, bool defaultState, bool isLoop, bool defaultActive, float movementT, float waitT);
	~Movement_Appear();

	void StartMovement(bool reset);
	void SetMovementState(bool state);
	void StopMovement(bool stopForward);
	void Reset();
	void ResetDefault();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void ApplyMovement();

	void SetMovementColor(vector4s fColor, vector4s bColor);//temp
	void SetVecTParam(int index, vector3s param);
};

class Movement_Breakable :public Movement {
private:
	bool targetState;
	bool curState;
public:

	Movement_Breakable();
	Movement_Breakable(BaseCell* p, bool defaultState, bool isLoop, bool defaultActive, float movementT, float waitT);
	~Movement_Breakable();

	void StartMovement(bool reset);
	void SetMovementState(bool state);
	void StopMovement(bool stopForward);
	void Reset();
	void ResetDefault();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void ApplyMovement();

	void SetMovementColor(vector4s fColor, vector4s bColor);//temp
	void SetVecTParam(int index, vector3s param);

};


#pragma endregion movement