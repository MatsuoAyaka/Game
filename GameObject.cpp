#pragma once
#include "GameObject.h"

inline bool AdjustScreenPoint(vector3s& pos, vector2s size);

#pragma region basecell

#pragma region baseCell_method
BaseCell::BaseCell() :cellId(-1), enable(true),drawLayer(E_DrawLayer::DrawLayer_Scene_2)
{
	componentDic.clear();

	cellType = E_CellType::CellType_None;

	Transform* transformComponent = new Transform();//必ずトランスフォーマーコンポーネントを持っている（位置や回転をコントロールする）
	transform = transformComponent;

	movementDic.clear();
	if (RegisterComponent(E_Component::Component_Transform, transformComponent) == false && transformComponent != NULL) {
		delete transformComponent;
	}

	defaultColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	affectColorChangeSign = false;
	drawData = NULL;

	objCellType = E_ObjCellType::ObjCellType_None;

	m_parent = NULL;
	m_childArray.clear();
}

BaseCell::~BaseCell()
{
	//コンポーネントを削除する
	map<E_Component, Component*>::iterator iter;
	for (iter = componentDic.begin(); iter != componentDic.end(); iter++) {
		if (iter->second != NULL) {
			Component* temp = iter->second;
			iter->second = NULL;
			delete temp;
		}
	}

	transform = NULL;

	//delete all movement
	map<E_MovementType, Movement*>::iterator iter_movement;
	for (iter_movement = movementDic.begin(); iter_movement != movementDic.end(); iter_movement++) {
		if (iter_movement->second != NULL) {
			delete iter_movement->second;
		}
	}

	componentDic.clear();
	movementDic.clear();

	//描画データを削除する
	if (drawData != NULL) {
		delete drawData;
		drawData = NULL;
	}

	m_parent = NULL;
	m_childArray.clear();
}

void BaseCell::DoInit() {
	//コンポーネントを初期化する
	map<E_Component, Component*>::iterator iter;
	for (iter = componentDic.begin(); iter != componentDic.end(); iter++) {
		if (iter->second != NULL && iter->second->CheckComponentState()) {
			iter->second->DoInit();
		}
	}

	//init movement
	map<E_MovementType, Movement*>::iterator iter_movement;
	for (iter_movement = movementDic.begin(); iter_movement != movementDic.end(); iter_movement++) {
		if (iter_movement->second != NULL) {
			iter_movement->second->DoInit();
		}
	}

}

void BaseCell::UnInit() {

}

/// <summary>
/// 描画順序を設定する
/// </summary>
/// <param name="layer"></param>
void BaseCell::SetDrawLayer(E_DrawLayer layer) {
	drawLayer = layer;
}
/// <summary>
/// 描画順序を取得する
/// </summary>
/// <returns></returns>
E_DrawLayer BaseCell::GetDrawLayer()
{
	return drawLayer;
}

/// <summary>
/// トランスフォーマーコンポーネントを取得する
/// </summary>
/// <returns></returns>
Transform* BaseCell::GetTransform()
{
	return transform;
}

/// <summary>
/// コンポーネントを追加する
/// </summary>
/// <param name="componentType">　コンポーネント種類　</param>
/// <param name="componentEntity">　コンポーネントのインスタンス　</param>
/// <param name="replace">　もし同じ種類のコンポーネントがもう存在してるなら、新しいインスタンスに切り替えるかどうか</param>
/// <returns></returns>
bool BaseCell::RegisterComponent(E_Component componentType, Component* componentEntity, bool replace)
{

	if (componentEntity == NULL) {
		//cout<<"error\n";
		return false;
	}

	map<E_Component, Component*>::iterator iter = componentDic.find(componentType);//リストの中にもう同じ種類のコンポーネントが存在してるかのをチェック
	if (iter != componentDic.end()) {//同じもの存在してる
		
		if (replace) {
			componentDic[componentType] = componentEntity;//新しいものに切り替える
		}
		else {
			return false;
		}
	}
	else {//同じものが存在してない
		componentDic[componentType] = componentEntity;//リストに追加する
	}

	componentEntity->SetParent(this);//コンポーネントの所有者を設定する（今このゲームオブジェクトのポインタ）

	return true;

}

