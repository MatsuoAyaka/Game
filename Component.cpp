#pragma once

#include "Component.h"
#include "Scene.h"

#pragma region component_method

Component::Component() :componentType(E_Component::Component_None), enable(true), parent(NULL)
{

}

Component::~Component()
{
	parent = NULL;
}

Component::Component(BaseCell* p, E_Component cType) :enable(true), parent(p)
{

}

/// <summary>
/// 　所有者ポインターを取得する
/// </summary>
/// <returns></returns>
BaseCell* Component::GetParent()
{
	return parent;
}

/// <summary>
/// 　所有者ポインターを設定する
/// </summary>
/// <param name="p"></param>
void Component::SetParent(BaseCell* p)
{
	parent = p;
}


#pragma region Transform_method

Transform::Transform() :m_pos(vector3s(0.0f, 0.0f, 0.0f)), m_rot(vector3s(0.0f, 0.0f, 0.0f)), m_scale(vector3s(1.0f, 1.0f, 1.0f)), prePos(vector3s(0.0f, 0.0f, 0.0f)),
defaultPos(vector3s(0.0f, 0.0f, 0.0f)), defaultRot(vector3s(0.0f, 0.0f, 0.0f)), defaultScale(vector3s(1.0f, 1.0f, 1.0f))
{
	tempPosSign = false;
	componentType = E_Component::Component_Transform;//コンポーネントの種類を設定する
}

Transform::Transform(vector3s pos, vector3s rot, vector3s scale) : m_pos(pos), m_rot(rot), m_scale(scale), prePos(pos)
{
	tempPosSign = false;
	componentType = E_Component::Component_Transform;//コンポーネントの種類を設定する
}

void Transform::DoInit()
{
	Component::DoInit();//親クラスの初期化関数を呼び出す
}

void Transform::UnInit()
{
}

/// <summary>
/// 位置を設定する
/// </summary>
/// <param name="p"></param>
/// <param name="updatePrePos"></param>
void Transform::SetPos(vector3s p, bool updatePrePos)
{
	updatePrePos = true;
	if (updatePrePos) {
		prePos = m_pos;
	}
	else {
		tempPosSign = true;
		tempPos = m_pos;
	}
	m_pos = p;
}

void Transform::Copy(const Component& src)
{
	if (src.GetComponentType() != E_Component::Component_Transform) {
		//error
		return;
	}
	try {
		Transform* srcT = (Transform*)(&src);

		this->parent = srcT->parent;
		this->m_pos = srcT->m_pos;
		this->m_rot = srcT->m_rot;
		this->m_scale = srcT->m_scale;

		if (this->parent != NULL) {
			this->parent->RegisterComponent(E_Component::Component_Transform, this, true);
		}
	}
	catch (exception e) {

	}
}

/// <summary>
/// 初期の状態を保存する
/// </summary>
void Transform::RecordCurState()
{
	prePos = m_pos;
	defaultPos = m_pos;
	defaultRot = m_rot;
	defaultScale = m_scale;
}

/// <summary>
/// 初期の状態に戻す
/// </summary>
void Transform::Reset()
{
	prePos = defaultPos;
	m_pos = defaultPos;
	m_rot = defaultRot;
	m_scale = defaultScale;
}

bool Transform::AdjustPosition(BaseCell* cell, vector3s center, vector2s rect, bool inner)
{
	if (cell == NULL)return false;

	vector3s pos = cell->GetTransform()->GetPos();
	vector2s size = cell->GetColliderSize(true, 0);

	if (inner) {
		bool adjustSign = false;
		float border = center.x - rect.x * 0.5f;
		if (pos.x - size.x * 0.5f < border) {
			pos.x = border + size.x * 0.5f;
			adjustSign = true;
		}

		border = center.x + rect.x * 0.5f;
		if (pos.x + size.x * 0.5f > border) {
			pos.x = border - size.x * 0.5f;
			adjustSign = true;
		}

		border = center.y - rect.y * 0.5f;
		if (pos.y - size.y * 0.5f < border) {
			pos.y = border + size.y * 0.5f;
			adjustSign = true;
		}
		border = center.y + rect.y * 0.5f;
		if (pos.y + size.y * 0.5f > border) {
			pos.y = border - size.y * 0.5f;
			adjustSign = true;
		}

		cell->SetPos(pos, true);

		return adjustSign;
	}
	else {
		//bool adjustSign = false;
		vector3s pos2Center = pos - center;
		vector2s tempSize = size + rect;
		float tempX = abs(pos2Center.x - tempSize.x * 0.5f);
		float tempY = abs(pos2Center.y - tempSize.y * 0.5f);

		E_Dir dir = E_Dir::Dir_Up;
		if (abs(rect.x) < 0.000001f) {
			if (pos2Center.y > 0) {
				dir = E_Dir::Dir_Down;
			}
			else {
				dir = E_Dir::Dir_Up;
			}
		}
		else if (abs(rect.y) < 0.000001f) {
			if (pos2Center.x > 0) {
				dir = E_Dir::Dir_Right;
			}
			else {
				dir = E_Dir::Dir_Left;
			}
		}
		else {
			if (abs(pos2Center.x / rect.x) > abs(pos2Center.y / rect.y)) {//yAxis
				if (pos2Center.x > 0) {
					dir = E_Dir::Dir_Right;
				}
				else {
					dir = E_Dir::Dir_Left;
				}
			}
			else {//xAxis
				if (pos2Center.y > 0) {
					dir = E_Dir::Dir_Down;
				}
				else {
					dir = E_Dir::Dir_Up;
				}
			}

		}

		if (dir == E_Dir::Dir_Up) {
			pos.y = center.y - rect.y * 0.5f - size.y * 0.5f - 0.5f;
		}
		else if (dir == E_Dir::Dir_Down) {
			pos.y = center.y + rect.y * 0.5f + size.y * 0.5f + 0.5f;
		}
		else if (dir == E_Dir::Dir_Left) {
			pos.x = center.x - rect.x * 0.5f - size.x * 0.5f - 0.5f;
		}
		else if (dir == E_Dir::Dir_Right) {
			pos.x = center.x + rect.x * 0.5f + size.x * 0.5f + 0.5f;
		}

		cell->SetPos(pos, true);
		return true;
	}

	return false;
}


#pragma endregion Transform_method


#pragma region Collider_method

ColliderData::ColliderData() :id(-1), parent(NULL),isTrigger(false),
colliderType(E_ColliderType::Collider_None), size(vector2s(0.0f, 0.0f)), offset(vector3s(0.0f, 0.0f, 0.0f))
{

}

/// <summary>
/// データをほかのインスタンスからコピペする
/// </summary>
/// <param name="src"></param>
ColliderData::ColliderData(const ColliderData& src)
{
	this->id = src.id;
	//this->parent = src.parent;
	this->parent = NULL;
	this->isTrigger = src.isTrigger;
	this->colliderType = src.colliderType;
	this->size = src.size;
	this->offset = src.offset;

}

ColliderData::ColliderData(int i, Collider* p, bool trigger, E_ColliderType cType, vector2s s, vector3s o) :id(i), parent(p), isTrigger(trigger)
, colliderType(cType), size(s), offset(o) {

}

ColliderData::~ColliderData()
{
	parent = NULL;//削除する時に所有者ポインターの変数をヌルに設定する必要がある
}

void ColliderData::Copy(const ColliderData& src)
{
	this->id = src.id;
	this->parent = src.parent;
	this->isTrigger = src.isTrigger;
	this->colliderType = src.colliderType;
	this->size = src.size;
	this->offset = src.offset;
}

Collider::Collider()
{
	componentType = E_Component::Component_Collider;//コンポーネントの種類を設定する
	this->colliderDataArray.clear();//コライダー配列を初期化する
}

Collider::Collider(const Collider& src) :Component(src.parent, E_Component::Component_Collider)
{
	this->colliderDataArray.clear();//コライダー配列を初期化する
	int arraySize = src.colliderDataArray.size();//コライダーのデータをコピーする
	for (int index = 0; index < arraySize; index++) {
		if (src.colliderDataArray[index] != NULL) {
			this->colliderDataArray.push_back(new ColliderData(*(src.colliderDataArray[index])));
		}
	}

	if (arraySize != this->colliderDataArray.size()) {
		//error
	}
}

