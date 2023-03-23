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
	E_Component componentType;//�R���|�[�l���g�̎��

	BaseCell* parent;//�R���|�[�l���g�̏��L�ҁi�Q�[���I�u�W�F�N�g�j
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
	virtual void RecordCurState() = 0;//������Ԃ�ۑ�����
	virtual void Reset() = 0;//�����̏�Ԃɖ߂�
};

class Transform :public Component {
private:

	vector3s defaultPos;//�����̈ʒu
	vector3s defaultRot;//�����̉�]�p�x
	vector3s defaultScale;//�����̃X�P�[��

	vector3s m_pos;//���̈ʒu
	vector3s m_rot;//���̉�]�p�x
	vector3s m_scale;//���̃X�P�[��

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
	Collider* parent;//���L�҃|�C���^�[
	bool isTrigger;

	E_ColliderType colliderType;//�R���C�_�[�̎�ށ@�F�~�@OR�@�l�p�`
	vector2s size;//�R���C�_�[�{�b�N�X�̃T�C�Y
	vector3s offset;//�R���C�_�[�{�b�N�X�̈ʒu�̓I�u�W�F�N�g�̌��_���ǂꂭ�炢����Ă邩

	ColliderData();
	ColliderData(const ColliderData& src);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="i">�@ID</param>
	/// <param name="p">�@���L�҃|�C���^�[�i�R���C�_�[�R���|�[�l���g�j</param>
	/// <param name="trigger"></param>
	/// <param name="cType">�@�R���C�_�[�̎�ށi�~OR�l�p�`�j</param>
	/// <param name="s">�@�R���C�_�[�{�b�N�X�̃T�C�Y</param>
	/// <param name="o">	�R���C�_�[�{�b�N�X�̈ʒu�̓I�u�W�F�N�g�̌��_���ǂꂭ�炢����Ă邩</param>
	ColliderData(int i, Collider* p, bool trigger, E_ColliderType cType, vector2s s, vector3s o);
	~ColliderData();

	void Copy(const ColliderData& src);
};

class Collider :public Component {
private:
	vector<ColliderData*> colliderDataArray;//�R���C�_�[�{�b�N�X�̃f�[�^�̃��X�g�i��̃Q�[���I�u�W�F�N�g�ɕ����̃R���C�_�[��ݒ肷�邱�Ƃ͉\
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
	static bool CollisionDetect(BaseCell* cell_1, BaseCell* cell_2);//�����蔻��F�~�Ɖ~�A�~�Ǝl�p�`�A�l�p�`�Ǝl�p�`
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
	BaseCell* parent;//�A�j���[�V�����̏��L��

	E_Anim animType;//�A�j���[�V�����̎��

	//�e�N�X�`���\�p�����[�^
	vector2s frameIndexRange;
	int curIndex;

	float animTime;//�A�j���[�V�����̎�������
	float animTime_I;//	1.0f/animTime
	float animTimePass;// �o�ߎ���
	bool animState;//	�A�j���[�V�����̏��
	bool dir;	//�A�j���[�V�����̕���

	bool loop;	//�J��Ԃ��t���O

public:
	//�R�[���o�b�N�֐�
	Anim_CallBack* animStart_Callback;//�X�^�[�g�R�[���o�b�N
	Anim_CallBack* animEnd_Callback;//�G���h�R�[���o�b�N

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
	/// �A�j���[�V�����̃p�����[�^��ݒ肷��
	/// </summary>
	/// <param name="animT">�@�A�j���[�V�����̎��</param>
	/// <param name="indexRange">�@�e�N�X�`���\�p�����[�^</param>
	/// <param name="time">�@��������</param>
	/// <param name="animDir">�@�A�j���[�V�����̕���</param>
	/// <param name="isLoop">�@�J��Ԃ��t���O</param>
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
	/// �A�j���[�V�������Đ�����r
	/// </summary>
	void StartAnim() {
		if (dir) {//�������A�ŏ��̃A�j���[�V�����t���C���ɐݒ肷��
			curIndex = frameIndexRange.x + 0.5f;
		}
		else {
			curIndex = frameIndexRange.y + 0.5f;
		}
		if (animTimePass >= animTime) {
			animTimePass = 0.0f;
		}
		animState = true;
		OnAnimStart();//�X�^�[�g�R�[���o�b�N
	}