/// <summary>
/// 位置を設定する
/// </summary>
/// <param name="pos"></param>
void BaseCell::SetPos(vector3s pos, bool updatePrePos = true)
{
	//Transform* transform = (Transform*) GetComponent(E_Component::Component_Transform);
	if (transform == NULL) {
		//cout<<"error\n";
		return;
	}

	transform->SetPos(pos, updatePrePos);
}
/// <summary>
/// 回転角度を設定する
/// </summary>
/// <param name="rot"></param>
void BaseCell::SetRot(vector3s rot)
{
	//Transform* transform = (Transform*)GetComponent(E_Component::Component_Transform);
	if (transform == NULL) {
		//cout<<"error\n";
		return;
	}

	transform->SetRot(rot);
}
/// <summary>
/// スケールを設定する
/// </summary>
/// <param name="scale"></param>
void BaseCell::SetScale(vector3s scale)
{
	//Transform* transform = (Transform*)GetComponent(E_Component::Component_Transform);
	if (transform == NULL) {
		//cout<<"error\n";
		return;
	}

	transform->SetScale(scale);
}

/// <summary>
/// 位置や回転角度やスケールを共に設定する（他のトランスフォーマーインスタンスからコピペする
/// </summary>
/// <param name="t"></param>
void BaseCell::SetTransform(const Transform& t)
{
	if (transform != NULL) {
		transform->SetPos(t.GetPos(), true);
		transform->SetRot(t.GetRot());
		transform->SetScale(t.GetScale());
	}
}

void BaseCell::ResetPos(bool x, bool y)
{
	if (transform == NULL)return;
}

/// <summary>
/// サイズを取得する
/// </summary>
/// <param name="withScale">そのサイズはスケールによって変えられてるかどうか</param>
/// <returns>　サイズ　</returns>
vector2s BaseCell::GetSize(bool withScale = true) const
{
	if (drawData == NULL) {//描画データが存在してない
		if (withScale) {
			return transform == NULL ? vector2s(0, 0) : vector2s(transform->GetScale().x, transform->GetScale().y);
		}
		else {
			return vector2s(0, 0);
		}
	}
	else {//描画データが存在してる
		if (withScale) {//スケールによるサイズ
			return transform == NULL ? drawData->GetSize() : drawData->GetSize() * vector2s(transform->GetScale().x, transform->GetScale().y);
		}
		else {
			return drawData->GetSize();//（スケールを無視）元々のサイズを戻り値にする
		}
	}
}

/// <summary>
/// コライダーボックスのサイズを取得する
/// </summary>
/// <param name="withScale">　そのサイズはスケールによって変えられてるかどうか　</param>
/// <param name="colliderDataIndex">　コライダー配列の添え字（一つのコライダーしかないときは０）</param>
/// <returns>　サイズ　</returns>
vector2s BaseCell::GetColliderSize(bool withScale,int colliderDataIndex) 
{
	Collider* collider = (Collider*)GetComponent(E_Component::Component_Collider);
	if (collider == NULL)return vector2s(0,0);
	if (collider->GetColliderDataArraySize() == 0 || collider->GetColliderDataArraySize() <=colliderDataIndex || collider->GetColliderDataArray()[colliderDataIndex] == NULL)return vector2s(0,0);

	vector2s size = collider->GetColliderDataArray()[colliderDataIndex]->size;
	if (withScale == true) {
		vector3s scale = this->GetTransform()->GetScale();
		size.x *= scale.x;
		size.y *= scale.y;
	}
	return size;
}