Collider::~Collider()
{
	/// <summary>
	/// すべてのコライダーを削除する
	/// </summary>
	int arrayLength = colliderDataArray.size();
	for (int index = 0; index < arrayLength; index++) {
		if (colliderDataArray[index] == NULL)continue;
		delete colliderDataArray[index];
	}

	colliderDataArray.clear();

}

void Collider::DoInit()
{
	Component::DoInit(); // 親クラスの初期化関数を呼び出す
}

void Collider::UnInit()
{
}

/// <summary>
/// コライダーを取得する
/// </summary>
/// <param name="id">　コライダーのID　</param>
/// <returns></returns>
ColliderData* Collider::GetColliderData( int id)
{

	int size = colliderDataArray.size();
	for (int index = 0; index < size; index++) {
		if (colliderDataArray[index] == NULL)continue;

		if ( colliderDataArray[index]->id == id) {
			return colliderDataArray[index];
		}
	}

	return nullptr;
}

/// <summary>
/// コライダーデータを追加する
/// </summary>
/// <param name="data"></param>
void Collider::AddColliderData(ColliderData* data) {
	if (data == NULL)return;
	colliderDataArray.push_back(data);
}

/// <summary>
/// コライダーデータを全部削除する
/// </summary>
void Collider::ClearColliderDataArray()
{
	int arrayLength = colliderDataArray.size();
	for (int index = 0; index < arrayLength; index++) {
		if (colliderDataArray[index] == NULL)continue;
		delete colliderDataArray[index];
	}

	colliderDataArray.clear();
}

void Collider::Copy(const Component& src)
{
	if (src.GetComponentType() != E_Component::Component_Collider)return;
	Collider* tempC = (Collider*)(&src);
	int colliderDataSize = tempC->GetColliderDataArraySize();
	if (tempC == NULL || colliderDataSize == 0)return;
	ClearColliderDataArray();
	for (int index = 0; index < colliderDataSize; index++) {
		if (tempC->colliderDataArray[index] != NULL) {
			this->colliderDataArray.push_back(new ColliderData(*tempC->colliderDataArray[index]));
		}
	}
}

/// <summary>
/// コライダーのサイズを設定する
/// </summary>
/// <param name="size"></param>
/// <param name="colliderDataIndex">　</param>
void Collider::SetColliderSize(vector2s size,int colliderDataIndex)
{
	if (colliderDataArray.size() == 0 || colliderDataArray.size() <=colliderDataIndex || colliderDataArray[colliderDataIndex] == NULL)return;
	colliderDataArray[colliderDataIndex]->size = size;
}

void Collider::SetColliderOffset(vector3s _offset, int _colliderDataIndex)
{
	if (colliderDataArray.size() == 0 || colliderDataArray.size() <= _colliderDataIndex || colliderDataArray[_colliderDataIndex] == NULL)return;
	colliderDataArray[_colliderDataIndex]->offset = _offset;
}

/// <summary>
/// 当たり判定（円と円、円と四角形、四角形と四角形）
/// </summary>
/// <param name="cell_1">　一番目のゲームオブジェクト</param>
/// <param name="cell_2">　二番目のゲームオブジェクト</param>
/// <returns></returns>
bool Collider::CollisionDetect(BaseCell* cell_1, BaseCell* cell_2)
{
	if (cell_1 == NULL || cell_2 == NULL)return false;
	if (cell_1 == cell_2)return true;

	//コライダーコンポーネントを取得
	Collider* c_1 = (Collider*)cell_1->GetComponent(E_Component::Component_Collider);
	Collider* c_2 = (Collider*)cell_2->GetComponent(E_Component::Component_Collider);

	if (c_1 == NULL || c_2 == NULL)return false;
	if (!c_1->enable || !c_2->enable)
		return false;

	//コライダーデータの配列を取得
	vector<ColliderData*> c_array_1 = c_1->GetColliderDataArray();
	vector<ColliderData*> c_array_2 = c_2->GetColliderDataArray();
	int size_1 = c_array_1.size();
	int size_2 = c_array_2.size();
	if (size_1 == 0 || size_2 == 0)return false;

	for (int index_1 = 0; index_1 < size_1; index_1++) {
		if (c_array_1[index_1] == NULL)continue;

		for (int index_2 = 0; index_2 < size_2; index_2++) {

			if (c_array_2[index_2] == NULL)continue;

			if (c_array_1[index_1]->colliderType == c_array_2[index_2]->colliderType) {//コライダー種類が同じの場合
				if (c_array_1[index_1]->colliderType == E_ColliderType::Collider_Rect) {//四角形と四角形
					if (RectCollisionDetect(*(cell_1->GetTransform()), *(cell_2->GetTransform()), *c_array_1[index_1], *c_array_2[index_2])) {
						return true;
					}
				}
				else {//円と円
					if (CircleCollisionDetect(*(cell_1->GetTransform()), *(cell_2->GetTransform()), *c_array_1[index_1], *c_array_2[index_2])) {
						return true;
					}
				}
			}
			else {//コライダー種類が違う場合　‐　円と四角形
				if (RectCircleCollisionDetect(*(cell_1->GetTransform()), *(cell_2->GetTransform()), *c_array_1[index_1], *c_array_2[index_2])) {
					return true;
				}
			}
		}
	}


	return false;
}

bool Collider::CollisionDetect(BaseCell* cell_1, Transform* transform_2, Collider* c_2) {
	if (cell_1 == NULL || transform_2 == NULL || c_2 == NULL)return false;

	Collider* c_1 = (Collider*)cell_1->GetComponent(E_Component::Component_Collider);

	if (c_1 == NULL)return false;

	vector<ColliderData*> c_array_1 = c_1->GetColliderDataArray();
	vector<ColliderData*> c_array_2 = c_2->GetColliderDataArray();
	int size_1 = c_array_1.size();
	int size_2 = c_array_2.size();
	if (size_1 == 0 || size_2 == 0)return false;

	for (int index_1 = 0; index_1 < size_1; index_1++) {
		if (c_array_1[index_1] == NULL)continue;

		for (int index_2 = 0; index_2 < size_2; index_2++) {

			if (c_array_2[index_2] == NULL)continue;

			if (c_array_1[index_1]->colliderType == c_array_2[index_2]->colliderType) {
				if (c_array_1[index_1]->colliderType == E_ColliderType::Collider_Rect) {
					if (RectCollisionDetect(*(cell_1->GetTransform()), *transform_2, *c_array_1[index_1], *c_array_2[index_2])) {
						return true;
					}
				}
				else {
					if (CircleCollisionDetect(*(cell_1->GetTransform()), *transform_2, *c_array_1[index_1], *c_array_2[index_2])) {
						return true;
					}
				}
			}
			else {
				if (RectCircleCollisionDetect(*(cell_1->GetTransform()), *transform_2, *c_array_1[index_1], *c_array_2[index_2])) {
					return true;
				}
			}
		}
	}
	return false;
}