	/// <summary>
	/// �A�j���[�V�����̏�Ԃ�ݒ肷��i�ꎞ��~��Đ��Ȃ�
	/// </summary>
	/// <param name="state"></param>
	void SetAnimState(bool state) {
		animState = state;
	}
	/// <summary>
	/// �A�j���[�V�����̏I������
	/// </summary>
	void StopAnim() {
		animState = false;
		if (dir) {//�Ō�̃A�j���[�V�����t���C���ɐݒ肷��
			curIndex = frameIndexRange.x + 0.5f;
		}
		else {
			curIndex = frameIndexRange.y + 0.5f;
		}
		OnAnimEnd();//�G���h�R�[���o�b�N
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

	Animation* parent;//�i�A�j���[�V�����R���|�[�l���g�j���L�҃|�C���^�[

	map<E_Anim, AnimClip*> animDic;//�A�j���[�V�������X�g
	E_Anim defaultAnim;//�f�t�H���g�A�j���[�V����
	E_Anim curAnim;//���̃A�j���[�V����
	AnimClip* curAnimClip;//���̃A�j���[�V�����̃C���X�^���X

	bool animState;
public:
	AnimFSM() :parent(NULL), curAnim(E_Anim::Anim_None), defaultAnim(E_Anim::Anim_None), animState(true),curAnimClip(NULL) {
		animDic.clear();
	}

	~AnimFSM() {
		//�A�j���[�V�������폜����
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
		SwitchAnim(defaultAnim, true, false);//�f�t�H���g�A�j���[�V�����ɐ؂�ւ���
		SetAnimState(true);
	}

	/// <summary>
	/// ���L�҂�ݒ肷��
	/// </summary>
	/// <param name="p"></param>
	void SetParent(Animation* p) {
		parent = p;
	}

	/// <summary>
	/// �A�j���[�V�����̏�Ԃ�ݒ肷��i�ꎞ��~��ĊJ�Ȃ�
	/// </summary>
	/// <param name="state"></param>
	void SetAnimState(bool state) {
		animState = state;
	}

	/// <summary>
	/// �A�j���[�V�����̐؂�ւ��̏���
	/// </summary>
	/// <param name="animType">�@���̃A�j���[�V����</param>
	/// <param name="resetAnim">�@���̃A�j���[�V�����Ɠ����̏ꍇ�A���Z�b�g���邩�ǂ���</param>
	/// <param name="lastClipEndCB">�@���̃A�j���[�V�����̏I�������֐����Ăяo�����ǂ���</param>
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
			curAnimClip->StopAnim();//���̃A�j���[�V�����̏I������
		}
		curAnimClip = targetAnimClip;
		if (curAnimClip != NULL) {//���̃A�j���[�V�����ɐ؂�ւ�
			curAnimClip->StartAnim();

			curAnim = animType;
		}
	}

	/// <summary>
	/// �V�����A�j���[�V������ǉ�����
	/// </summary>
	/// <param name="animType">�@�A�j���[�V�����̎��</param>
	/// <param name="anim">�@�A�j���[�V�����̃C���X�^���X</param>
	/// <param name="setDefaultAnim">�@�f�t�H���g�A�j���[�V�����ɐݒ肷�邩�ǂ���</param>
	void RegisterAnim(E_Anim animType, AnimClip* anim, bool setDefaultAnim = false);

	/// <summary>
	/// �A�b�v�f�[�g����
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
	AnimFSM animFSM;//�A�j���V�����L���I�[�g�}�g��

public:
	Animation() {
		animFSM.SetParent(this);
		componentType = E_Component::Component_Animation;
	}
	~Animation() {

	}

	/// <summary>
	/// �V�����A�j���[�V������ǉ�����
	/// </summary>
	/// <param name="animType">�@�A�j���[�V�����̎��</param>
	/// <param name="anim">�@�A�j���[�V�����̃C���X�^���X</param>
	/// <param name="setDefaultAnim">�@�f�t�H���g�A�j���[�V�����ɐݒ肷�邩�ǂ���</param>
	void RegisterAnim(E_Anim anim, AnimClip* animClip, bool setDefault) {
		animFSM.RegisterAnim(anim, animClip, setDefault);
	}

	void DoInit();
	void UnInit();

	/// <summary>
	/// ���L�҂�ݒ肷��
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
	void SwitchAnim(E_Anim targetAnim, bool resetAnim, bool lastClipEndCB);//�A�j���V������؂�ւ�
	void SwitchToDefaultAnim();//�f�t�H���g�A�j���V�����ɐ؂�ւ�
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
	FSMSystem* fsmSystem;//AI�L���I�[�g�}�g��
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