vector3s BaseCell::GetColliderPos(bool _withScale, int colliderDataIndex)
{
	Collider* collider = (Collider*)GetComponent(E_Component::Component_Collider);
	if (collider == NULL)return this->GetTransform()->GetPos();
	if (collider->GetColliderDataArraySize() == 0 || collider->GetColliderDataArraySize() <= colliderDataIndex || collider->GetColliderDataArray()[colliderDataIndex] == NULL)return this->GetTransform()->GetPos();

	vector3s pos = this->GetTransform()->GetPos();
	vector3s offset = collider->GetColliderDataArray()[colliderDataIndex]->offset;
	if (_withScale == true) {
		vector3s scale = this->GetTransform()->GetScale();
		offset.x *= scale.x;
		offset.y *= scale.y;
		offset.z *= scale.z;
	}
	pos = pos + offset;
	return pos;
}

void BaseCell::DoUpdate(float deltatime)
{
	if (enable == false)return;

	//コンポーネントのアップデート
	map<E_Component, Component*>::iterator iter;
	for (iter = componentDic.begin(); iter != componentDic.end(); iter++) {
		if (iter->second == NULL || iter->second->CheckComponentState() == false) {
			continue;
		}

		iter->second->DoUpdate(deltatime);
	}

	//movement update
	map<E_MovementType, Movement*>::iterator iter_movement;
	for (iter_movement = movementDic.begin(); iter_movement != movementDic.end(); iter_movement++) {
		if (iter_movement->second == NULL)continue;
		iter_movement->second->DoUpdate(deltatime);
	}

}

void BaseCell::DoLateUpdate(float deltatime)
{
	map<E_Component, Component*>::iterator iter;
	for (iter = componentDic.begin(); iter != componentDic.end(); iter++) {
		if (iter->second == NULL || iter->second->CheckComponentState() == false) {
			continue;
		}

		iter->second->DoLastUpdate(deltatime);
	}

	//movement update
	map<E_MovementType, Movement*>::iterator iter_movement;
	for (iter_movement = movementDic.begin(); iter_movement != movementDic.end(); iter_movement++) {
		if (iter_movement->second == NULL)continue;
		//iter_movement->second->DoLateUpdate(deltatime);
	}
}

void BaseCell::SetState(bool state) {
	enable = state;
	if (m_childArray.empty() == false) {
		for (auto cur : m_childArray) {
			if (cur == NULL)continue;
			cur->SetState(state);
		}
	}
}

void BaseCell::SwitchAnim(E_Anim _anim, bool _resetAnim, bool _lastClipEndCB){
	Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
	if (anim != NULL) {
		anim->SwitchAnim(_anim, _resetAnim, _lastClipEndCB);
	}
}

void BaseCell::SetDrawDataSize(vector2s _size)
{
	if (drawData == NULL)return;
	drawData->SetSize(_size);
}

bool BaseCell::CheckInScreenRect(BaseCell* cell, bool checkWholeCell)
{
	if (cell == NULL)return false;

	if (checkWholeCell == false) {
		return Collider::IsRectContainPoint(vector3s(0, 0, 0), vector2s(SCREEN_WIDTH, SCREEN_HEIGHT), vector3s(0, 0, 0), cell->GetTransform()->GetPos());
	}
	else {
		//TODO
		return false;
	}

}

void BaseCell::SetParent(BaseCell* _parent)
{
	if (m_parent != NULL) {
		m_parent->RemoveChild(this);
		m_parent = NULL;
	}

	m_parent = _parent;
	if (_parent != NULL) {
		_parent->AddChild(this);
	}
}

void BaseCell::AddChild(BaseCell* _child)
{
	if (_child == NULL)return;

	vector<BaseCell*>::iterator iter;
	for (iter = m_childArray.begin(); iter != m_childArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		if ((*iter)->GetID() == _child->GetID()) {
			return;
		}
	}

	m_childArray.push_back(_child);
}