/// <summary>
/// 円と四角形の当たり判定
/// </summary>
/// <param name="transform_1">　一番目のゲームオブジェクトのトランスフォーマーデータ</param>
/// <param name="transform_2">　二番目のゲームオブジェクトのトランスフォーマーデータ</param>
/// <param name="data_1">一番目のゲームオブジェクトのコライダーデータ</param>
/// <param name="data_2">二番目のゲームオブジェクトのコライダーデータ</param>
/// <returns>　当たるかどうか</returns>
bool Collider::RectCircleCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2)
{
	ColliderData& rectC = data_1.colliderType == E_ColliderType::Collider_Rect ? data_1 : data_2;
	Transform& rectT = data_1.colliderType == E_ColliderType::Collider_Rect ? transform_1 : transform_2;
	ColliderData& circleC = data_1.colliderType == E_ColliderType::Collider_Circle ? data_1 : data_2;
	Transform& circleT = data_1.colliderType == E_ColliderType::Collider_Circle ? transform_1 : transform_2;

	//situation 1
	vector3s rectCenter = vector3s(0.0f, 0.0f, 0.0f);
	float rectWidth = rectC.size.x * rectT.GetScale().x;
	float rectHeight = rectC.size.y * rectT.GetScale().y;


	vector3s rot = rectT.GetRot();


	//get rectStartPoint(rotated)
	vector3s rectStartPoint = rectCenter;
	rectStartPoint.x = rectCenter.x - rectWidth * 0.5f;
	rectStartPoint.y = rectCenter.y - rectHeight * 0.5f;

	float tempX = rectStartPoint.x;
	float tempY = rectStartPoint.y;
	if ((int)(rot.z) != 0) {
		rectStartPoint.x = tempX * cosf(rot.z * DEG2RAD) - tempY * sinf(rot.z * DEG2RAD);
		rectStartPoint.y = tempX * sinf(rot.z * DEG2RAD) + tempY * cosf(rot.z * DEG2RAD);
	}

	rectStartPoint.x += rectC.offset.x + rectT.GetPos().x;
	rectStartPoint.y += rectC.offset.y + rectT.GetPos().y;

	float radius = circleC.size.x * circleT.GetScale().x * 0.5f;

	vector3s circleCenter = circleT.GetPos() + circleC.offset;
	vector3s rect2Circle = circleCenter - rectStartPoint;

	//rotate to rect coordinate
	if ((int)(rot.z) != 0) {
		tempX = rect2Circle.x;
		tempY = rect2Circle.y;
		rect2Circle.x = tempX * cosf(-rot.z * DEG2RAD) - tempY * sinf(-rot.z * DEG2RAD);
		rect2Circle.y = tempX * sinf(-rot.z * DEG2RAD) + tempY * cosf(-rot.z * DEG2RAD);
	}

	//collision detect
	rectStartPoint.x = 0.0f;//set startPoint to originPoint
	rectStartPoint.y = 0.0f;

	vector3s nearestPoint;
	nearestPoint.z = 0.0f;
	nearestPoint.x = s_max(rectStartPoint.x, s_min(rect2Circle.x, rectStartPoint.x + rectWidth));
	nearestPoint.y = s_max(rectStartPoint.y, s_min(rect2Circle.y, rectStartPoint.y + rectHeight));

	if (VecDist(rect2Circle, nearestPoint) < radius) {
		return true;
	}
	else {
		return false;
	}
}

bool Collider::RectCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2)
{
	vector3s rot_1 = transform_1.GetRot();
	vector3s rot_2 = transform_2.GetRot();

	vector2s rectSize_1, rectSize_2;
	//rectSize_1 = data_1.size * vector2s(transform_1.GetScale().x, transform_1.GetScale().y);
	//rectSize_2 = data_2.size * vector2s(transform_2.GetScale().x, transform_2.GetScale().y);
	rectSize_1.x = data_1.size.x * transform_1.GetScale().x;
	rectSize_1.y = data_1.size.y * transform_1.GetScale().y;
	rectSize_2.x = data_2.size.x * transform_2.GetScale().x;
	rectSize_2.y = data_2.size.y * transform_2.GetScale().y;

	vector3s center_1 = transform_1.GetPos() + data_1.offset;
	vector3s center_2 = transform_2.GetPos() + data_2.offset;
	vector2s centerV = vector2s(center_1.x - center_2.x, center_1.y - center_2.y);
	bool sign = false;
	if ((int)(rot_1.z) == 0 && (int)(rot_2.z) == 0) {//回転なし当たり判定
		//AABBバウンディングボックス
		if (fabs(centerV.x) < (static_cast<double>(rectSize_1.x) + static_cast<double>(rectSize_2.x)) * 0.5f && fabs(centerV.y) < (static_cast<double>(rectSize_1.y) +static_cast<double>(rectSize_2.y)) * 0.5f) {
			return true;
		}
		else {
			return false;
		}
	}
	else {//回転あり当たり判定
		//OBBバウンディングボックス
		//(Separating axis theorem）分離軸判定法アルゴリズム
		vector2s axisA_1, axisA_2, axisB_1, axisB_2;
		if ((int)(rot_1.z) != 0) {
			float cos_1 = cosf(rot_1.z * DEG2RAD);
			float sin_1 = sinf(rot_1.z * DEG2RAD);
			axisA_1.x = cos_1;
			axisA_1.y = sin_1;
			axisA_2.x = -sin_1;
			axisA_2.y = cos_1;
		}
		else {
			axisA_1.x = 1;
			axisA_1.y = 0;
			axisA_2.x = 0;
			axisA_2.y = 1;
		}

		if ((int)(rot_2.z) != 0) {
			float cos_2 = cosf(rot_2.z * DEG2RAD);
			float sin_2 = sinf(rot_2.z * DEG2RAD);
			axisB_1.x = cos_2;
			axisB_1.y = sin_2;
			axisB_2.x = -sin_2;
			axisB_2.y = cos_2;
		}
		else {
			axisB_1.x = 1;
			axisB_1.y = 0;
			axisB_2.x = 0;
			axisB_2.y = 1;
		}

		axisA_1 = VecNormalize(axisA_1);
		axisA_2 = VecNormalize(axisA_2);
		axisB_1 = VecNormalize(axisB_1);
		axisB_2 = VecNormalize(axisB_2);

		//projection 1
		float dotV_1 = fabsf(VecDot(axisA_1, axisB_1 * (rectSize_2.x * 0.5f)));
		float dotV_2 = fabsf(VecDot(axisA_1, axisB_2 * (rectSize_2.y * 0.5f)));
		float centerDotV_1 = fabsf(VecDot(axisA_1, centerV));

		if (centerDotV_1 >= dotV_1 + dotV_2 + rectSize_1.x * 0.5f) {
			return false;
		}

		dotV_1 = fabsf(VecDot(axisA_2, axisB_1 * (rectSize_2.x * 0.5f)));
		dotV_2 = fabsf(VecDot(axisA_2, axisB_2 * (rectSize_2.y * 0.5f)));
		centerDotV_1 = fabsf(VecDot(axisA_2, centerV));
		if (centerDotV_1 >= dotV_1 + dotV_2 + rectSize_1.y * 0.5f) {
			return false;
		}

		dotV_1 = fabsf(VecDot(axisB_1, axisA_1 * (rectSize_1.x * 0.5f)));
		dotV_2 = fabsf(VecDot(axisB_1, axisA_2 * (rectSize_1.y * 0.5f)));
		centerDotV_1 = fabsf(VecDot(axisB_1, centerV));
		if (centerDotV_1 >= dotV_1 + dotV_2 + rectSize_2.x * 0.5f) {
			return false;
		}

		dotV_1 = fabsf(VecDot(axisB_2, axisA_1 * (rectSize_1.x * 0.5f)));
		dotV_2 = fabsf(VecDot(axisB_2, axisA_2 * (rectSize_1.y * 0.5f)));
		centerDotV_1 = fabsf(VecDot(axisB_2, centerV));
		if (centerDotV_1 >= dotV_1 + dotV_2 + rectSize_2.y * 0.5f) {
			return false;
		}

		return true;

	}

}

bool Collider::CircleCollisionDetect(Transform& transform_1, Transform& transform_2, ColliderData& data_1, ColliderData& data_2)
{
	vector3s circleCenter_1 = transform_1.GetPos() + data_1.offset;
	vector3s circleCenter_2 = transform_2.GetPos() + data_2.offset;
	float radius_1 = transform_1.GetScale().x * data_1.size.x * 0.5f;
	float radius_2 = transform_2.GetScale().x * data_2.size.x * 0.5f;

	if (VecDistS(circleCenter_1, circleCenter_2) > (radius_1 + radius_2) * (radius_1 + radius_2)) {
		return false;
	}
	else {
		return true;
	}

}

bool Collider::IsContainPoint(BaseCell* cell, vector3s point)
{
	if (cell == NULL)return false;
	Collider* c = (Collider*)cell->GetComponent(E_Component::Component_Collider);
	if (c == NULL)return false;

	int size = c->GetColliderDataArraySize();
	if (size == 0)return false;

	vector<ColliderData*>& colliderDataArray = c->GetColliderDataArray();
	for (int index = 0; index < size; index++) {
		if (colliderDataArray[index] == NULL)continue;
		if (colliderDataArray[index]->colliderType == E_ColliderType::Collider_Rect) {
			if (IsRectContainPoint(cell->GetTransform(), *colliderDataArray[index], point)) {
				return true;
			}
		}
		else {
			if (IsCircleContainPoint(cell->GetTransform(), *colliderDataArray[index], point)) {
				return true;
			}
		}
	}
	return false;
}

bool Collider::IsRectContainPoint(Transform* transform, ColliderData& data, vector3s point)
{
	if (transform == NULL)return false;

	vector3s pos = transform->GetPos() + data.offset;
	vector2s size = vector2s(transform->GetScale().x, transform->GetScale().y) * data.size;
	vector3s rot = transform->GetRot();
	vector2s center2Point = vector2s(pos.x - point.x, pos.y - point.y);

	if ((int)(rot.z) == 0) {
		if (fabsf(center2Point.x) > size.x * 0.5f || fabsf(center2Point.y) > size.y * 0.5f) {
			return false;
		}
		else {
			return true;
		}

	}
	else {

		vector2s xAxis;
		vector2s yAxis;

		float cos_1 = cosf(rot.z * DEG2RAD);
		float sin_1 = sinf(rot.z * DEG2RAD);
		xAxis.x = cos_1;
		xAxis.y = sin_1;
		yAxis.x = -sin_1;
		yAxis.y = cos_1;

		xAxis = VecNormalize(xAxis);
		yAxis = VecNormalize(yAxis);

		float dotV = fabsf(VecDot(xAxis, center2Point));
		if (dotV > size.x * 0.5f) {
			return false;
		}

		dotV = fabsf(VecDot(yAxis, center2Point));
		if (dotV > size.y * 0.5f) {
			return false;
		}
		return true;
	}
}

bool Collider::IsRectContainPoint(vector3s pos, vector2s size, vector3s rot, vector3s point)
{

	vector2s center2Point = vector2s(pos.x - point.x, pos.y - point.y);

	if ((int)(rot.z) == 0) {
		if (fabsf(center2Point.x) > size.x * 0.5f || fabsf(center2Point.y) > size.y * 0.5f) {
			return false;
		}
		else {
			return true;
		}
	}
	else {

		vector2s xAxis;
		vector2s yAxis;

		float cos_1 = cosf(rot.z * DEG2RAD);
		float sin_1 = sinf(rot.z * DEG2RAD);
		xAxis.x = cos_1;
		xAxis.y = sin_1;
		yAxis.x = -sin_1;
		yAxis.y = cos_1;

		xAxis = VecNormalize(xAxis);
		yAxis = VecNormalize(yAxis);

		float dotV = fabsf(VecDot(xAxis, center2Point));
		if (dotV > size.x * 0.5f) {
			return false;
		}

		dotV = fabsf(VecDot(yAxis, center2Point));
		if (dotV > size.y * 0.5f) {
			return false;
		}
		return true;
	}

	return false;
}

bool Collider::IsCircleContainPoint(Transform* transform, ColliderData& data, vector3s point)
{
	if (transform == NULL)return false;

	vector3s pos = transform->GetPos();
	float radius = transform->GetScale().x * data.size.x * 0.5f;

	float dist_s = VecDistS(pos, point);
	if (dist_s > radius * radius) {
		return false;
	}
	else {
		return true;
	}
}