void BaseCell::RemoveChild(BaseCell* _child)
{
	if (_child == NULL || m_childArray.empty()==true)return;

	vector<BaseCell*>::iterator iter;
	for (iter = m_childArray.begin(); iter != m_childArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		if ((*iter)->GetID() == _child->GetID()) {
			m_childArray.erase(iter);
			break;
		}
	}
}

void BaseCell::RecordCurState()
{
	//transform
	if (transform != NULL) {
		transform->RecordCurState();//初期の位置や回転角度やスケールを保存する
	}
	//movement
	if (drawData != NULL) {
		defaultColor = drawData->GetColor();
	}
}

void BaseCell::Reset()
{
	//transform
	if (transform != NULL) {
		transform->Reset();//初期の状態に戻す
	}
	//movement
	if (movementDic.size() != 0) {
		map<E_MovementType, Movement*>::iterator iter;
		for (iter = movementDic.begin(); iter != movementDic.end(); iter++) {
			if (iter->second == NULL)continue;
			//iter->second->ResetDefault();
		}
	}

	if (drawData != NULL) {
		drawData->SetColor(defaultColor);
	}
}

void BaseCell::Recycle()
{
	GameMgr::GetInstance()->sceneObjMgr->RecycleCell(this);
}

void BaseCell::OnCellInit()
{
	this->SetState(true);
}

void BaseCell::OnCellRecycle()
{
	m_parent = NULL;
	m_childArray.clear();
	this->SetState(false);
}

void BaseCell::DeleteCell()
{
	//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(this);
	GameMgr::GetInstance()->sceneObjMgr->DeleteCell(this);
}

void BaseCell::UpdateMovementData()
{
	for (auto curMovement : movementDic) {
		if (curMovement.second != NULL && curMovement.second->GetMovementType() == E_MovementType::MovementType_Axis) {
			((Movement_Axis*)curMovement.second)->SetDefaultPos(this->transform->GetPos());
		}
	}
}

void BaseCell::AddMovement(Movement* movement)
{
	if (movement == NULL)return;

	map<E_MovementType, Movement*>::iterator iter = movementDic.find(movement->GetMovementType());
	if (iter == movementDic.end()) {
		movementDic[movement->GetMovementType()] = movement;
	}
	else {
		//error
		return;
	}
}

void BaseCell::RemoveMovement(E_MovementType movementType) {
	if (movementDic.size() == 0)return;

	map<E_MovementType, Movement*>::iterator iter = movementDic.find(movementType);
	if (iter != movementDic.end()) {
		Movement* tempMovement = iter->second;
		movementDic.erase(iter);

		if (tempMovement != NULL) {
			delete tempMovement;
		}
	}
}

void BaseCell::StartMovement(E_MovementType targetMovement,bool reset,bool allMovement) {
	if (movementDic.size() == 0)return;
	if (allMovement == true) {
		map<E_MovementType, Movement*>::iterator iter;
		for (iter = movementDic.begin(); iter != movementDic.end(); iter++) {
			if (iter->second == NULL)continue;

			iter->second->StartMovement(reset);
		}
	}
	else {
		if (movementDic.find(targetMovement) != movementDic.end() && movementDic[targetMovement] != NULL) {
			movementDic[targetMovement]->StartMovement(reset);
		}
	}
}

void BaseCell::StopMovement(E_MovementType targetMovement,bool stopForward,bool allMovement) {
	if (movementDic.size() == 0)return;
	if (allMovement == true) {
		map<E_MovementType, Movement*>::iterator iter;
		for (iter = movementDic.begin(); iter != movementDic.end(); iter++) {
			if (iter->second == NULL)continue;

			iter->second->StopMovement(stopForward);
		}
	}
	else {
		if (movementDic.find(targetMovement) != movementDic.end() && movementDic[targetMovement] != NULL) {
			movementDic[targetMovement]->StopMovement(stopForward);
		}
	}
}