bool Collider::IsCircleContainPoint(vector3s pos, float radius, vector3s point)
{
	float dist_s = VecDistS(pos, point);
	if (dist_s > radius * radius) {
		return false;
	}
	else {
		return true;
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="cell_1">  detect target cell  </param>
/// <param name="transform_2">  scene obj   </param>
/// <param name="c_2"></param>
/// <returns></returns>
bool Collider::SceneObjCollisionDetect(BaseCell* cell_1, Transform* transform_2, Collider* c_2)
{
	if (cell_1 == NULL || transform_2 == NULL || c_2 == NULL)return false;
	Transform* transform_1 = cell_1->GetTransform();
	Collider* c_1 = (Collider*)cell_1->GetComponent(E_Component::Component_Collider);
	if (c_1 == NULL)return false;
	vector<ColliderData*> c_array_1 = c_1->GetColliderDataArray();
	vector<ColliderData*> c_array_2 = c_2->GetColliderDataArray();
	int size_1 = c_array_1.size();
	int size_2 = c_array_2.size();
	if (size_1 == 0 || size_2 == 0)return false;
	ColliderData* data_1 = NULL;
	ColliderData* data_2 = NULL;
	for (int index_1 = 0; index_1 < size_1; index_1++) {
		data_1 = c_array_1[index_1];
		if (data_1 == NULL) {
			continue;
		}
		for (int index_2 = 0; index_2 < size_2; index_2++) {
			data_2 = c_array_2[index_2];
			if (data_2 == NULL) {
				continue;
			}
			if (data_1->colliderType != E_ColliderType::Collider_Rect || data_2->colliderType != E_ColliderType::Collider_Rect)return false;
			vector2s rectSize_1, rectSize_2;
			rectSize_1 = data_1->size * vector2s(transform_1->GetScale().x, transform_1->GetScale().y);
			rectSize_2 = data_2->size * vector2s(transform_2->GetScale().x, transform_2->GetScale().y);
			vector3s center_1 = transform_1->GetPos() + data_1->offset;
			vector3s center_2 = transform_2->GetPos() + data_2->offset;
			vector2s centerV = vector2s(center_1.x - center_2.x, center_1.y - center_2.y);
			vector3s detectBoxCenter = vector3s(0, 0, 0);
			vector2s detectBoxRect = vector2s(0, 0);
			vector3s detectPointCenter = vector3s(0, 0, 0);
			vector2s detectPointRect = vector2s(0, 0);
			//xAixs
			if (rectSize_1.y > rectSize_2.y) {
				detectBoxCenter = center_1;
				detectBoxRect = rectSize_1;
				detectPointCenter = center_2;
				detectPointRect = rectSize_2;
			}
			else {
				detectBoxCenter = center_2;
				detectBoxRect = rectSize_2;
				detectPointCenter = center_1;
				detectPointRect = rectSize_1;
			}
			int containPointCounter = 0;
			vector3s detectPoint = detectPointCenter;
			vector3s secondDetectPoint = detectPointCenter;
			detectPoint.x = detectPointCenter.x - detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//left center point
				return true;
			}
			detectPoint.x = detectPointCenter.x - detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y + detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//LT
				//second detect
				secondDetectPoint.x = detectPoint.x + SECOND_DETECT_OFFSET_X;
				secondDetectPoint.y = detectPoint.y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
				secondDetectPoint.x = detectPoint.x;
				secondDetectPoint.y = detectPoint.y - SECOND_DETECT_OFFSET_Y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
			}
			detectPoint.x = detectPointCenter.x + detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//right center point
				return true;
			}
			detectPoint.x = detectPointCenter.x + detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y + detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//RT
				//second detect
				secondDetectPoint.x = detectPoint.x - SECOND_DETECT_OFFSET_X;
				secondDetectPoint.y = detectPoint.y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
				secondDetectPoint.x = detectPoint.x;
				secondDetectPoint.y = detectPoint.y - SECOND_DETECT_OFFSET_Y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
			}
			//yAixs
			if (rectSize_1.x > rectSize_2.x) {
				detectBoxCenter = center_1;
				detectBoxRect = rectSize_1;
				detectPointCenter = center_2;
				detectPointRect = rectSize_2;
			}
			else {
				detectBoxCenter = center_2;
				detectBoxRect = rectSize_2;
				detectPointCenter = center_1;
				detectPointRect = rectSize_1;
			}
			detectPoint.x = detectPointCenter.x;
			detectPoint.y = detectPointCenter.y + detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//up center point
				return true;
			}
			detectPoint.x = detectPointCenter.x - detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y - detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//LB
				//second detect
				secondDetectPoint.x = detectPoint.x + SECOND_DETECT_OFFSET_X;
				secondDetectPoint.y = detectPoint.y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
				secondDetectPoint.x = detectPoint.x;
				secondDetectPoint.y = detectPoint.y + SECOND_DETECT_OFFSET_Y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
			}
			detectPoint.x = detectPointCenter.x;
			detectPoint.y = detectPointCenter.y - detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//down center point
				return true;
			}
			detectPoint.x = detectPointCenter.x + detectPointRect.x * 0.5f;
			detectPoint.y = detectPointCenter.y - detectPointRect.y * 0.5f;
			if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), detectPoint)) {//RB
				//second detect
				secondDetectPoint.x = detectPoint.x - SECOND_DETECT_OFFSET_X;
				secondDetectPoint.y = detectPoint.y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
				secondDetectPoint.x = detectPoint.x;
				secondDetectPoint.y = detectPoint.y + SECOND_DETECT_OFFSET_Y;
				if (IsRectContainPoint(detectBoxCenter, detectBoxRect, vector3s(0, 0, 0), secondDetectPoint)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Collider::LineCollisionDetect(BaseCell* cell_1, BaseCell* cell_2,vector3s& collisionPoint,vector3s& collisionNormal)
{
	if (cell_1 == NULL || cell_2 == NULL)return false;

	Collider* collider = (Collider*)cell_1->GetComponent(E_Component::Component_Collider);
	if (collider == NULL)return false;
	if (collider->GetColliderDataArraySize() == 0)return false;

	ColliderData* data = collider->GetColliderDataArray()[0];
	if (data == NULL)return false;

	vector3s pos = cell_1->GetTransform()->GetPos();
	vector3s scale = cell_1->GetTransform()->GetScale();
	vector2s size = data->size;

	vector3s startPos = vector3s(-size.x * scale.x * 0.5f, 0.0f, 0.0f);
	vector3s endPos = vector3s(size.x * scale.x * 0.5f, 0.0f, 0.0f);

	//rot
	vector3s rot = cell_1->GetTransform()->GetRot();
	if ((int)(rot.z) != 0) {

		float cosRotZ = cosf(rot.z * DEG2RAD);
		float sinRotZ = sinf(rot.z * DEG2RAD);

		float tempX = startPos.x;
		float tempY = startPos.y;

		startPos.x = (tempX * cosRotZ - tempY * sinRotZ);
		startPos.y = (tempX * sinRotZ + tempY * cosRotZ);

		tempX = endPos.x;
		tempY = endPos.y;

		endPos.x = (tempX * cosRotZ - tempY * sinRotZ);
		endPos.y = (tempX * sinRotZ + tempY * cosRotZ);
	}

	startPos = pos + startPos;
	endPos = pos + endPos;
	//vector3s startPos = pos;
	//startPos.x -= size.x * scale.x * 0.5f;
	//vector3s endPos = pos;
	//endPos.x += size.x * scale.x * 0.5f;

	return LineCollisionDetect(startPos, endPos, cell_2,collisionPoint, collisionNormal);
}

bool Collider::LineCollisionDetect(vector3s startPos, vector3s endPos, BaseCell* cell,vector3s& collisionPoint,vector3s& collisionNormal)
{
	if (cell == NULL)return false;

	Collider* collider = (Collider*)cell->GetComponent(E_Component::Component_Collider);
	if (collider == NULL)return false;
	if (collider->GetColliderDataArraySize() == 0)return false;

	ColliderData* data = collider->GetColliderDataArray()[0];
	if (data == NULL)return false;

	vector3s pos = cell->GetTransform()->GetPos();
	vector3s scale = cell->GetTransform()->GetScale();
	vector3s rot = cell->GetTransform()->GetRot();
	vector2s size = data->size;

	float halfX = size.x * scale.x * 0.5f;
	float halfY = size.y * scale.y * 0.5f;

	vector3s lt_point = vector3s(pos.x - halfX, pos.y - halfY,0.0f);
	vector3s rt_point = vector3s(pos.x + halfX, pos.y - halfY, 0.0f);
	vector3s lb_point = vector3s(pos.x - halfX, pos.y + halfY, 0.0f);
	vector3s rb_point = vector3s(pos.x + halfX, pos.y + halfY, 0.0f);
	//rot
	if ((int)(rot.z) != 0) {

		float cosRotZ = cosf(rot.z * DEG2RAD);
		float sinRotZ = sinf(rot.z * DEG2RAD);

		float tempX = lt_point.x;
		float tempY = lt_point.y;
		lt_point.x = (tempX * cosRotZ - tempY * sinRotZ);
		lt_point.y = (tempX * sinRotZ + tempY * cosRotZ);

		tempX = rt_point.x;
		tempY = rt_point.y;
		rt_point.x = (tempX * cosRotZ - tempY * sinRotZ);
		rt_point.y = (tempX * sinRotZ + tempY * cosRotZ);

		tempX = lb_point.x;
		tempY = lb_point.y;
		lb_point.x = (tempX * cosRotZ - tempY * sinRotZ);
		lb_point.y = (tempX * sinRotZ + tempY * cosRotZ);

		tempX = rb_point.x;
		tempY = rb_point.y;
		rb_point.x = (tempX * cosRotZ - tempY * sinRotZ);
		rb_point.y = (tempX * sinRotZ + tempY * cosRotZ);
	}

	vector3s topEdge = rt_point - lt_point;
	vector3s leftEdge = lt_point - lb_point;
	vector3s bottomEdge = lb_point - rb_point;
	vector3s rightEdge = rb_point - rt_point;

	bool collisionSign = false;
	vector3s nearestPoint = endPos;
	vector3s normal = vector3s(0, 0, 0);
	//line collision detect
	vector3s collisionPoint_topEdge = vector3s();
	if (LineCollisionDetect(startPos, endPos, rt_point, lt_point, collisionPoint_topEdge) == true) {//top edge
		if (VecDistS(nearestPoint, startPos) > VecDistS(collisionPoint_topEdge, startPos)) {
			nearestPoint = collisionPoint_topEdge;
			normal = upVector;
			collisionSign = true;
		}
	}

	vector3s collisionPoint_leftEdge = vector3s();
	if (LineCollisionDetect(startPos, endPos, lt_point, lb_point, collisionPoint_leftEdge) == true) {
		if (VecDistS(nearestPoint, startPos) > VecDistS(collisionPoint_leftEdge, startPos)) {
			nearestPoint = collisionPoint_leftEdge;
			normal = leftVector;
			collisionSign = true;
		}
	}

	vector3s collisionPoint_bottomEdge = vector3s();
	if (LineCollisionDetect(startPos, endPos, lb_point, rb_point, collisionPoint_bottomEdge) == true) {
		if (VecDistS(nearestPoint, startPos) > VecDistS(collisionPoint_bottomEdge, startPos)) {
			nearestPoint = collisionPoint_bottomEdge;
			normal = downVector;
			collisionSign = true;
		}
	}

	vector3s collisionPoint_rightEdge = vector3s();
	if (LineCollisionDetect(startPos, endPos, rb_point , rt_point, collisionPoint_rightEdge) == true) {
		if (VecDistS(nearestPoint, startPos) > VecDistS(collisionPoint_rightEdge, startPos)) {
			nearestPoint = collisionPoint_rightEdge;
			normal = rightVector;
			collisionSign = true;
		}
	}

	//normal rot
	if ((int)(rot.z) != 0) {
		float cosRotZ = cosf(rot.z * DEG2RAD);
		float sinRotZ = sinf(rot.z * DEG2RAD);

		float tempX = normal.x;
		float tempY = normal.y;
		normal.x = (tempX * cosRotZ - tempY * sinRotZ);
		normal.y = (tempX * sinRotZ + tempY * cosRotZ);
	}
	normal.z = 0.0f;

	collisionNormal = VecNormalize(normal);
	collisionPoint = nearestPoint;

	return collisionSign;
}

bool Collider::LineCollisionDetect(vector3s a, vector3s b, vector3s c, vector3s d, vector3s& collisionPos)
{
	float s_abc = (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
	float s_abd = (a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);
	if (s_abc * s_abd >= 0) {
		return false;
	}

	float s_cda = (c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
	float s_cdb = s_cda + s_abc - s_abd;
	if (s_cda * s_cdb >= 0) {
		return false;
	}

	float dx = 0.0f;
	float dy = 0.0f;
	float t = s_abd - s_abc;
	if (abs(t) > 0.000001f) {
		t = s_cda / t;
		dx = t * (b.x - a.x);
		dy = t * (b.y - a.y);
	}
	collisionPos.x = a.x + dx;
	collisionPos.y = a.y + dy;

	return true;
}


#pragma endregion Collider_method


#pragma region Animation_method

#pragma region Anim_callback

void Anim_Effect_Normal_CB::Func(AnimClip* baseClip) {
	if (baseClip == NULL || baseClip->GetParent() == NULL)return;

	BaseCell* parent = baseClip->GetParent();
	if (parent != NULL) {
		parent->Recycle();
		//parent->DeleteCell();
	}
}

void Anim_Player_Win_CB::Func(AnimClip* _baseClip)
{
	if (GameMgr::GetInstance()->sceneMgr->switchSceneSign == false) {
		GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_End, true, true);
	}
}



#pragma endregion Anim_callback

AnimClip::~AnimClip()
{
	parent = NULL;//所有者ポインターをヌルに設定する

	animStart_Callback = NULL;
	animEnd_Callback = NULL;
}

void AnimClip::DoUpdate(float deltatime)
{
	if (animState) {
		animTimePass += deltatime;

		if (animTimePass > animTime) {
			if (loop) {//繰り返しアニメーションであれば
				animTimePass = 0.0f;
			}
			else {
				animState = false;
				OnAnimEnd();//アニメーションが終了、終了コールバック関数を呼び出す（終了処理）
				return;
			}
		}

		float progress = animTimePass * animTime_I;
		if (dir) {//アニメーションの方向
			curIndex = I_Lerp(frameIndexRange.x, frameIndexRange.y, progress);//テクスチャ―座標を補間関数で取得
		}
		else {
			curIndex = I_Lerp(frameIndexRange.y, frameIndexRange.x, progress);//テクスチャ―座標を補間関数で取得
		}

		//描画データを取得する、テクスチャ―座標を変える
		if (parent != NULL) {
			DrawData* data = parent->GetDrawData();
			if (data != NULL) {
				data->SetShaderIntParam(3, curIndex);//テクスチャ―座標を変える
			}
		}

	}
}

/// <summary>
/// アニメーションを追加する
/// </summary>
/// <param name="animType">　アニメーションの種類　</param>
/// <param name="anim">　アニメーションのインスタンス</param>
/// <param name="setDefaultAnim">　これはデフォルトアニメションかどうか　</param>
void AnimFSM::RegisterAnim(E_Anim animType, AnimClip* anim, bool setDefaultAnim)
{
	if (anim == NULL)return;

	if (animDic.find(animType) != animDic.end()) {
		//had same value
	}
	else {
		animDic[animType] = anim;//リストに追加する
		if (parent != NULL) {
			anim->SetParent(parent->GetParent());//所有者を設定する
		}
		else {
			//error
		}
		if (setDefaultAnim) {//このアニメーションをデフォルトアニメーションに設定する
			SwitchAnim(animType, true, false);
			defaultAnim = animType;
		}
	}
}

void Animation::DoInit()
{
	Component::DoInit();//親クラスの初期化関数を呼び出す
	animFSM.DoInit();//アニメーションを管理するクラスを初期化する
}

void Animation::UnInit()
{
}

void Animation::SetParent(BaseCell* p)
{
	Component::SetParent(p);
	animFSM.SetParent(this);
}

void Animation::DoUpdate(float deltatime)
{
	if (enable) {
		animFSM.DoUpdate(deltatime);
	}
	Component::DoUpdate(deltatime);//親クラスのアップデート関数を呼び出す
}

void Animation::DoLateUpdate(float deltatime)
{
	animFSM.DoLateUpdate(deltatime);
}

void Animation::Copy(const Component& src)
{
	if (src.GetComponentType() != E_Component::Component_Animation) {
		//error
		return;
	}
	try {

		Animation* srcA = (Animation*)(&src);

		this->parent = srcA->parent;
		//TODO

	}
	catch (exception e) {

	}
}

/// <summary>
/// アニメーションを切り替える
/// </summary>
/// <param name="targetAnim">　切り替えたいアニメーションの種類</param>
/// <param name="resetAnim">　今のアニメーションと同じの場合、リセットするかどうか</param>
/// <param name="lastClipEndCB">　今のアニメーションの終了処理関数を呼び出すかどうか</param>
void Animation::SwitchAnim(E_Anim targetAnim, bool resetAnim, bool lastClipEndCB)
{
	animFSM.SwitchAnim(targetAnim, resetAnim, lastClipEndCB);
}

/// <summary>
/// デフォルトアニメーションに切り替える
/// </summary>
void Animation::SwitchToDefaultAnim()
{
	animFSM.DoInit();
}

/// <summary>
/// 今のアニメーションを停止する（デフォルトアニメーションに切り替える
/// </summary>
void Animation::StopAnim()
{
	animFSM.DoInit();
	animFSM.SetAnimState(false);
}

/// <summary>
/// アニメーションを状態を設定する（一時停止
/// </summary>
/// <param name="pause"></param>
void Animation::PauseAnim(bool pause)
{
	animFSM.SetAnimState(!pause);
}


#pragma endregion Animation_method;

#pragma region ai_method


SimplePatrolAI::SimplePatrolAI():
	parent(NULL),patrolType(E_PatrolType::PatrolType_Const),forward(true),defaultForward(true),curStartPointIndex(0) {
	patrolPoint.clear();
	defaultPoint = vector3s(0, 0, 0);
	curPos = curStartPoint = curTargetPoint = defaultPoint;

	nextPointInterval = timePass = 0.0f;
}

SimplePatrolAI::~SimplePatrolAI() {
	parent = NULL;
	patrolPoint.clear();
}

void SimplePatrolAI::DoInit()
{
	Reset();
}

SimplePatrolAI::SimplePatrolAI(BaseCell* p, E_PatrolType patrolT, vector3s defaultP, float nextInterval, bool defaultF) :
	parent(p), patrolType(patrolT), defaultPoint(defaultP), defaultForward(defaultF), forward(defaultF), nextPointInterval(nextInterval), timePass(0.0f),
	curStartPoint(defaultP), curTargetPoint(defaultP), curPos(defaultP), curStartPointIndex(0),
	moveDone(false), curMoveTime(0.0f), curMoveTimePass(0.0f)
{

}

void SimplePatrolAI::DoUpdate(float deltatime) {

	if (patrolType == E_PatrolType::PatrolType_Const) {
		return;
	}

	if (moveDone == true) {//switch 2 next point

		if (parent != NULL) {
			//set idle anim
			Animation* anim = (Animation*)parent->GetComponent(E_Component::Component_Animation);
			if (anim != NULL) {
				anim->SwitchAnim(E_Anim::Anim_Idle, false, false);
			}
		}

		if (timePass >= nextPointInterval) {//interval count down done
			
			SwitchNextPoint(false);

			float dist = VecDist(curStartPoint, curTargetPoint);
			curMoveTime = dist / AI_NPC_MOVE_SPEED;
			curMoveTimePass = 0.0f;
			timePass = 0.0f;
			moveDone = false;
		}
		else {
			timePass += deltatime;
		}
	}
	else {//move
		curMoveTimePass += deltatime;
		float progress = 0;
		if (curMoveTime <= 0.0000001f) {
			progress = 1.0f;
		}
		else {
			progress = curMoveTimePass / curMoveTime + 0.01f;
		}
		curPos = VecLerp(curStartPoint, curTargetPoint, progress);

		if (parent != NULL) {
			vector3s parentCurPos = parent->GetTransform()->GetPos();
			curPos.z = parentCurPos.z;

			if (parentCurPos.x > curPos.x) {
				parent->SetRot(vector3s(180.0f, 0.0f, 0.0f));
			}
			else {
				parent->SetRot(vector3s(0.0f, 0.0f, 0.0f));
			}

			parent->SetPos(curPos, true);

			//set move anim
			Animation* anim = (Animation*)parent->GetComponent(E_Component::Component_Animation);
			if (anim != NULL) {
				anim->SwitchAnim(E_Anim::Anim_Move, false, false);
			}
		}

		if (progress >= 1) {
			moveDone = true;
		}
	}

}

void SimplePatrolAI::SwitchNextPoint(bool initState) {
	if (patrolType == E_PatrolType::PatrolType_EndInverse) {
		if (forward == true) {

			int arraySize = patrolPoint.size();

			if (initState == true) {
				curStartPointIndex = arraySize - 1;
			}

			if (arraySize > 1) {
				if (curStartPointIndex + 1 >= arraySize - 1) {
					forward = false;//inverse

					curStartPointIndex = arraySize - 1;

					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex - 1];
				}
				else {
					curStartPointIndex++;
					curStartPoint = curTargetPoint;
					curTargetPoint = patrolPoint[curStartPointIndex + 1];
				}
			}
			else if (arraySize == 1) {
				curPos = curStartPoint = curTargetPoint = patrolPoint[0];
			}
			else {
				return;
			}
		}
		else {

			int arraySize = patrolPoint.size();

			if (initState == true) {
				curStartPointIndex = 0;
			}

			if (arraySize > 1) {
				if (curStartPointIndex - 1 <= 0) {
					forward = true;//inverse

					curStartPointIndex = 0;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex + 1];
				}
				else {
					curStartPointIndex--;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex - 1];
				}
			}
			else if (arraySize == 1) {
				curPos = curStartPoint = curTargetPoint = patrolPoint[0];
			}
			else {
				return;
			}
		}
	}
	else if (patrolType == E_PatrolType::PatrolType_LoopCircle) {
		if (forward == true) {
			int arraySize = patrolPoint.size();

			if (initState == true) {
				curStartPointIndex = arraySize -1;
			}

			if (arraySize > 1) {

				if (curStartPointIndex >= arraySize - 1) {
					curStartPointIndex = 0;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex + 1];
				}
				else if (curStartPointIndex >= arraySize - 2) {

					curStartPointIndex = arraySize - 1;

					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[0];
				}

				else {
					curStartPointIndex++;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex + 1];
				}
			}
			else if (arraySize == 1) {
				curPos = curStartPoint = curTargetPoint = patrolPoint[0];
			}
			else {
				return;
			}
		}
		else {
			int arraySize = patrolPoint.size();

			if (initState == true) {
				curStartPointIndex = 0;
			}

			if (arraySize > 1) {
				if (curStartPointIndex - 1 <= 0) {

					curStartPointIndex = 0;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[arraySize - 1];

				}
				else if (curStartPointIndex <= 0) {
					curStartPointIndex = arraySize - 1;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex - 1];
				}
				else {
					curStartPointIndex--;
					curStartPoint = patrolPoint[curStartPointIndex];
					curTargetPoint = patrolPoint[curStartPointIndex - 1];
				}
			}
			else if (arraySize == 1) {
				curPos = curStartPoint = curTargetPoint = patrolPoint[0];
			}
			else {
				return;
			}
		}
	}
}

void SimplePatrolAI::AddPatrolPoint(vector3s point)
{
	patrolPoint.push_back(point);
}

void SimplePatrolAI::Reset() {
	forward = defaultForward;

	SwitchNextPoint(true);

	float dist = VecDist(curStartPoint, curTargetPoint);
	curMoveTime = dist / AI_NPC_MOVE_SPEED;
	curMoveTimePass = 0.0f;
	timePass = 0.0f;
	moveDone = false;
}


AIComponent::AIComponent()
{
	patrolAI = NULL;
	fsmSystem = new FSMSystem();//AI有限オートマトンをインスタンスする
}

AIComponent::~AIComponent()
{
	if (patrolAI != NULL) {
		delete patrolAI;
		patrolAI = NULL;
	}

	if (fsmSystem != NULL) {
		delete fsmSystem;
		fsmSystem = NULL;
	}
}

void AIComponent::DoInit()
{
	//if (patrolAI != NULL) {
	//	patrolAI->DoInit();
	//}
	fsmSystem->SetParent(this->parent);
	Reset();
}

void AIComponent::UnInit()
{
	if (patrolAI != NULL) {
		patrolAI->Reset();
	}
}

void AIComponent::DoUpdate(float deltatime)
{
	if (enable == true) {
		if (patrolAI != NULL) {
			patrolAI->DoUpdate(deltatime);
		}
		fsmSystem->DoUpdate(deltatime);
	}
	Component::DoUpdate(deltatime);
}