void BaseCell::SetMovementState(E_MovementType targetMovement,bool state,bool allMovement) {
	if (movementDic.size() == 0)return;
	if (allMovement == true) {
		map<E_MovementType, Movement*>::iterator iter;
		for (iter = movementDic.begin(); iter != movementDic.end(); iter++) {
			if (iter->second == NULL)continue;

			iter->second->SetMovementState(state);
		}
	}
	else {
		if (movementDic.find(targetMovement) != movementDic.end() && movementDic[targetMovement] != NULL) {
			movementDic[targetMovement]->SetMovementState(state);
		}
	}
}

map<E_MovementType, Movement*>& BaseCell::GetMovementDic()
{
	return movementDic;
}

bool BaseCell::CheckCurMovementDoneState(E_MovementType movementType)
{
	if (movementDic.size() == 0 || movementDic.find(movementType)==movementDic.end()||movementDic[movementType]==NULL)return true;
	return movementDic[movementType]->IsMovementDone();
}

#pragma endregion baseCell_method
#pragma endregion basecell

//#pragma region player
//
//Player::Player()
//{
//	cellType = E_CellType::CellType_Player;
//}
//
//Player::~Player()
//{
//}
//
//void Player::DoUpdate(float deltatime) {
//
//	if (enable == true);
//	
//	BaseCell::DoUpdate(deltatime);
//}
//
//void Player::Input(float deltatime) {
//	if (enable == false)return;
//
//	if (GetKeyboardTrigger(DIK_L)) {
//		inputLock = !inputLock;
//	}
//
//	if (inputLock == true) {
//		return;
//	}
//
//	vector2s movement = vector2s(0, 0);//移動量
//	vector3s tempPos = this->GetTransform()->GetPos();
//	bool playerMovementSign = false;
//
//	BaseCell* tempCell = NULL;
//
//	Bullet* bullet = (Bullet*)GameMgr::GetInstance()->sceneObjMgr->GetCellWithType(E_CellType::CellType_Bullet);//戻り値はBaseCell＊タイプなので、Bullet＊に転換する必要があります
//
//	if (GetKeyboardPress(DIK_W)) {
//		movement.y -= PLAYER_MOVEMENT_SPEED * deltatime;//ｙ軸方向の移動量
//
//		//実際にプレイヤーを移動する前に、まず一時的なオブジェクト（tempCell)を使って、プレイヤーオブジェクトの代わりに当たり判定を行う
//		//移動できる場合は、プレイヤーの位置を変える(SetPos())
//		if (tempCell != NULL) {
//			tempPos.y += movement.y;
//			//プレイや‐オブジェクトのコライダーボックスのパラメータをtempCellに渡す
//			//														移動先			回転角度						スケール					コライダーボックスのサイズ
//			GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true,0),0);
//			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {//移動できない場合は:移動先に移動できないだとしても、ギリギリぶつかる前の位置に移動できるので、その位置に移動させる
//				movement.y = 0;
//				this->SetPos(tempCell->GetTransform()->GetPos(), true);//ギリギリぶつかる前の位置に移動する
//			}
//			else {//移動できる場合
//				playerMovementSign = true;//移動フラグ
//			}
//		}
//		else {
//			playerMovementSign = true;
//		}
//	}
//	else if (GetKeyboardPress(DIK_S)) {
//		movement.y += PLAYER_MOVEMENT_SPEED * deltatime;
//
//		if (tempCell != NULL) {
//			tempPos.y += movement.y;
//			GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true,0),0);
//			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
//				movement.y = 0;
//				this->SetPos(tempCell->GetTransform()->GetPos(), true);
//			}
//			else {
//				playerMovementSign = true;
//			}
//		}
//		else {
//			playerMovementSign = true;
//		}
//	}
//	if (GetKeyboardPress(DIK_A)) {
//		movement.x -= PLAYER_MOVEMENT_SPEED * deltatime;
//
//		if (tempCell != NULL) {
//			tempPos.x += movement.x;
//			GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true,0),0);
//			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
//				movement.x = 0;
//				this->SetPos(tempCell->GetTransform()->GetPos(), true);
//			}
//			else {
//				playerMovementSign = true;
//			}
//		}
//		else {
//			playerMovementSign = true;
//		}
//	}
//	else if (GetKeyboardPress(DIK_D)) {
//		movement.x += PLAYER_MOVEMENT_SPEED * deltatime;
//
//		if (tempCell != NULL) {
//			tempPos.x += movement.x;
//			GameMgr::GetInstance()->sceneObjMgr->SetTempCellParam(tempPos, this->GetTransform()->GetRot(), this->GetTransform()->GetScale(), this->GetColliderSize(true,0),0);
//			if (GameMgr::GetInstance()->sceneObjMgr->CheckTempCollision() == true) {
//				movement.x = 0;
//				this->SetPos(tempCell->GetTransform()->GetPos(), true);
//			}
//			else {
//				playerMovementSign = true;
//			}
//		}
//		else {
//			playerMovementSign = true;
//		}
//	}
//	//ｘ移動量によってプレイヤーが向かいている方向を決める（左右）
//	if (movement.x > 0) {
//		this->transform->SetRot(vector3s(0.0f, 0.0f, 0.0f));
//	}
//	else if (movement.x < 0) {
//		this->transform->SetRot(vector3s(-180.0f, 0.0f, 0.0f));
//	}
//
//	//anim
//	Animation* anim = (Animation*)GetComponent(E_Component::Component_Animation);
//	if (anim != NULL) {
//		if (playerMovementSign) {//移動している場合は、moveアニメーションに切り替える
//			anim->SwitchAnim(E_Anim::Anim_Move, false, true);
//		}
//		else {//移動していない場合は、idleアニメーションに切り替える
//			anim->SwitchAnim(E_Anim::Anim_Idle, false, true);
//		}
//	}
//
//	movement = VecNormalize(movement) * PALYER_MAX_MOVE_SPEED;
//
//	vector3s curPos = this->GetTransform()->GetPos();
//	curPos.x += movement.x;
//	curPos.y += movement.y;
//
//	//当たり判定
//	//画面の四角形との当たり判定；画面の外に出られないように
//	AdjustScreenPoint(curPos, this->GetSize(true));
//	this->SetPos(curPos, true);
//
//	GameMgr::GetInstance()->sceneObjMgr->CheckPlayerCollision(this);//他との当たり判定（敵とか
//
//}
//
//void Player::SetState(bool state)
//{
//	if (state == false) {
//	}
//	else {
//	}
//	BaseCell::SetState(state);
//}
//
//
//#pragma endregion player

#pragma region obstacle

Obstacle::Obstacle() :obstacleType(E_Obstacle::Obstacle_Const)
{
	cellType = E_CellType::CellType_Obstacle;
}

Obstacle::~Obstacle()
{

}

void Obstacle::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);
}

void Obstacle::SetObstacleType(E_Obstacle obstacleT)
{
	obstacleType = obstacleT;
}

E_Obstacle Obstacle::GetObstacleType()
{
	return obstacleType;
}

void Obstacle::Recycle()
{
	BaseCell::Recycle();
}

void Obstacle::OnCellInit()
{
	BaseCell::OnCellInit();
}

void Obstacle::OnCellRecycle()
{
	BaseCell::OnCellRecycle();
}

void Obstacle::DeleteCell()
{
	BaseCell::DeleteCell();
}

void Obstacle::SetParent(BaseCell* _parent)
{
	BaseCell::SetParent(_parent);
}

void Obstacle::Reset()
{
	BaseCell::Reset();
}


#pragma endregion obstacle

#pragma region enemy

#pragma endregion enemy

//void Bullet::PlayerTest(Player* player)
//{
//}