void AIComponent::SetAI(SimplePatrolAI* aiEntity)
{
	patrolAI = aiEntity;
}

SimplePatrolAI* AIComponent::GetAI()
{
	return patrolAI;
}

FSMSystem* AIComponent::GetFSMSystem()
{
	return fsmSystem;
}

void AIComponent::Reset()
{
	if (patrolAI != NULL) {
		patrolAI->DoInit();
	}

	if (fsmSystem != NULL) {
		fsmSystem->DoInit();
	}
}

#pragma endregion ai_method

#pragma endregion component_method

#pragma region movement_method

Movement::Movement() :
	parent(NULL), movementType(E_MovementType::MovementType_None), defaultForward(true), forward(true), defaultEnter(false),
	movementTime(0.0f), movementTime_I(0.0f), isLoop(false), moveTimePass(0.0f), movementState(false), movementDone(true), isEnterLight(false) {

}

Movement::Movement(BaseCell* p, E_MovementType mT, bool movementDefaultDir, bool loop, bool defaultActive, float movementT) :
	parent(p), movementType(mT), defaultForward(movementDefaultDir), forward(movementDefaultDir),
	movementTime(movementT), isLoop(loop), defaultEnter(defaultActive),moveTimePass(0.0f), movementState(true), movementDone(false), isEnterLight(false) {

	if (movementT <= 0) {
		movementTime_I = 1.0f;
	}
	else {
		movementTime_I = 1.0f / movementTime;
	}
}

Movement::~Movement() {
	parent = NULL;
}

void Movement::DoInit() {
	ResetDefault();
}

void Movement::StartMovement(bool reset) {

}

void Movement::SetMovementState(bool state) {
	if (movementState == state)return;

	movementState = state;//TODO
}

void Movement::StopMovement(bool reset) {
	movementDone = true;
}

void Movement::Reset() {
	SetMovementState(defaultEnter);
}

void Movement::ResetDefault()
{
	Reset();
}

void Movement::SetMovementTime(float time) {
	if ((int)(time + 0.5f) <= 0) {
		movementTime = 0.0f;
		movementTime_I = 1.0f;
	}
	else {
		movementTime = time;
		movementTime_I = 1.0f / movementTime_I;
	}
}

void Movement::SetParent(BaseCell* p) {
	parent = p;
}

bool Movement::CheckDefaultForward()
{
	return defaultForward;
}

bool Movement::IsMovementDone()
{
	return movementDone;
}


#pragma region movenet_axis_method

Movement_Axis::Movement_Axis() :isRelatedPos(true), startPos(vector3s(0, 0, 0)), endPos(vector3s(0, 0, 0)), curPos(vector3s(0, 0, 0)), defaultPos(vector3s(0, 0, 0)) {
	movementType = E_MovementType::MovementType_Axis;
}

Movement_Axis::Movement_Axis(BaseCell* p, bool isRelated, bool movementDefaultDir, bool loop, bool defaultActive, float movementT, vector3s defaultP, vector3s startP, vector3s endP) :
	isRelatedPos(isRelated), defaultPos(defaultP), startPos(startP), endPos(endP),
	Movement(p, E_MovementType::MovementType_Axis, movementDefaultDir, loop, defaultActive, movementT) {

	curPos = (movementDefaultDir) ? startPos : endPos;
}

Movement_Axis::~Movement_Axis()
{
}

void Movement_Axis::StartMovement(bool reset) {
	if (reset) {
		Reset();
	}
	movementState = true;
	movementDone = false;
}

void Movement_Axis::SetMovementState(bool state) {

	bool newState = state;

	movementDone = false;

	Movement::SetMovementState(newState);

}

void Movement_Axis::StopMovement(bool stopForward) {
	if (stopForward) {
		curPos = defaultForward ? endPos : startPos;
	}
	else {
		curPos = defaultForward ? startPos : endPos;
	}
}

void Movement_Axis::Reset() {
	if (defaultForward) {
		curPos = startPos;
	}
	else {
		curPos = endPos;
	}
	forward = defaultForward;
	moveTimePass = 0.0f;

	Movement::Reset();
}

void Movement_Axis::ResetDefult()
{
	Movement::ResetDefault();
	Reset();
}

void Movement_Axis::DoUpdate(float deltatime) {
	if (movementState == true && movementDone == false) {

		if (moveTimePass > movementTime) {
			if (isLoop) {
				forward = !forward;
				moveTimePass = 0.0f;
			}
			else {
				movementDone = true;
				//return
			}
		}
		else {
			moveTimePass += deltatime;
		}

		float progress = moveTimePass * movementTime_I;
		if (forward) {
			curPos = VecLerp(startPos, endPos, progress);
		}
		else {
			curPos = VecLerp(endPos, startPos, progress);
		}
	}
	ApplyMovement();

}

void Movement_Axis::DoLateUpdate(float deltatime) {

}

void Movement_Axis::ApplyMovement() {
	if (parent == NULL) {
		return;
	}

	//TODO
	if (isRelatedPos) {
		vector3s targetPos = defaultPos + curPos;
		parent->SetPos(targetPos, true);
	}
	else {
		parent->SetPos(curPos, true);
	}

}

void Movement_Axis::SetVecTParam(int index, vector3s param) {
	if (index == 0) {
		defaultPos = param;
	}
	//TODO
}

void Movement_Axis::ResetStartEndPos(vector3s start, vector3s end)
{
	startPos = start;
	endPos = end;
}

void Movement_Axis::SetDefaultPos(vector3s _pos)
{
	defaultPos = _pos;
}

void Movement_Axis::SetRelated(bool _isRelated)
{
	isRelatedPos = _isRelated;
}

void Movement_Axis::SetStartPos(vector3s _pos)
{
	startPos = _pos;
}

void Movement_Axis::SetEndPos(vector3s _pos)
{
	endPos = _pos;
}

#pragma endregion movenet_aixs_method




#pragma endregion movement_method

#pragma region movement_appear

Movement_Appear::Movement_Appear() :curCellState(false), waitTime(0.0f), waitTimePass(0.0f), forwardColor(vector4s()), backwardColor(vector4s()), curColor(vector4s())
{
	movementType = E_MovementType::MovementType_Appear;
}

Movement_Appear::Movement_Appear(BaseCell* p, bool defaultState, bool loop, bool defaultActive, float movementT, float waitT) :
	waitTime(waitT), waitTimePass(0.0f), curCellState(defaultState), forwardColor(vector4s()), backwardColor(vector4s()), curColor(vector4s()),
	Movement(p, E_MovementType::MovementType_Appear, defaultState, loop, defaultActive, movementT)
{
}

Movement_Appear::~Movement_Appear()
{

}

void Movement_Appear::StartMovement(bool reset)
{
	if (reset) {
		Reset();
	}
	movementState = true;
	movementDone = false;

	stateChange = false;
	curCellState = !defaultForward;
}

void Movement_Appear::SetMovementState(bool state)
{

	bool newState = state;
	movementDone = false;

	Movement::SetMovementState(newState);
}

void Movement_Appear::StopMovement(bool stopForward)
{
	if (stopForward) {
		//check default state
		curColor = forwardColor;
	}
	else {
		//check default state
		curColor = backwardColor;
	}
}

void Movement_Appear::Reset()
{

	forward = defaultForward;
	if (defaultForward) {
		curColor = backwardColor;
	}
	else {
		curColor = forwardColor;
	}

	moveTimePass = 0.0f;
	waitTimePass = 0.0f;

}

void Movement_Appear::ResetDefault()
{
	Movement::ResetDefault();
	Reset();
}

void Movement_Appear::DoUpdate(float deltatime)
{
	if (isLoop) {
		if (moveTimePass > movementTime) {
			if (stateChange == false) {
				stateChange = true;
				curCellState = forward;
			}
			if (movementState == true) {
				//counter wait time
				waitTimePass += deltatime;
				if (waitTimePass > waitTime) {
					forward = !forward;
					waitTimePass = 0.0f;
					moveTimePass = 0.0f;
					stateChange = false;
					//2022.11.24 Test
					ApplyMovement();
				}

			}

			//else:do nothing
		}
		else {
			moveTimePass += deltatime;
		}

		float progress = moveTimePass * movementTime_I;
		if (forward) {
			//curColor = VecLerp(backwardColor, forwardColor, progress);
		}
		else {
			//curColor = VecLerp(forwardColor, backwardColor, progress);
		}

	}
	else {

		if (movementState == true && movementDone == false) {

			if (moveTimePass > movementTime) {
				if (stateChange == false) {
					stateChange = true;
					curCellState = forward;
				}
				if (isLoop) {
					waitTimePass += deltatime;
					if (waitTimePass > waitTime) {
						forward = !forward;
						waitTimePass = 0.0f;
						moveTimePass = 0.0f;
						stateChange = false;
					}
					//else: do nothing
				}
				else {
					movementDone = true;
					movementState = false;
				}
			}
			else {
				moveTimePass += deltatime;
			}
			//TODO: Movement
			float progress = moveTimePass * movementTime_I;
			if (forward) {
				//curColor = VecLerp(backwardColor, forwardColor, progress);
				//curColor = progress >= 0.999f ? forwardColor : backwardColor;
			}
			else {
				//curColor = VecLerp(forwardColor, backwardColor, progress);
				//curColor = progress >= 0.999f ? backwardColor : forwardColor;
			}

		}
	}
	//ApplyMovement();
}

void Movement_Appear::DoLateUpdate(float deltatime)
{
}

void Movement_Appear::ApplyMovement()
{
	//TODO
	if (parent != NULL) {
		Collider* tempCollider = (Collider*)parent->GetComponent(E_Component::Component_Collider);
		if (tempCollider != NULL) {
			//2022.11.24 Test
			DrawData* d = parent->GetDrawData();
			if (tempCollider->CheckComponentState())
			{
				tempCollider->SetComponentState(false);
				d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
			}
			else
			{
				tempCollider->SetComponentState(true);
				d->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}
}

void Movement_Appear::SetMovementColor(vector4s fColor, vector4s bColor)
{
	forwardColor = fColor;
	backwardColor = bColor;
	curColor = !forward ? forwardColor : backwardColor;
}

void Movement_Appear::SetVecTParam(int index, vector3s param)
{

}

#pragma endregion movement_appear

#pragma region movement_breakable

Movement_Breakable::Movement_Breakable() :targetState(false),curState(true)
{
	movementType = E_MovementType::MovementType_Appear;
}

Movement_Breakable::Movement_Breakable(BaseCell* p, bool defaultState, bool loop, bool defaultActive, float movementT, float waitT) :
	targetState(!defaultState),curState(defaultState),
	Movement(p, E_MovementType::MovementType_Appear, defaultState, loop, defaultActive, movementT)
{
}

Movement_Breakable::~Movement_Breakable()
{

}

void Movement_Breakable::StartMovement(bool reset)
{
	if (reset) {
		Reset();
	}
	movementState = true;
	movementDone = false;

}

void Movement_Breakable::SetMovementState(bool state)
{
	bool newState = state;
	movementDone = false;

	Movement::SetMovementState(newState);
}

void Movement_Breakable::StopMovement(bool stopForward)
{
	if (stopForward) {
		//check default state
	}
	else {
		//check default state
	}
}

void Movement_Breakable::Reset()
{
	curState = !targetState;

	moveTimePass = 0.0f;

	SetMovementState(defaultEnter);
}

void Movement_Breakable::ResetDefault()
{
	Movement::ResetDefault();
	Reset();
}

void Movement_Breakable::DoUpdate(float deltatime)
{

		if (movementState == true && movementDone == false) {

			if (moveTimePass > movementTime) {
				movementDone = true;
				curState = targetState;
				GameMgr::GetInstance()->effectMgr->SetCameraShake(0.15f, 11.0f);
				ApplyMovement();
			}
			else {
				moveTimePass += deltatime;
			}
		}
	//}
}

void Movement_Breakable::DoLateUpdate(float deltatime)
{
}

void Movement_Breakable::ApplyMovement()
{
	if (parent != NULL) {
		parent->SetState(curState);
	}
}

void Movement_Breakable::SetMovementColor(vector4s fColor, vector4s bColor)
{

}

void Movement_Breakable::SetVecTParam(int index, vector3s param)
{

}

#pragma endregion movement_breakable

