#include "GameLogic.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"
#include "Enemy.h"
#include "MapGenerator.h"
#include "EventCenter.h"
#include "Effect.h"

#define BAR_SCALE_X  (3)


#pragma region input_manager

InputMgr::InputMgr() :curInputMode(E_InputMode::InputMode_None) {

}

InputMgr::~InputMgr() {

}

void InputMgr::DoUpdate(float deltatime) {
	if (inputState == true) {

		////追加松尾================================================================
		//if (curInputMode == E_InputMode::InputMode_Logo) {//タイトルでの入力
		//	if (GetKeyboardTrigger(DIK_SPACE) == true) {
		//		GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Title, true);
		//		
		//	}
		//}
		//=========================================================================
		if (curInputMode == E_InputMode::InputMode_Title) {//タイトルでの入力
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START)) {
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal,true);
			}
		}
		else if (curInputMode == E_InputMode::InputMode_Game) {//ゲーム入力
			if (GameMgr::GetInstance()->sceneObjMgr->GetPlayer() != NULL) {
				GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->Input(deltatime);//プレイヤーキャラクターの入力処理
			}
			//MainCamera::GetInstance()->MoveTest();
			MainCamera::GetInstance()->ZoomTest();
		}
		else if (curInputMode == E_InputMode::InputMode_Game_Menu) {//ゲームメニューでの入力

		}
		else if (curInputMode == E_InputMode::InputMode_Game_Lose_Menu) {
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START))
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal, true);
		}
		else if (curInputMode == E_InputMode::InputMode_Game_Win_Menu) {
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START)) {
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal, true);
			}
		}
	}
}

/// <summary>
/// 入力モードを切り替える
/// </summary>
/// <param name="targetInputMode"></param>
void InputMgr::SwitchInputMode(E_InputMode targetInputMode) {
	curInputMode = targetInputMode;
}

/// <summary>
/// 入力状態を設定する
/// </summary>
/// <param name="state"></param>
void InputMgr::SetInputState(bool state) {
	inputState = state;
}

/// <summary>
/// 今の入力モードを取得する
/// </summary>
/// <returns></returns>
E_InputMode InputMgr::GetCurInputMode()
{
	return curInputMode;
}

#pragma endregion input_manager

#pragma region SceneObjMgr_method

SceneObjMgr::SceneObjMgr() :cellIdCounter(10){

	m_objectPoolMgr = NULL;
	drawCellDic.clear();
	cellDic.clear();
	cellTypeDic.clear();

	m_player = NULL;

	tempCell = NULL;
}

SceneObjMgr::~SceneObjMgr()
{
	drawCellDic.clear();
	cellDic.clear();
	cellTypeDic.clear();
	drawCellDic.clear();
	m_player = NULL;

	if (tempCell != NULL) {
		delete tempCell;
		tempCell = NULL;
	}
	if (m_objectPoolMgr != NULL) {
		delete m_objectPoolMgr;
	}
}

void SceneObjMgr::DoInit() {

	cellIdCounter = 10;
	cellDic.clear();

	m_objectPoolMgr = new ObjectPoolMgr();
	m_objectPoolMgr->DoInit();

	//プレイヤーキャラクターをインスタンス化
	Player* player = new Player();
	player->SetID(this->GetID());//IDを設定する
	player->SetPos(vector3s(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.3f, 0.0f), true);//位置を設定する
	//描画データを作る
	DrawData* drawData = new DrawData();
	drawData->shakeWithCamera = true;//カメラ揺れの時、ゲーム画面と一緒に揺れる
	drawData->SetSize(const_size_player);//描画サイズ
	drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\playerAniSheet.png"));//テクスチャ―
	drawData->texIndex = vector2s(TEX_PLAYER_ANIM_IDLE_START_INDEX_X, TEX_PLAYER_ANIM_IDLE_START_INDEX_Y);//最初のテクスチャ―UV座標
	drawData->tex_w_count = TEX_PLAYER_ANIM_CELL_W_COUNT;//テクスチャ―の横の図案の数
	drawData->tex_texel_size = character_cell_tex_size;//テクスチャ―の図案のサイズ　
	//drawData->usePreComputeUV = false;
	//drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//drawData->preComputeUVSize = vector2s(1.0f,1.0f);
	//drawData->tex_texel_size = vector2s(1.0f,1.0f);//テクスチャ―の図案のサイズ　
	player->SetDrawData(drawData);//描画データをプレイヤーゲームオブジェクトに渡す

	//プレイヤーアニメーションコンポーネントを作る
	Animation* playerAnimComponent = new Animation();//インスタンス化
	if (player->RegisterComponent(E_Component::Component_Animation, playerAnimComponent) == false) {//アニメーションコンポーネントを追加する
		delete playerAnimComponent;
		playerAnimComponent = NULL;
	}
	else {
		
		//アイドルアニメーションを作る
		AnimClip* playerIdleAnim = new AnimClip();
		int startTexIndex = TEX_PLAYER_ANIM_IDLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_IDLE_START_INDEX_X;//アニメーションのスタートUV座標
		int endTexIndex = startTexIndex + TEX_PLAYER_ANIM_IDLE_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerIdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);//アニメーションのパラメータを設定する
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Idle, playerIdleAnim, true);//アニメーションコンポーネントに新しいアニメーションを追加する

		//移動アニメーションを作る
		AnimClip* playerMoveAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_MOVE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_MOVE_START_INDEX_X; // アニメーションのスタートUV座標
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_MOVE_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerMoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Move, playerMoveAnim, false);//アニメーションコンポーネントに新しいアニメーションを追加する
		
		//滑りアニメーションを作る
		AnimClip* playerSlideAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_SLIDE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_SLIDE_START_INDEX_X; // アニメーションのスタートUV座標
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_SLIDE_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerSlideAnim->SetAnimParam(E_Anim::Anim_Slide, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Slide, playerSlideAnim, false);//アニメーションコンポーネントに新しいアニメーションを追加する

		//転ぶアニメーションを作る
		AnimClip* playerTumbleAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_TUMBLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_TUMBLE_START_INDEX_X; // アニメーションのスタートUV座標
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_TUMBLE_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerTumbleAnim->SetAnimParam(E_Anim::Anim_Tumble, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Tumble, playerTumbleAnim, false);//アニメーションコンポーネントに新しいアニメーションを追加する
	
		//攻撃受けアニメーションを作る
		AnimClip* playerHurtAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_HURT_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_HURT_START_INDEX_X; // アニメーションのスタートUV座標
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_HURT_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerHurtAnim->SetAnimParam(E_Anim::Anim_Hurt, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Hurt, playerHurtAnim, false);//アニメーションコンポーネントに新しいアニメーションを追加する
	
		//勝利アニメーションを作る
		AnimClip* playerWinAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_WIN_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_WIN_START_INDEX_X; // アニメーションのスタートUV座標
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_WIN_FRAME_COUNT;//アニメーションのエンドUV座標
		//								種類				UV座標							持続時間、方向、繰り返しフラグ
		playerWinAnim->SetAnimParam(E_Anim::Anim_Win, vector2s(startTexIndex, endTexIndex), 1.5f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Win, playerWinAnim, false);//アニメーションコンポーネントに新しいアニメーションを追加する
		playerWinAnim->animEnd_Callback = new Anim_Player_Win_CB();

	}
	vector2s size = player->GetSize(true);
	//size.x *= 0.92f;
	//size.y *= 1.0f;
	//プレイヤーコライダーコンポーネントを作る
	Collider* playerColliderComponent = new Collider();//インスタンス化
	player->RegisterComponent(E_Component::Component_Collider, playerColliderComponent);//コライダーコンポーネントを追加する
		//コライダーデータを作る
	ColliderData* playerCollider_1 = new ColliderData(1, playerColliderComponent, true,
		E_ColliderType::Collider_Circle, size, vector3s(0, 0, 0));

	playerColliderComponent->AddColliderData(playerCollider_1);//コライダーコンポーネントにコライダーデータを追加する

	RegisterCell(player,true);//新しいゲームオブジェクトをSceneObjMgrに保存してもらう
	m_player = player;

	player->DoInit();
	player->SetState(false);

	//一時的なゲームオブジェクトを作る（当たり判定用なので、コライダーコンポーネントしか持っていない）
	//描画やアップデート処理などの必要がないので、リストに追加することは不要
	tempCell = new BaseCell();
	Collider* collider = new Collider();
	ColliderData* collider_data = new ColliderData(0, collider, false, E_ColliderType::Collider_Rect, vector2s(0, 0), vector3s(0, 0, 0));
	collider->AddColliderData(collider_data);
	tempCell->RegisterComponent(E_Component::Component_Collider, collider);

	m_nezumi = new Nezumi();
	RegisterCell(m_nezumi, true);//新しいゲームオブジェクトをSceneObjMgrに保存してもらう
	m_nezumi->DoInit();
	//m_nezumi->SetState(false);


}

void SceneObjMgr::UnInit(bool deleteCell) {
	//すべてのゲームオブジェクトを削除する
	if (deleteCell) {
		map<int, BaseCell*>::iterator iter;
		for (iter = cellDic.begin(); iter != cellDic.end(); iter++) {
			if (iter->second == NULL)continue;
			BaseCell* temp = iter->second;
			iter->second = NULL;
			delete temp;
		}

		cellDic.clear();
		cellIdCounter = 10;

		drawCellDic.clear();
	}
}

/// <summary>
/// すべてのゲームオブジェクトを初期化する（ゲームシーンをロードするときに呼び出される
/// </summary>
void SceneObjMgr::DoInitCell() {
	for (auto cell : cellDic) {
		if (cell.second == NULL || cell.second->CheckState() == false) {
			continue;
		}

		cell.second->DoInit();
	}
}

/// <summary>
/// すべてのゲームオブジェクトのアップデート処理
/// </summary>
/// <param name="deltatime"></param>
void SceneObjMgr::DoUpdate(float deltatime) {
	int size = cellDic.size();
	for (auto cell : cellDic) {
		if (cell.second == NULL || cell.second->CheckState() == false) {
			continue;
		}

		cell.second->DoUpdate(deltatime);

	}
	//23.1
	DeleteCell();
}

void SceneObjMgr::DoLateUpdate(float deltatime) {

}

/// <summary>
/// すべてのゲームオブジェクトの描画処理
/// </summary>
void SceneObjMgr::DoDraw()
{
	//描画順序によって描画する
	//bottom	‐　一番後ろ
	//scene_3
	//scene_2
	//scene_1
	//ui
	//top		‐	一番前
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Bottom) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Bottom];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_3) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_3];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_2) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_2];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_1) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_1];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_UI) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_UI];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Top) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Top];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
}

/// <summary>
/// 新しいゲームオブジェクトを配列の中に保存する
/// </summary>
/// <param name="id">　ゲームオブジェクトのID</param>
/// <param name="cell">　ゲームオブジェクトのインスタンス</param>
/// <param name="recordDefaultState">ゲームオブジェクトの初期の状態を保存するかどうか</param>
void SceneObjMgr::RegisterCell(int id, BaseCell* cell, bool recordDefaultState) {
	if (cell == NULL)return;

	if (recordDefaultState == true) {
		cell->RecordCurState();//初期の状態を保存する
	}
	//ゲームオブジェクトのIDを添え字としてリストに追加する
	if (cellDic.find(id) == cellDic.end()) {
		cellDic[id] = cell;
	}
	else {
		//error
	}

	E_CellType cellType = cell->GetCellType();
	//ゲームオブジェクトを種類によって分類してからリストに追加する
	if (cellTypeDic.find(cellType) == cellTypeDic.end()) {
		cellTypeDic[cellType].clear();//init
	}
	if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
		return; //error
	}
	cellTypeDic[cellType][id] = cell;

	//ゲームオブジェクトを描画順序によって分類してからリストに追加する
	E_DrawLayer drawLayer = cell->GetDrawLayer();
	int drawLayerIndex = (int)drawLayer;
	if (drawCellDic.find(drawLayerIndex) == drawCellDic.end()) {
		drawCellDic[drawLayerIndex].clear();
	}
	if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
		//error
	}
	else {
		drawCellDic[drawLayerIndex][id] = cell;
	}
}

void SceneObjMgr::RegisterCell(BaseCell* cell, bool recordDefaultState) {
	if (cell == NULL) {
		return;
	}

	if (recordDefaultState == true) {
		cell->RecordCurState();//初期の状態を保存する
	}

	int id = cell->GetID();
	//ゲームオブジェクトのIDを添え字としてリストに追加する
	if (cellDic.find(id) == cellDic.end()) {
		cellDic[id] = cell;
	}
	else {
		//error
	}

	E_CellType cellType = cell->GetCellType();
	//ゲームオブジェクトを種類によって分類してからリストに追加する
	if (cellTypeDic.find(cellType) == cellTypeDic.end()) {
		cellTypeDic[cellType].clear();//init
	}
	if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
		//error
	}
	else {
		cellTypeDic[cellType][id] = cell;
	}

	//ゲームオブジェクトを描画順序によって分類してからリストに追加する
	E_DrawLayer drawLayer = cell->GetDrawLayer();
	int drawLayerIndex = (int)drawLayer;
	if (drawCellDic.find(drawLayerIndex) == drawCellDic.end()) {
		drawCellDic[drawLayerIndex].clear();
	}
	if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
		//error
	}
	else {
		drawCellDic[drawLayerIndex][id] = cell;
	}

}

void SceneObjMgr::InitCornetState(vector3s defaultPos)
{
	if (m_nezumi != nullptr) {
		m_nezumi->SetPos(defaultPos, true);
		m_nezumi->SetState(true);
		m_nezumi->SetStatus(E_EnemyStatus::ENEMY_STS_NONE);
	}
}

void SceneObjMgr::UninitCornetState()
{
	if (m_nezumi != nullptr) {
		m_nezumi->SetState(false);
	}
}

/// <summary>
/// ゲームオブジェクトを削除する
/// </summary>
/// <param name="id"></param>
void SceneObjMgr::UnRegisterCell(int id) {
	if (cellDic.find(id) == cellDic.end()) {
		return;
	}
	BaseCell* cell = cellDic[id];
	cellDic.erase(id);

	if (cell != NULL) {

		E_CellType cellType = cell->GetCellType();
		if (cellTypeDic.find(cellType) != cellTypeDic.end()) {
			if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
				cellTypeDic[cellType].erase(id);
			}
		}

		//draw cell dic
		E_DrawLayer drawLayer = cell->GetDrawLayer();
		int drawLayerIndex = (int)drawLayer;
		if (drawCellDic.find(drawLayerIndex) != drawCellDic.end()) {
			if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
				drawCellDic[drawLayerIndex].erase(id);
			}
		}


		delete cell;
	}
}

/// <summary>
/// ゲームオブジェクトを削除する
/// </summary>
/// <param name="cell"></param>
void SceneObjMgr::UnRegisterCell(BaseCell* cell) {
	if (cell == NULL)return;
	int id = cell->GetID();
	if (cellDic.find(id) == cellDic.end()) {
		return;
	}

	cellDic.erase(id);

	if (cell != NULL) {

		E_CellType cellType = cell->GetCellType();
		if (cellTypeDic.find(cellType) != cellTypeDic.end()) {
			if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
				cellTypeDic[cellType].erase(id);
			}
		}

		//draw cell dic
		E_DrawLayer drawLayer = cell->GetDrawLayer();
		int drawLayerIndex = (int)drawLayer;
		if (drawCellDic.find(drawLayerIndex) != drawCellDic.end()) {
			if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
				drawCellDic[drawLayerIndex].erase(id);
			}
		}

		delete cell;
	}
}

/// <summary>
/// IDによってゲームオブジェクトのインスタンスを取得する
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
BaseCell* SceneObjMgr::GetCell(int id) {
	map<int, BaseCell*>::iterator iter = cellDic.find(id);
	if (iter != cellDic.end()) {
		return iter->second;
	}

	return NULL;
}

/// <summary>
/// プレイヤーゲームオブジェクトを取得する
/// </summary>
/// <returns></returns>
Player* SceneObjMgr::GetPlayer()
{
	return m_player;
}

Nezumi* SceneObjMgr::GetNezumi()
{
	return m_nezumi;
}

/// <summary>
/// プレイヤーの当たり判定
/// </summary>
/// <param name="player"></param>
//void SceneObjMgr::CheckPlayerCollision(Player* player) {
//	if (player == NULL)return;
//
//	//障害物との当たり判定
//	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true,0), false);
//				player->SetCurGounrd(iter->second);
//			}
//		}
//
//	}
//	//アイスとの当たり判定
//	if (cellTypeDic.find(E_CellType::CellType_Ice) != cellTypeDic.end()) {
//
//		bool iceCollision = false;
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Ice];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				player->SetOnIce(true);
//				iceCollision = true;
//				break;
//			}
//		}
//		if (iceCollision == false) {
//			player->SetOnIce(false);
//		}
//	}
//	//ゴールとの当たり判定
//	if (cellTypeDic.find(E_CellType::CellType_TargetPoint) != cellTypeDic.end()) {
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_TargetPoint];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				//clear
//				//次のステージ（シーン）に切り替え
//				GameMgr::GetInstance()->sceneMgr->SwitchNextScene();
//				break;
//			}
//		}
//	}
//	/*
//	//敵との当たり判定
//	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Enemy];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
//			}
//		}
//
//	}
//	*/
//}

bool SceneObjMgr::CheckTempCollision()
{
	if (tempCell == NULL) {
		return false;
	}
	bool returnSign = false;
	//障害物との当たり判定
	//もし他のものと当たり判定したい場合は
	//E_CellType::CellType_Obstacleを他の値に変えればいいです　（E_CellType::CellType_Enemyとか
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//当たり判定
				//adjust pos
				//移動先に移動できないだとしても、ぶつかる前の位置に移動できるので、その位置に移動させる
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true,0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true,0) , iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}
	if (cellTypeDic.find(E_CellType::CellType_MoveGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_MoveGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//当たり判定
				//adjust pos
				//移動先に移動できないだとしても、ぶつかる前の位置に移動できるので、その位置に移動させる
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true,0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	if (cellTypeDic.find(E_CellType::CellType_AppearGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_AppearGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//当たり判定
				//adjust pos
				//移動先に移動できないだとしても、ぶつかる前の位置に移動できるので、その位置に移動させる
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	if (cellTypeDic.find(E_CellType::CellType_SpringGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_SpringGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//当たり判定
				//adjust pos
				//移動先に移動できないだとしても、ぶつかる前の位置に移動できるので、その位置に移動させる
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	return returnSign;
}


void SceneObjMgr::InitPlayerObjState(vector3s defaultPos)
{
	if (m_player != NULL) {
		m_player->Reset();
		m_player->SetinAir(false);
		m_player->SetvelocityY(0.0f);
		m_player->SetPos(defaultPos, true);
		m_player->SetState(true);
		m_player->SetCurGround(NULL);
		m_player->inputLock = false;
		//m_player->m_winSign = false;
		m_player->SetWinSign(false);
	}
}

void SceneObjMgr::UninitPlayerObjState()
{
	if (m_player != NULL) {
		m_player->Reset();
		m_player->SetPos(vector3s(SCREEN_WIDTH*0.3f,SCREEN_HEIGHT*0.5f,1.0f), true);
		m_player->SetState(false);
		m_player->SetCurGround(NULL);
		m_player->inputLock = true;
	}

}



BaseCell* SceneObjMgr::GetTempCell()
{
	return tempCell;
}

void SceneObjMgr::SetTempCellParam(vector3s pos, vector3s rot, vector3s scale, vector2s colliderSize,int colliderDataIndex)
{
	if (tempCell == NULL)return;
	tempCell->SetPos(pos,true);
	tempCell->SetRot(rot);
	tempCell->SetScale(scale);

	Collider* collider = (Collider*) tempCell->GetComponent(E_Component::Component_Collider);
	if (collider != NULL) {
		collider->SetColliderSize(colliderSize, colliderDataIndex);
	}

}


//メモリプール
//種類によってオブジェクトを取得
BaseCell* SceneObjMgr::CreateCell(E_CellType _cellType, int _count, ...)//[可変長引数]　CreateCell（E_CellType , 後ろについている引数の数、他の引数...)
{
	E_ObjCellType objCellType = E_ObjCellType::ObjCellType_None;

	//if (_cellType == E_CellType::CellType_Boss) {
	//	objCellType = E_ObjCellType::ObjCellType_Boss;
	//	BaseCell* cell = m_objectPoolMgr->GetCellFromPool(objCellType);//メモリプールからオブジェクトを取り出す
	//	if (cell == NULL)return NULL;

	//	return (BaseCell*)cell;
	//}

	va_list args;

	if (_cellType == E_CellType::CellType_Obstacle) {
		va_start(args, _count);

		E_Obstacle obstacleType = va_arg(args, E_Obstacle);
		switch (obstacleType) {
		case E_Obstacle::Obstacle_Ground:
			objCellType = E_ObjCellType::ObjCellType_Ground;
			break;
		case E_Obstacle::Obstacle_Ice:
			objCellType = E_ObjCellType::ObjCellType_Ice;
			break;
		case E_Obstacle::Obstacle_Stone:
			objCellType = E_ObjCellType::ObjCellType_Stone;
			break;
		default:
			break;
		}
		va_end(args);

	}
	else if (_cellType == E_CellType::CellType_Enemy_Dumpling) {

		objCellType = E_ObjCellType::ObjCellType_Enemy_Hiyoko;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Ittanmomen) {

		objCellType = E_ObjCellType::ObjCellType_Enemy_Karasu;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Pasta) {
		objCellType = E_ObjCellType::ObjCellType_Enemy_Monkey;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Cornet) {
		objCellType = E_ObjCellType::ObjCellType_Enemy_Nezumi;
	}
	else if (_cellType == E_CellType::CellType_Boss) {
		objCellType = E_ObjCellType::ObjCellType_Boss;
	}
	else if (_cellType == E_CellType::CellType_Ice)
	{
		objCellType = E_ObjCellType::ObjCellType_Ice;
	}
	else if (_cellType == E_CellType::CellType_Bullet)
	{
		objCellType = E_ObjCellType::ObjCellType_Bullet;
	}
	else if (_cellType == E_CellType::CellType_Bullet_Fireball) {
		objCellType = E_ObjCellType::ObjCellType_Bullet_Fireball;
	}

	else if (_cellType == E_CellType::CellType_BeltConveyor)
	{
		objCellType = E_ObjCellType::ObjCellType_BeltConveyor;
	}
	else if (_cellType == E_CellType::CellType_Wind)
	{
		objCellType = E_ObjCellType::ObjCellType_Wind;
	}
	else if (_cellType == E_CellType::CellType_MoveGround)
	{
		objCellType = E_ObjCellType::ObjCellType_MoveGround;
	}
	else if (_cellType == E_CellType::CellType_AppearGround)
	{
		objCellType = E_ObjCellType::ObjCellType_AppearGround;
	}
	else if (_cellType == E_CellType::CellType_SpringGround) 
	{
		objCellType = E_ObjCellType::ObjCellType_SpringGround;
	}
	else if (_cellType == E_CellType::CellType_Onigiri) {
		objCellType = E_ObjCellType::ObjCellType_Onigiri;
	}
	else if (_cellType == E_CellType::CellType_Sugar) {
		objCellType = E_ObjCellType::ObjCellType_Sugar;
	}
	else if (_cellType == E_CellType::CellType_Pepper) {
		objCellType = E_ObjCellType::ObjCellType_Pepper;
	}
	else if (_cellType == E_CellType::CellType_Pickled_Plums) {
		objCellType = E_ObjCellType::ObjCellType_Pickled_Plums;
	}
	else if (_cellType == E_CellType::CellType_Miso_Soup) {
		objCellType = E_ObjCellType::ObjCellType_Miso_Soup;
	}
	else if (_cellType == E_CellType::CellType_Effect_Hit) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Hit;
	}
	else if (_cellType == E_CellType::CellType_Effect_Jump) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Jump;
	}
	else if (_cellType == E_CellType::CellType_Effect_Rush) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Rush;
	}
	else if (_cellType == E_CellType::CellType_Effect_Recover) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Recover;
	}
	else if (_cellType == E_CellType::CellType_Effect_Button) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Button;
	}


	BaseCell* cell = m_objectPoolMgr->GetCellFromPool(objCellType);//メモリプールからオブジェクトを取り出す
	if (cell == NULL)return NULL;

	return (BaseCell*)cell;
}

void SceneObjMgr::RecycleCell(BaseCell* _cell)
{
	if (m_objectPoolMgr != NULL) {
		m_objectPoolMgr->RecycleCell(_cell);
	}
}

void SceneObjMgr::DeleteCell(BaseCell* _cell)
{
	if (_cell == NULL)return;
	if (m_deleteCellDic.find(_cell->GetID()) == m_deleteCellDic.end()) {
		m_deleteCellDic[_cell->GetID()] = _cell;
	}
}

void SceneObjMgr::DeleteCell()
{
	for (auto& temp : m_deleteCellDic) {
		if (temp.second == NULL)continue;
		this->UnRegisterCell(temp.second);
	}

	m_deleteCellDic.clear();
}


#pragma endregion SceneObjMgr_method

#pragma region effect_manager

EffectMgr::EffectMgr()
{
	cameraShakeFactor = 0.0f;
	cameraShakeTime = 0.0f;
	cameraShakeTimePass = 0.0f;
	shakeTargetPos = vector3s(0, 0, 0);

	cameraShakeSwitch = false;
	shakePosOffset = vector3s(0.0f, 0.0f, 0.0f);

	sceneTransformBg = NULL;
	sceneTransformBg_2 = NULL;

	hitEffect_end_cb = new Anim_Effect_Normal_CB();

}

EffectMgr::~EffectMgr()
{
	sceneTransformBg = NULL;
	sceneTransformBg_2 = NULL;

	if (hitEffect_end_cb != NULL) {
		delete hitEffect_end_cb;
	}
	hitEffect_end_cb = NULL;
}

void EffectMgr::DoInit()
{
	////create effect data　：TODO
	//scene transform bg
	sceneTransformBg = new BaseCell();
	sceneTransformBg->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	sceneTransformBg->SetCellType(E_CellType::CellType_BG);
	sceneTransformBg->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	sceneTransformBg->SetPos(vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), true);

	DrawData* sceneTransformBg_drawData = new DrawData();
	sceneTransformBg_drawData->SetSize(sceneTransformBGSize);
	sceneTransformBg_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\SceneTransformBG.png"));
	sceneTransformBg_drawData->usePreComputeUV = true;
	sceneTransformBg_drawData->preComputeUV = vector2s(0, 0);
	sceneTransformBg_drawData->preComputeUVSize = vector2s(1, 1);
	sceneTransformBg_drawData->tex_texel_size = vector2s(1, 1);
	sceneTransformBg->SetDrawData(sceneTransformBg_drawData);

	Movement_Axis* sceneTransformBg_movement = new Movement_Axis(sceneTransformBg, false, true, false, false, 0.8f,
		vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.5f, -1.0f));
	sceneTransformBg->AddMovement(sceneTransformBg_movement);

	sceneTransformBg->SetState(false);

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sceneTransformBg, true);

	sceneTransformBg_2 = new BaseCell();
	sceneTransformBg_2->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	sceneTransformBg_2->SetCellType(E_CellType::CellType_BG);
	sceneTransformBg_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	sceneTransformBg_2->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), true);
	sceneTransformBg_2->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* sceneTransformBg_2_drawData = new DrawData();
	sceneTransformBg_2_drawData->SetSize(sceneTransformBGSize);
	sceneTransformBg_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\sceneTransformBg.png"));
	sceneTransformBg_2_drawData->usePreComputeUV = true;
	sceneTransformBg_2_drawData->preComputeUV = vector2s(0, 0);
	sceneTransformBg_2_drawData->preComputeUVSize = vector2s(1, 1);
	sceneTransformBg_2_drawData->tex_texel_size = vector2s(1, 1);
	sceneTransformBg_2->SetDrawData(sceneTransformBg_2_drawData);

	Movement_Axis* sceneTransformBg_2_movement = new Movement_Axis(sceneTransformBg_2, false, true, false, false, 0.8f,
		vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH + sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f));
	sceneTransformBg_2->AddMovement(sceneTransformBg_2_movement);

	sceneTransformBg_2->SetState(false);

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sceneTransformBg_2, true);


	//20230119 桜吹雪初期化
	sakuraFubuki = new EffectSakura();
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sakuraFubuki, true);//SceneObjMgrに保存してもらう
}

void EffectMgr::UnInit()
{

}

void EffectMgr::DoUpdate(float deltatime)
{
	//カメラ揺れ処理
	if (cameraShakeSwitch == true) {

		cameraShakeFactor -= deltatime * CAMERA_SHAKE_REDUCE_FACTOR;//強さをだんだん減らしていく
		//ランダム方向に揺れる
		shakeTargetPos.x = (rand() % 200 - 100) * 0.01f;
		shakeTargetPos.y = (rand() % 200 - 100) * 0.01f;
		shakeTargetPos.z = 0.0f;
		shakeTargetPos = VecNormalize(shakeTargetPos);//方向ベクトルを正規化する
		shakeTargetPos = shakeTargetPos * cameraShakeFactor;//強さの値をかける
		shakeTargetPos = shakeTargetPos;//目標の位置をアップデートする

		shakePosOffset = VecLerp(shakePosOffset, shakeTargetPos, deltatime * cameraShakeFactor);//補間関数でカメラの次の位置を取得する

		cameraShakeTimePass += deltatime;
		if (cameraShakeTimePass >= cameraShakeTime) {//カメラ揺れが終わり
			cameraShakeSwitch = false;//スイッチオフ
			cameraShakeFactor = 0.0f;
			cameraShakeTime = 0.0f;
		}
	}
}

void EffectMgr::SetCameraShake(float shakeTime, float shakeFactor)
{
	if (cameraShakeSwitch == false) {//カメラ揺れがオフの場合、まず初期化する
		shakePosOffset = vector3s(0.0f, 0.0f, 0.0f);
	}
	else {//カメラ揺れがオンの場合（作動中）
		if (shakeFactor < cameraShakeFactor) { //もしカメラ揺れの強さが今の強さより弱いであれば、上書きできないことにする
			return;
		}
	}

	cameraShakeSwitch = true;//スイッチオン
	cameraShakeTime = shakeTime;//持続時間
	cameraShakeFactor = shakeFactor;//強さ
	cameraShakeTimePass = 0.0f;//経過時間を初期化する
}


/// <summary>
/// シーンの切り替えのアニメーションを使う
/// </summary>
/// <param name="show">　使うかどうか</param>
/// <param name="swapIn">　true : フェイトイン　false : フェイトアウト</param>
void EffectMgr::ShowSceneTransformBG(bool show, bool swapIn)
{
	if (swapIn == true) {
		if (sceneTransformBg == NULL)return;
		if (show == true) {
			sceneTransformBg->Reset();
			sceneTransformBg->StartMovement(E_MovementType::MovementType_None, true,true);
			sceneTransformBg->SetState(true);
		}
		else {
			sceneTransformBg->SetState(false);
		}
	}
	else {
		if (sceneTransformBg != NULL) {
			sceneTransformBg->SetState(false);
		}

		if (sceneTransformBg_2 == NULL)return;
		if (show == true) {
			sceneTransformBg_2->Reset();
			sceneTransformBg_2->StartMovement(E_MovementType::MovementType_None, true,true);
			sceneTransformBg_2->SetState(true);
		}
		else {
			sceneTransformBg_2->SetState(false);
		}
	}
}

/// <summary>
/// 切り替えのアニメーションの状態をチェック
/// </summary>
/// <returns></returns>
bool EffectMgr::CheckSceneTransformBGSwapInAnimDone()
{
	if (sceneTransformBg == NULL || sceneTransformBg->CheckState() == false)return true;

	return sceneTransformBg->CheckCurMovementDoneState(E_MovementType::MovementType_Axis);
}

void EffectMgr::ShowSakura()
{
	sakuraFubuki->SetState(true);
}

void EffectMgr::HideSakura()
{
	sakuraFubuki->SetState(false);
}


/// <summary>
/// 切り替えのアニメーションの状態をチェック
/// </summary>
/// <returns></returns>
bool EffectMgr::CheckSceneTransformBGSwapOutAnimDone()
{
	if (sceneTransformBg_2 == NULL || sceneTransformBg_2->CheckState() == false)return true;

	return sceneTransformBg_2->CheckCurMovementDoneState(E_MovementType::MovementType_Axis);
}

Effect* EffectMgr::CreateEffect(E_Effect effectType, BaseCell* followObj, vector3s pos, vector3s dir)
{
	Effect* effect = NULL;
	switch (effectType) {
	case E_Effect::Effect_Hit:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Hit);
		break;
	case E_Effect::Effect_Jump:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Jump);
		break;
	case E_Effect::Effect_Rush:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Rush);
		break;
	case E_Effect::Effect_Recover:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Recover);
		break;
	case E_Effect::Effect_Button:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Button);
		break;
	default:
		break;
	}
	if (effect == NULL)return NULL;

	//calc rot
	//default dir: right
	vector3s directoin = VecNormalize(dir);

	float angle = (atan2(rightVector.x, rightVector.y) - atan2(dir.x, dir.y)) * RAD2DEG;
	effect->GetTransform()->SetRot(vector3s(0, 0, angle));

	pos.z = -10;
	effect->SetPos(pos, true);
	effect->StartEffect();
	effect->SetState(true);

	return effect;
}


#pragma endregion effect_manager


#pragma region scene_manager_method

SceneMgr::SceneMgr() : defaultSceneType(E_Scene::Scene_None), curSceneType(E_Scene::Scene_None), curScene(NULL),targetScene(E_Scene::Scene_None),reloadTargetScene(false)
{
	switchSceneSign = switchSceneSign_1 = switchSceneSign_2 = false;
	stageMgr = new StageManager();
	sceneDic.clear();

	char filename_GameClear[] = "data\\BGM\\GameClear.wav";
	m_sound_GameClear = LoadSound(filename_GameClear);

	char filename_GameOver[] = "data\\BGM\\GameOver.wav";
	m_sound_GameOver = LoadSound(filename_GameOver);

	char filename_GamePlay[] = "data\\BGM\\GamePlay.wav";
	m_sound_GamePlay = LoadSound(filename_GamePlay);

	char filename_title[] = "data\\BGM\\Shall_we_meet？.wav";
	m_sound_title = LoadSound(filename_title);
}

SceneMgr::~SceneMgr()
{
	if (stageMgr != NULL) {
		delete stageMgr;
		stageMgr = NULL;
	}
	curScene = NULL;
}

void SceneMgr::DoInit()
{
	Scene_1* scene_1 = new Scene_1();
	scene_1->SetNextScene(E_Scene::Scene_Stage2);
	scene_1->SetDefaultPlayerPos(player_stage_1_defaultPos);

	Scene_2* scene_2 = new Scene_2();
	scene_2->SetNextScene(E_Scene::Scene_Stage3);
	scene_2->SetDefaultPlayerPos(player_stage_1_defaultPos);

	Scene_3* scene_3 = new Scene_3();
	scene_3->SetNextScene(E_Scene::Scene_Stage4);
	scene_3->SetDefaultPlayerPos(player_stage_3_defaultPos);

	Scene_4* scene_4 = new Scene_4();
	scene_4->SetNextScene(E_Scene::Scene_Stage5);
	scene_4->SetDefaultPlayerPos(player_stage_4_defaultPos);

	Scene_5* scene_5 = new Scene_5();
	scene_5->SetNextScene(E_Scene::Scene_End);
	scene_5->SetDefaultPlayerPos(player_stage_5_defaultPos);

	TestScene* testScene = new TestScene();
	testScene->SetNextScene(E_Scene::Scene_End);
	testScene->SetDefaultPlayerPos(player_stage_5_defaultPos);

	//==============
	// 追加松尾
	//==============
	BossScene* bossScene = new BossScene();
	bossScene->SetNextScene(E_Scene::Scene_End);
	bossScene->SetDefaultPlayerPos(player_stage_5_defaultPos);

	RegisterScene(scene_1, true);
	RegisterScene(scene_2, false);
	RegisterScene(scene_3, false);
	RegisterScene(scene_4, false);
	RegisterScene(scene_5, false);

	RegisterScene(testScene, false);

	//==============
	// 追加松尾
	//==============
	RegisterScene(bossScene, false);


	switchSceneSign = switchSceneSign_1 = switchSceneSign_2 = switchSceneSign_3 = false;
	targetScene = E_Scene::Scene_None;
	reloadTargetScene = false;
	//load default scene
	//SwitchScene(defaultSceneType,true);
}

void SceneMgr::UnInit()
{
	ClearSceneDic();
}

void SceneMgr::DoUpdate(float deltatime)
{
	if (curScene != NULL) {
		curScene->DoUpdate(deltatime);
	}

	//シーンの切り替えのアニメーション処理
	if (switchSceneSign == true) {
		if (switchSceneSign_1 == true) {
			if (GameMgr::GetInstance()->effectMgr->CheckSceneTransformBGSwapInAnimDone() == true) {
				if ((targetScene == E_Scene::Scene_End || targetScene == E_Scene::Scene_None)) {
					if (GameMgr::GetInstance()->GetCurGameState() != E_GameState::GameState_Game_Lose) {
						SwitchScene_2(E_Scene::Scene_None, reloadTargetScene);
						GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_End, true);
					}
					else {
						switchSceneSign = false;
						switchSceneSign_1 = false;
						switchSceneSign_2 = false;
						switchSceneSign_3 = false;

						GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, true);
						GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, false);
						return;
					}
				}
				else {
					SwitchScene_2(targetScene, reloadTargetScene);//実際にシーンを切り替える
				}
				switchSceneSign_1 = false;
				switchSceneSign_2 = true;
			}
		}
		else if (switchSceneSign_2 == true) {
			GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, true);
			GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(true, false);
			//GameMgr::GetInstance()->inputMgr->SetInputState(true);
			switchSceneSign_2 = false;
			switchSceneSign_3 = true;

		}
		else if (switchSceneSign_3 == true) {
			if (GameMgr::GetInstance()->effectMgr->CheckSceneTransformBGSwapOutAnimDone() == true) {
				switchSceneSign_3 = false;
				switchSceneSign = false;
				GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, false);
			}
		}
	}

}

void SceneMgr::DoLateUpdate(float deltatime)
{
}

/// <summary>
/// シーンを追加する
/// </summary>
/// <param name="s">　シーンのインスタンス</param>
/// <param name="setDefaultScene"></param>
void SceneMgr::RegisterScene(Scene* s, bool setDefaultScene)
{
	if (s == NULL)return;
	E_Scene sceneType = s->GetSceneType();
	if (sceneDic.size() == 0 || sceneDic.find(sceneType) == sceneDic.end()) {
		sceneDic[sceneType] = s;//シーンを配列の中に追加する

		if (setDefaultScene == true) {
			defaultSceneType = sceneType;
		}
	}
	else {
		//error
	}
}

void SceneMgr::UnregisterScene(Scene* s)
{
	if (s == NULL || sceneDic.size() == 0)return;

	map<E_Scene, Scene*>::iterator iter = sceneDic.find(s->GetSceneType());
	if (iter != sceneDic.end()) {
		sceneDic.erase(iter);
	}
}

/// <summary>
/// 指定されたシーンに切り替える
/// </summary>
/// <param name="sceneType">　次のシーン</param>
/// <param name="reload">　次のシーンを初期化するかどうか</param>
/// <param name="activeTransformAnim">　切り替えのアニメーションを使うかどうか</param>
void SceneMgr::SwitchScene(E_Scene sceneType, bool reload,bool activeTransformAnim) {
	if (activeTransformAnim == false) {
		SwitchScene_2(sceneType, reload);
	}
	else {

		if (switchSceneSign == true) {
			return;
		}

		targetScene = sceneType;//次のシーンを一旦保存する
		reloadTargetScene = reload;
		switchSceneSign = true;
		switchSceneSign_1 = true;
		switchSceneSign_2 = false;
		switchSceneSign_3 = false;

		GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(true,true);
		//GameMgr::GetInstance()->inputMgr->SetInputState(false);

		//GameMgr::GetInstance()->effectMgr->ShowSceneTransformLogo(true,true);
		//GameMgr::GetInstance()->inputMgr->SetInputState(false);
	}
}

/// <summary>
/// 実際にシーンを切り替える処理
/// </summary>
/// <param name="sceneType">　次のシーン</param>
/// <param name="reload">　次のシーンを初期化するかどうか</param>
void SceneMgr::SwitchScene_2(E_Scene sceneType, bool reload)
{
	//GameMgr::GetInstance()->uiMgr->ShowTitle(false);
	//GameMgr::GetInstance()->uiMgr->ShowEndMenu(false);
	//GameMgr::GetInstance()->effectMgr->HideSakura();

	if (sceneType == curSceneType && sceneType != E_Scene::Scene_None && reload == false) {//次のシーンが今のシーンと同じであれば
		ResetCurScene();
		return;
	}

	if (sceneDic.find(sceneType) == sceneDic.end() && sceneDic[sceneType] != NULL) {//シーンが見つからないであれば、リターン
		return;
	}

	if (curScene != NULL) {
		curScene->Unload();//今のシーンの終了処理
		curScene = NULL;
	}

	curScene = sceneDic[sceneType];
	if (curScene != NULL) {
		curScene->Load();//次のシーンをロードする
		curSceneType = sceneType;
	}
}

/// <summary>
/// あらかじめ設定された次のシーンに切り替える
/// </summary>
void SceneMgr::SwitchNextScene()
{
	if (curScene != NULL) {
		E_Scene nextScene = curScene->GetNextScene();//次のシーンを取得する
		SwitchScene(nextScene, true, true);
	}
}

/// <summary>
/// 今のシーンをもう一度初期化する
/// </summary>
void SceneMgr::ResetCurScene()
{
	SwitchScene(curSceneType, true, true);
}

void SceneMgr::ClearSceneDic()
{
	map<E_Scene, Scene*>::iterator iter;
	for (iter = sceneDic.begin(); iter != sceneDic.end(); iter++) {
		if (iter->second != NULL) {
			delete iter->second;
		}
	}
	sceneDic.clear();
}

/// <summary>
/// 今のシーンを取得する
/// </summary>
/// <returns></returns>
E_Scene SceneMgr::GetCurSceneType()
{
	return curSceneType;
}

vector3s SceneMgr::GetSceneCameraPos()
{
	//if (curScene != NULL) {
	//	return curScene->GetSceneCameraPos();
	//}
	return vector3s(0, 0, 0);
}

Scene* SceneMgr::GetCurScene()
{
	return curScene;
}


#pragma endregion scene_manager_method

#pragma region ui_manager

UIMgr::UIMgr()
{
	logoMenu = NULL;
	titleMenu = NULL;
	endMenu = NULL;
	m_title = NULL;
	//早川追加(仮)=======
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = NULL;
	}
	//===============
}

UIMgr::~UIMgr() {
	logoMenu = NULL;
	titleMenu = NULL;
	endMenu = NULL;
	m_title = NULL;

	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = NULL;
	}
	//===============
}

void UIMgr::DoInit()
{
	//=======================================================================
	//追加松尾
	//ロゴ表示画面を作る
	logoMenu = new BaseCell();
	logoMenu->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	logoMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	logoMenu->SetCellType(E_CellType::CellType_Menu);//ゲームオブジェクトのタイプをMenuに設定する
	logoMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置

	DrawData* logoMenu_drawData = new DrawData();
	logoMenu_drawData->SetSize(vector2s(SCREEN_WIDTH, SCREEN_HEIGHT));//描画サイズ
	logoMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));//テクスチャ―
	logoMenu_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	logoMenu_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	logoMenu_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	logoMenu_drawData->tex_texel_size = vector2s(1, 1);
	logoMenu->SetDrawData(logoMenu_drawData);//描画データを渡す

	logoMenu->SetState(false);//非表示にする

	//========================================================================
	//ロゴ表示
	teamLogo = new BaseCell();
	teamLogo->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//描画順序を設定する
	teamLogo->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	teamLogo->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
	teamLogo->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
	teamLogo_drawData = new DrawData();
	teamLogo_drawData->SetSize(vector2s(842, 596));//描画サイズ
	teamLogo_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\TeamLogo.png"));//テクスチャ―
	teamLogo_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	teamLogo_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	teamLogo_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	teamLogo_drawData->tex_texel_size = vector2s(1, 1);
	teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
	teamLogo->SetDrawData(teamLogo_drawData);//描画データを渡す

	teamLogo->SetState(false);//非表示にする
	//
	//=======================================================================

	//タイトルUIを作る
	titleMenu = new BaseCell();
	titleMenu->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	titleMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	titleMenu->SetCellType(E_CellType::CellType_Menu);//ゲームオブジェクトのタイプをMenuに設定する
	titleMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
		//描画データを作る
	DrawData* titleMenu_drawData = new DrawData();
	titleMenu_drawData->SetSize(vector2s(SCREEN_WIDTH, SCREEN_HEIGHT));//描画サイズ
	titleMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));//テクスチャ―
	titleMenu_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	titleMenu_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	titleMenu_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	titleMenu_drawData->tex_texel_size = vector2s(1, 1);
	titleMenu_drawData->SetColor(D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f));
	titleMenu->SetDrawData(titleMenu_drawData);//描画データを渡す

	titleMenu->SetState(false);//非表示にする

	m_title = new BaseCell();
	m_title->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	m_title->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	m_title->SetCellType(E_CellType::CellType_Menu);//ゲームオブジェクトのタイプをMenuに設定する
	m_title->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
		//描画データを作る
	DrawData* title_drawData = new DrawData();
	//title_drawData->SetSize(vector2s(1784.0f, 1784*0.563f));//描画サイズ
	title_drawData->SetSize(vector2s(1920.0f, 1080.0f));//描画サイズ
	title_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\title.png"));//テクスチャ―
	title_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	title_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	title_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	title_drawData->tex_texel_size = vector2s(1, 1);
	m_title->SetDrawData(title_drawData);//描画データを渡す

	m_title->SetState(false);//非表示にする

	//ゲームオーバーUIを作る
	gameOver = new BaseCell();
	gameOver->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//描画順序を設定する
	gameOver->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	gameOver->SetCellType(E_CellType::CellType_GameOver);//ゲームオブジェクトのタイプをMenuに設定する
	gameOver->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
	//描画データを作る
	DrawData* gameOver_drawData = new DrawData();
	//gameOver_drawData->SetSize(vector2s(1784.0f, 1784*0.563f));//描画サイズ
	gameOver_drawData->SetSize(vector2s(1920.0f, 1080.0f));//描画サイズ
	gameOver_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\GameOver.png"));//テクスチャ―
	gameOver_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	gameOver_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	gameOver_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	gameOver_drawData->tex_texel_size = vector2s(1, 1);
	gameOver->SetDrawData(gameOver_drawData);//描画データを渡す

	gameOver->SetState(false);//非表示にする


	endMenu = new BaseCell();
	endMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	endMenu->SetCellType(E_CellType::CellType_Menu);//ゲームオブジェクトのタイプをMenuに設定する
	endMenu->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//描画順序を設定する
	endMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
		//描画データを作る
	DrawData* endMenu_drawData = new DrawData();
	endMenu_drawData->SetSize(vector2s(lightTextEff_size.x * 0.8f, lightTextEff_size.y * 0.8f));
	endMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\lightText.png"));
	//endMenu_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	//endMenu_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	//endMenu_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	endMenu_drawData->texIndex = vector2s(TEX_EFFECT_LIGHTTEXT_START_INDEX_X, TEX_EFFECT_LIGHTTEXT_START_INDEX_Y);
	endMenu_drawData->tex_w_count = TEX_EFFECT_LIGHTTEXT_CELL_W_COUNT;
	endMenu_drawData->tex_texel_size = lightText_cell_tex_size;
	endMenu->SetDrawData(endMenu_drawData);//描画データを渡す

	Animation* lightTextAni = new Animation();
	endMenu->RegisterComponent(E_Component::Component_Animation, lightTextAni, true);

	AnimClip* lightTextAniClip = new AnimClip();
	int startTexIndex = 0;
	int endTexIndex = startTexIndex + TEX_EFFECT_LIGHTTEXT_FRAME_COUNT;
	lightTextAniClip->SetAnimParam(E_Anim::Anim_LightTextEff, vector2s(startTexIndex, endTexIndex), LIGHTTEXT_SPEED, true, true);
	lightTextAni->RegisterAnim(E_Anim::Anim_LightTextEff, lightTextAniClip, true);

	endMenu->SetState(false);//非表示にする

		//リザルトエフェクト
	resultEff = new BaseCell();
	resultEff->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	resultEff->SetCellType(E_CellType::CellType_Effect_Result);//ゲームオブジェクトのタイプをMenuに設定する
	resultEff->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	resultEff->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//位置
		//描画データを作る
	DrawData* resultEff_drawData = new DrawData();
	resultEff_drawData->SetSize(resultEff_size);
	resultEff_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\resultEffect.png"));
	//resultEff_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	//resultEff_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	//resultEff_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	resultEff_drawData->texIndex = vector2s(TEX_EFFECT_RESULT_START_INDEX_X, TEX_EFFECT_RESULT_START_INDEX_Y);
	resultEff_drawData->tex_w_count = TEX_EFFECT_RESULT_CELL_W_COUNT;
	resultEff_drawData->tex_texel_size = result_cell_tex_size;
	resultEff->SetDrawData(resultEff_drawData);//描画データを渡す

	Animation* resultEffAni = new Animation();
	resultEff->RegisterComponent(E_Component::Component_Animation, resultEffAni, true);

	AnimClip* resultEffAniClip = new AnimClip();
	startTexIndex = 0;
	endTexIndex = startTexIndex + TEX_EFFECT_RESULT_FRAME_COUNT;
	resultEffAniClip->SetAnimParam(E_Anim::Anim_ResultEff, vector2s(startTexIndex, endTexIndex), RESULTEFF_SPEED, true, true);
	resultEffAni->RegisterAnim(E_Anim::Anim_ResultEff, resultEffAniClip, true);

	resultEff->SetState(false);//非表示にする

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(resultEff, true);

		//早川追加(仮)==============================================
	//**********プレイヤーライフのUIテスト用*************
	campos = MainCamera::GetInstance()->GetCamPos(); //カメラ位置取得

	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = new BaseCell();
		uiLife[i]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
		uiLife[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
		uiLife[i]->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
		uiLife[i]->SetPos(vector3s(120.0f + uiLife_gap * i, 120.0f, 0.0f), true);//位置
		//描画データを作る
		DrawData* uiLife_drawData = new DrawData();
		uiLife_drawData->SetSize(vector2s(120, 120));
		uiLife_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		uiLife_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
		uiLife_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
		uiLife_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
		uiLife_drawData->tex_texel_size = vector2s(1, 1);
		uiLife[i]->SetDrawData(uiLife_drawData);//描画データを渡す
		uiLife[i]->SetState(false);//非表示にする
		Life_Drawflag[i] = true;
		PlyerLife_before = 100;
		HpDecrease = 0;

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(uiLife[i], true);//SceneObjMgrに保存してもらう

		//uiLife_gap += 50; //ライフの表示位置をずらす
	}

	//弾表示のUI
	for (int n = 0; n < UI_BULLET_NUM; n++)
	{
		uiBullet[n] = new BaseCell();
		uiBullet[n]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
		uiBullet[n]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
		uiBullet[n]->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
		uiBullet[n]->SetPos(vector3s(680.0f - uiBullet_gap, 160.0f, 0.0f), true);//位置

		DrawData* uiBullet_drawData = new DrawData();
		uiBullet_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\okome.png"));
		uiBullet_drawData->SetSize(vector2s(40, 50));
		uiBullet_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
		uiBullet_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
		uiBullet_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
		uiBullet_drawData->tex_texel_size = vector2s(1, 1);
		uiBullet[n]->SetDrawData(uiBullet_drawData);//描画データを渡す
		uiBullet[n]->SetState(false);//非表示にする
		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(uiBullet[n], true);//SceneObjMgrに保存してもらう

		Bullet_Drawflag[n] = true;

		uiBullet_gap += 30;
	}
	bulletPos = 680.0f; // 01/23 早川追加

	//======================================================

	//bar表示                                      byヨウ
	//======================================================
	bar = new BaseCell();
	bar->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	bar->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	bar->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをMenuに設定する
	bar->SetPos(vector3s(195, 100, 0.0f), true);//位置
		//描画データを作る
	DrawData* bar_drawData = new DrawData();
	bar_drawData->SetSize(vector2s(300, 20));
	bar_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\bar.png"));
	bar_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	bar_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	bar_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	bar_drawData->tex_texel_size = vector2s(1, 1);
	bar->SetDrawData(bar_drawData);//描画データを渡す

	bar->SetState(false);//非表示にする
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(bar, true);//SceneObjMgrに保存してもらう

	//pIcon表示
	pIcon = new BaseCell();
	pIcon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	pIcon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	pIcon->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをMenuに設定する
	pIcon->SetPos(vector3s(bar->GetTransform()->GetPos().x + (bar_drawData->GetSize().x / 2), 100, 0.0f), true);//位置
		//描画データを作る
	DrawData* pIcon_drawData = new DrawData();
	pIcon_drawData->SetSize(vector2s(60, 60));
	pIcon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\pIcon.png"));
	pIcon_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	pIcon_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	pIcon_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	pIcon_drawData->tex_texel_size = vector2s(1, 1);
	pIcon->SetDrawData(pIcon_drawData);//描画データを渡す

	pIcon->SetState(false);//非表示にする
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(pIcon, true);//SceneObjMgrに保存してもらう

	//eIcon表示
	eIcon = new BaseCell();
	eIcon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	eIcon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	eIcon->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをMenuに設定する
	eIcon->SetPos(vector3s(bar->GetTransform()->GetPos().x - (bar_drawData->GetSize().x / 2), 100, 0.0f), true);//位置
		//描画データを作る
	DrawData* eIcon_drawData = new DrawData();
	eIcon_drawData->SetSize(vector2s(60, 60));
	eIcon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\eIcon.png"));
	eIcon_drawData->usePreComputeUV = true;//テクスチャ―マッピングのタイプを設定する
	eIcon_drawData->preComputeUV = vector2s(0, 0);//UV座標の始点（原点）
	eIcon_drawData->preComputeUVSize = vector2s(1, 1);//図案のサイズ（テクスチャ―のサイズと同じの場合、preComputeUVSizeとtex_texel_sizeも同じくvector2s(1,1)に設定すればOKです
	eIcon_drawData->tex_texel_size = vector2s(1, 1);
	eIcon->SetDrawData(eIcon_drawData);//描画データを渡す

	eIcon->SetState(false);//非表示にする
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(eIcon, true);//SceneObjMgrに保存してもらう
	//======================================================


	//====================23.1　仮
	campos = MainCamera::GetInstance()->GetCamPos(); //カメラ位置取得

	for (int i = 0; i < UI_NUM_COUNT; i++)
	{
		m_numArray[i] = new BaseCell();
		m_numArray[i]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
		m_numArray[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
		m_numArray[i]->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
		m_numArray[i]->SetPos(vector3s(120.0f + uiLife_gap * i, 280.0f, 0.0f), true);//位置
		//描画データを作る
		DrawData* uiNum_drawData = new DrawData();
		uiNum_drawData->SetSize(vector2s(100.0f, 100.0f));
		uiNum_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\num.png"));
		uiNum_drawData->texIndex = vector2s(TEX_NUM_START_INDEX_X + rand()% TEX_NUM_COUNT_W, TEX_NUM_START_INDEX_Y);
		uiNum_drawData->tex_w_count = TEX_NUM_COUNT_W;
		uiNum_drawData->tex_texel_size = num_tex_size;
		m_numArray[i]->SetDrawData(uiNum_drawData);//描画データを渡す
		m_numArray[i]->SetState(false);//非表示にする

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_numArray[i], true);//SceneObjMgrに保存してもらう

		//uiLife_gap += 50; //ライフの表示位置をずらす
	}

	m_icon = new BaseCell();
	m_icon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//描画順序を設定する
	m_icon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	m_icon->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
	m_icon->SetPos(vector3s(150.0f,SCREEN_HEIGHT* 0.4f,0.0f), true);//位置

	//描画データを作る
	DrawData* icon_drawData = new DrawData();
	icon_drawData->SetSize(vector2s(100.0f, 100.0f));
	icon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\cornet_icon.png"));
	icon_drawData->usePreComputeUV = true;
	icon_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	icon_drawData->preComputeUVSize = vector2s(1.0f, 1.0f);
	icon_drawData->tex_texel_size = vector2s(1.0f, 1.0f);
	m_icon->SetDrawData(icon_drawData);//描画データを渡す
	m_icon->SetState(false);//非表示にする

		//スコアテキストをつくる
	scoreText = new BaseCell();
	scoreText->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//描画順序を設定する
	scoreText->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
	scoreText->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
	scoreText->SetPos(vector3s(200.0f, SCREEN_HEIGHT * 0.8f, 0.0f), true);//位置

	//描画データを作る
	DrawData* scoreText_drawData = new DrawData();
	scoreText_drawData->SetSize(vector2s(500.0f, 100.0f));
	scoreText_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\Score.png"));
	scoreText_drawData->usePreComputeUV = true;
	scoreText_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	scoreText_drawData->preComputeUVSize = vector2s(1.0f, 1.0f);
	scoreText_drawData->tex_texel_size = vector2s(1.0f, 1.0f);
	scoreText->SetDrawData(scoreText_drawData);//描画データを渡す
	scoreText->SetState(false);//非表示にする


	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_icon, true);//SceneObjMgrに保存してもらう


	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(titleMenu, true);//SceneObjMgrに保存してもらう
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(endMenu, true);//SceneObjMgrに保存してもらう
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_title, true);//SceneObjMgrに保存してもらう

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(logoMenu, true);//SceneObjMgrに保存してもらう
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(teamLogo, true);//SceneObjMgrに保存してもらう

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(resultEff, true);//SceneObjMgrに保存してもらう
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(gameOver, true);//SceneObjMgrに保存してもらう
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(scoreText, true);//SceneObjMgrに保存してもらう
	//ライフの初期値を保存
	playerLifeMax = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();

	//score===============================
	//campos = MainCamera::GetInstance()->GetCamPos(); //カメラ位置取得

	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i] = new BaseCell();
		m_score[i]->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//描画順序を設定する
		m_score[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//IDを設定する
		m_score[i]->SetCellType(E_CellType::CellType_UI);//ゲームオブジェクトのタイプをUIに設定する
		m_score[i]->SetPos(vector3s(800.0f + uiLife_gap * i, 50.0f, 0.0f), true);//位置

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_score[i], true);//SceneObjMgrに保存してもらう
	}

	playerMaxPos = 0;
	m_number = 0;
}

/// <summary>
/// タイトル画面を表示する
/// </summary>
/// <param name="show"></param>
void UIMgr::ShowTitle(bool show)
{
	if (titleMenu == NULL)return;
	titleMenu->SetState(show);

	if (m_title != NULL)
		m_title->SetState(show);
}

/// <summary>
/// ゲームオーバー画面を表示する
/// </summary>
/// <param name="show"></param>
void UIMgr::ShowEndMenu(bool show)
{
	if (endMenu == NULL)return;
	endMenu->SetState(show);

	int playerHP;
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerHP = player->GetPlayerLifeValue();
	m_number = playerHP * (int)(playerMaxPos / 100);

	//m_number *= playerHP;
	scoreText->SetPos(vector3s(700.0f, SCREEN_LIMIT_DOWN - 50.0f, 0.0f), true);
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i]->SetPos(vector3s(1050.0f + score_gap * i, SCREEN_LIMIT_DOWN - 50.0f, 0.0f), true);//位置
	}
	scoreText->SetState(show);
	ShowScore();
	DrawScore(show);


	resultEff->SetState(show);
}

void UIMgr::ShowGameOver(bool show)
{
	if (gameOver == NULL)return;
	gameOver->SetState(show);
}

//========================================================
//追加松尾

void UIMgr::ShowLogoMenu(bool show)
{
	if (logoMenu == NULL)return;
	logoMenu->SetState(show);
}

void UIMgr::DrawTeamLogo(bool show)
{
	if (teamLogo == NULL)return;
	teamLogo->SetState(show);
}

void UIMgr::DoUpdate(float deltatime)
{
	alphateamLogo = teamLogo_drawData->GetColor().a;

	switch (fadeStatus)
	{
	case FadeIn:
		if (alphateamLogo < 1.0f)
		{
			alphateamLogo += 0.005f;
			if (alphateamLogo >= 1.0f)
			{
				alphateamLogo = 1.0f;
			}
			teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
			if (alphateamLogo == 1.0)
				SetFadeStatus(FadeOut);
		}
		break;
	case FadeOut:
		if (alphateamLogo > 0.0f)
		{
			alphateamLogo -= 0.005f;
			if (alphateamLogo <= 0.0f)
			{
				alphateamLogo = 0.0f;
			}
			teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
			if (alphateamLogo == 0.0)
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Title, true);
		}

	default:
		break;
	}

}
//barの更新と描画     by	ヨウ
//================================================
void UIMgr::UpdateBar()
{
	bar->SetPos(vector3s((campos->x + 195), (campos->y + bar_y_offset), 0.0f), true);//位置
	pIcon->SetPos(vector3s((campos->x + 345), (campos->y + bar_y_offset), 0.0f), true);//位置
	eIcon->SetPos(vector3s((campos->x + 45), (campos->y + bar_y_offset), 0.0f), true);//位置

	float maxX = 300;
	float ePosx = GameMgr::GetInstance()->sceneObjMgr->GetNezumi()->GetTransform()->GetPos().x;
	//float ePosx = 0;
	float pPosx = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetTransform()->GetPos().x;
	float tempx = abs(ePosx - pPosx) / BAR_SCALE_X;
	if (tempx >= 300)
	{
		tempx = 300;
	}
	eIcon->GetTransform()->SetPos(vector3s(pIcon->GetTransform()->GetPos().x - tempx, eIcon->GetTransform()->GetPos().y, 0));

}
void UIMgr::DrawBar(bool show)
{
	if (bar == NULL || eIcon == NULL || pIcon == NULL)
		return;

	bar->SetState(show);
	pIcon->SetState(show);
	eIcon->SetState(show);

}
//=================================

//早川追加=========================
void UIMgr::DoUpdate()
{
	if (m_HUDState == true) {

		DrawLife();
		DrawUIBullet();

		UpdateBar();

		UpdateScore();

		//プレイヤーのライフの取得
		playerLife = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();

		//uiLife_gap = 0.0f;
		for (int i = 0; i < UI_LIFE_NUM; i++)
		{
			uiLife[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 80.0f), 0.0f), true);//位置
			//uiLife_gap += 50; //ライフの表示位置をずらす
			Life_Drawflag[i] = true;
			DrawData* UiLife = uiLife[i]->GetDrawData();

			int LifePerUI = PLAYER_HP / UI_LIFE_NUM; // おにぎり1つ当たりのHP

			if (playerLife <= LifePerUI * (i + 1))
			{
				int LifeExcess = playerLife - (i * LifePerUI); //欠けているおにぎりの部分のライフ量

				if (LifeExcess > (LifePerUI / 4) * 3) // 0126修正
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
				}
				if (LifeExcess <= (LifePerUI / 4) * 3)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP2.png"));
				}
				if (LifeExcess <= (LifePerUI / 4) * 2)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP3.png"));
				}
				if (LifeExcess <= (LifePerUI / 4))
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP4.png"));
				}
				if (LifeExcess <= 0)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP5.png"));
				}

				uiLife[i]->SetDrawData(UiLife);
			}
		}
		//弾のUI
		uiBullet_gap = 0;

		// 01/23 早川追加****************************
		if (playerLife <= PLAYER_HP) { //01/24修正
			bulletPos = 680;
		}
		if (playerLife <= PLAYER_HP - 20) {
			bulletPos = 680 - uiLife_gap;
		}
		if (playerLife <= PLAYER_HP - (20 * 2)) {
			bulletPos = 680 - uiLife_gap * 2;
		}
		if (playerLife <= PLAYER_HP - (20 * 3)) {
			bulletPos = 680 - uiLife_gap * 3;
		}
		if (playerLife <= PLAYER_HP - (20 * 4)) {
			bulletPos = 680 - uiLife_gap * 4;
		}
		//*********************************************

		for (int n = 0; n < UI_BULLET_NUM; n++)
		{
			uiBullet[n]->SetPos(vector3s((campos->x + bulletPos) - uiBullet_gap, (campos->y + 160.0f), 0.0f), true);//位置
			uiBullet_gap += 20;

			//弾のUIの表示を減らす
			if (playerLife < PlyerLife_before)
			{
				if (Bullet_Drawflag[HpDecrease] == true)
				{
					HpDecrease++;
					Bullet_Drawflag[HpDecrease - 1] = false;
				}
			}
			//弾のUIの表示を戻す
			if (HpDecrease >= UI_BULLET_NUM)
			{
				Bullet_Drawflag[n] = true;
				if (Bullet_Drawflag[UI_BULLET_NUM - 1] == true)
				{
					HpDecrease = 0;
				}
			}
			//回復
			if (playerLife > PlyerLife_before)
			{
				// 0126 早川修正******************************
				int life = playerLife % 5;

				if (life == 0) {
					HpDecrease = 0;
				}
				else if (life == 1) {
					HpDecrease = 4;
				}
				else if (life == 2) {
					HpDecrease = 3;
				}
				else if (life == 3) {
					HpDecrease = 2;
				}
				else if (life == 4) {
					HpDecrease = 1;
				}

				for (int m = 0; m < UI_BULLET_NUM; m++) {
					if (life >= UI_BULLET_NUM - m) {
						Bullet_Drawflag[m] = true;
					}
				}
				//***************************************************
			}

			PlyerLife_before = playerLife;
		}
		//uiLife_gap = 0.0f;
		//for (int i = 0; i < UI_LIFE_NUM; i++)
		//{
		//	uiLife[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 120.0f), 0.0f), true);//位置
		//	//uiLife_gap += 50; //ライフの表示位置をずらす

		//	//HPに応じておにぎりを表示
		//	//HP100～91->10コ
		//	//HP90～81 ->9コ
		//	if ((i * 10) >= playerLife)
		//	{
		//		Life_Drawflag[i] = false;
		//	}
		//	else
		//	{
		//		Life_Drawflag[i] = true;
		//	}
		//}

		for (int i = 0; i < UI_NUM_COUNT; i++)
		{
			m_numArray[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 280.0f), 0.0f), true);//位置
		}

		m_icon->SetPos(vector3s(campos->x + 150.0f, SCREEN_HEIGHT * 0.4f, 0.0f), true);//位置
	}
	else {

		DrawScore(false);
		DrawBar(false);

		for (int i = 0; i < UI_BULLET_NUM; i++)
		{
			if (uiBullet[i] == NULL)return;
			uiBullet[i]->SetState(false);
		}
	}

}
//=================================

//========================================================

//早川追加=========================
//HPUIの表示
void UIMgr::DrawLife()
{
	if (m_HUDState == false)return;
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		if (uiLife[i] == NULL)return;
		uiLife[i]->SetState(Life_Drawflag[i]);
	}

}

//弾UI
void UIMgr::DrawUIBullet()
{
	for (int i = 0; i < UI_BULLET_NUM; i++)
	{
		if (uiBullet[i] == NULL)return;
		uiBullet[i]->SetState(Bullet_Drawflag[i]);
	}
}


//=================================

void UIMgr::ShowHUD(bool _show)
{
	m_HUDState = _show;
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		if (uiLife[i] != NULL) {
			uiLife[i]->SetState(_show);
		}
	}
	if (_show == false) {
		for (int i = 0; i < UI_BULLET_NUM; i++)
		{
			if (uiBullet[i] == NULL)return;
			uiBullet[i]->SetState(false);
		}
	}
	//for (int i = 0; i < UI_NUM_COUNT; i++)
	//{
	//	if (m_numArray[i] != NULL) {
	//		m_numArray[i]->SetState(_show);
	//	}
	//}
	//if (m_icon != NULL) {
	//	m_icon->SetState(_show);
	//}
}

//score===============================

void UIMgr::UpdateScore()
{
	vector3s playerPos;

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerPos = player->GetTransform()->GetPos();

	if ((int)playerPos.x > playerMaxPos)
	{
		playerMaxPos = (int)playerPos.x;
		m_number = playerMaxPos / 100;
	}
	else
	{
		m_number = playerMaxPos / 100;
	}



	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i]->SetPos(vector3s((campos->x + 1300 + score_gap * i), (campos->y + 80.0f), 0.0f), true);//位置

	}

	ShowScore();

}

void UIMgr::DrawScore(bool show)
{
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		if (m_score[i] != NULL) {
			m_score[i]->SetState(show);
		}
	}
}

void UIMgr::ShowScore()
{
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		//描画データを作る
		DrawData* uiScore_drawData = new DrawData();
		uiScore_drawData->SetSize(vector2s(100.0f, 100.0f));
		uiScore_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\num.png"));
		uiScore_drawData->tex_w_count = TEX_NUM_COUNT_W;
		uiScore_drawData->tex_texel_size = num_tex_size;

		if (i == 0) {
			uiScore_drawData->SetShaderIntParam(3, m_number / 10000);
		}
		else if (i == 1) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 10000 / 1000);
		}
		else if (i == 2) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 1000 / 100);
		}
		else if (i == 3) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 100 / 10);
		}
		else if (i == 4) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 10);
		}

		m_score[i]->SetDrawData(uiScore_drawData);//描画データを渡す
		//m_score[i]->SetState(false);//非表示にする
	}
}

void UIMgr::ResetLifeUI()
{
	PlyerLife_before = 100;
	HpDecrease = 0;
	//プレイヤーのライフの取得
	//playerLife = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();
	for (int i = 0; i < UI_LIFE_NUM; i++) {
		DrawData* UiLife = uiLife[i]->GetDrawData();
		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		uiLife[i]->SetDrawData(UiLife);
		//Life_Drawflag[i] = true;
		//DrawData* UiLife = uiLife[i]->GetDrawData();

		//int LifePerUI = PLAYER_HP / UI_LIFE_NUM; // おにぎり1つ当たりのHP

		//if (playerLife < LifePerUI * (i + 1))
		//{
		//	int LifeExcess = playerLife - (i * LifePerUI); //欠けているおにぎりの部分のライフ量

		//	if (LifeExcess >= LifePerUI)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4) * 3)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP2.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4) * 2)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP3.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4))
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP4.png"));
		//	}
		//	if (LifeExcess <= 0)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP5.png"));
		//	}

		//	uiLife[i]->SetDrawData(UiLife);
		//}
	}
}

#pragma endregion ui_manager



#pragma region game_state

GameState::GameState() :gameStateType(E_GameState::GameState_None)
{

}

GameState::GameState(E_GameState state) : gameStateType(state) {

}

GameState::~GameState() {

}



void GameState::DoUpdate(float deltatime) {

}

//================================================================================
//追加松尾
#pragma region game_state_logo

GameState_Logo::GameState_Logo() :GameState(E_GameState::GameState_Game_Logo)
{

}

GameState_Logo::~GameState_Logo() {

}

void GameState_Logo::DoUpdate(float deltatime)
{
	GameMgr::GetInstance()->uiMgr->DoUpdate(deltatime);

}

void GameState_Logo::DoInit()
{

	GameMgr::GetInstance()->uiMgr->DrawTeamLogo(true);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Logo);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);
	GameMgr::GetInstance()->uiMgr->ShowLogoMenu(true);
	GameMgr::GetInstance()->effectMgr->HideSakura();

}

void GameState_Logo::Uninit()
{
	GameMgr::GetInstance()->uiMgr->teamLogo->SetState(false);
	GameMgr::GetInstance()->uiMgr->DrawTeamLogo(false);
	GameMgr::GetInstance()->uiMgr->ShowLogoMenu(false);
}

#pragma endregion game_state_logo

//================================================================================



#pragma region game_state_title

GameState_Title::GameState_Title() :GameState(E_GameState::GameState_Game_Title)
{

}

GameState_Title::~GameState_Title() {

}

void GameState_Title::DoUpdate(float deltatime)
{
	//BaseCell* tempCell_1 = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
	//Ground* ground_1 = (Ground*)tempCell_1;

	//BaseCell* tempCell_2 = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
	//Ground* ground_2 = (Ground*)tempCell_2;

	//tempCell_1->DeleteCell();
	//tempCell_2->DeleteCell();
}

void GameState_Title::DoInit()
{
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Title);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);
	GameMgr::GetInstance()->uiMgr->ShowTitle(true);
	GameMgr::GetInstance()->effectMgr->ShowSakura();
	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_title, -1);
}

void GameState_Title::Uninit()
{
	GameMgr::GetInstance()->uiMgr->ShowTitle(false);
	GameMgr::GetInstance()->effectMgr->HideSakura();

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_title);
}

#pragma endregion game_state_title

#pragma region game_state_game

GameState_Game::GameState_Game() {

}

GameState_Game::~GameState_Game() {

}

void GameState_Game::DoUpdate(float deltatime) {
	////=============早川追加===================
	GameMgr::GetInstance()->uiMgr->DoUpdate();
	//GameMgr::GetInstance()->uiMgr->DrawLife();
	//GameMgr::GetInstance()->uiMgr->DrawUIBullet();
	////========================================

	//	//bar処理　　byヨウ
	////===========================================
	//GameMgr::GetInstance()->uiMgr->UpdateBar();
	//GameMgr::GetInstance()->uiMgr->DrawBar(true);
	////===========================================

	//	//score===========
	////===========================================
	//GameMgr::GetInstance()->uiMgr->UpdateScore();
	//GameMgr::GetInstance()->uiMgr->DrawScore(true);
	////===========================================
}

void GameState_Game::DoInit() {
	//load game scene
	//active spawner
	//switch input state
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(true);
	GameMgr::GetInstance()->uiMgr->SetEnemyCounter(0);

	GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_TestScene, true,true);//シーンを切り替える

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);//入力モードを切り替える
	GameMgr::GetInstance()->inputMgr->SetInputState(true);//入力スイッチをオンにするr

	//23.1
	GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	GameMgr::GetInstance()->uiMgr->SetScoreNum(0);

	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_GamePlay, -1);

}

void GameState_Game::Uninit() {
	//unload all unit(unload scene
	//inactive spawner
	//MainCamera::GetInstance()->ResetCamPos();
	//MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_None, true,false);//シーンの終了処理
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_None);//入力モードを切り替える
	
	//23.1
	GameMgr::GetInstance()->uiMgr->ShowHUD(false);

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GamePlay);

}

#pragma endregion game_state_game

#pragma region game_state_menu


GameState_Menu::GameState_Menu() {

}

GameState_Menu::~GameState_Menu() {

}

void GameState_Menu::DoUpdate(float deltatime) {

}

void GameState_Menu::DoInit() {

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Menu);//入力モードを切り替える
	GameMgr::GetInstance()->inputMgr->SetInputState(true);//入力スイッチをオンにするr

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);//ゲームを一時停止するr
}

void GameState_Menu::Uninit() {

	//return to game input
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);//入力モードを切り替える

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);//ゲームを再開する
}

#pragma endregion game_state_menu

#pragma region game_state_lose

GameState_Lose::GameState_Lose() {

}

GameState_Lose::~GameState_Lose() {

}

void GameState_Lose::DoUpdate(float deltatime) {

}

void GameState_Lose::DoInit() {
	//set timer
	//load lose panel
	//set input state
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Lose_Menu);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);

	GameMgr::GetInstance()->uiMgr->ShowGameOver(true);
	GameMgr::GetInstance()->sceneObjMgr->UninitPlayerObjState();

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);

	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_GameOver, -1);
}

void GameState_Lose::Uninit() {
	GameMgr::GetInstance()->sceneMgr->ResetCurScene();
	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);

	GameMgr::GetInstance()->uiMgr->ShowGameOver(false);

	//GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_TestScene, true, true);//シーンを切り替える
	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GameOver);
}

#pragma endregion game_state_lose

#pragma region game_state_end

GameState_End::GameState_End() {

}

GameState_End::~GameState_End() {

}

void GameState_End::DoUpdate(float deltatime) {

}

void GameState_End::DoInit() {
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Win_Menu);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);

	GameMgr::GetInstance()->uiMgr->ShowHUD(false);

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);
	GameMgr::GetInstance()->uiMgr->ShowEndMenu(true);

}

void GameState_End::Uninit() {

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);
	GameMgr::GetInstance()->uiMgr->ShowEndMenu(false);

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GameClear);
}

#pragma endregion game_state_end

#pragma endregion game_state

#pragma region game_manager

GameMgr::GameMgr() :curGameStateType(E_GameState::GameState_None), defaultGameStateType(E_GameState::GameState_None),
defaultSubGameStateType(E_GameState::GameState_None), curSubGameStateType(E_GameState::GameState_None),
curGameState(NULL), curSubGameState(NULL) {
	//インスタンス化
	sceneMgr = new SceneMgr();
	inputMgr = new InputMgr();
	sceneObjMgr = new SceneObjMgr();
	effectMgr = new EffectMgr();
	uiMgr = new UIMgr();
	timer = new Timer();
	dataMgr = new DataManager();

	gameStateDic.clear();
	subGameStateDic.clear();
}

GameMgr::~GameMgr() {
	if (sceneMgr != NULL) {
		delete sceneMgr;
	}
	if (inputMgr != NULL) {
		delete inputMgr;
	}
	if (sceneObjMgr != NULL) {
		delete sceneObjMgr;
	}
	if (effectMgr != NULL) {
		delete effectMgr;
	}
	if (timer != NULL) {
		delete timer;
	}
	if (dataMgr != NULL) {
		delete dataMgr;
	}
	sceneMgr = NULL;
	inputMgr = NULL;
	sceneObjMgr = NULL;
	effectMgr = NULL;
	timer = NULL;
	dataMgr = NULL;
	//ゲームステートを削除する
	map<E_GameState, GameState*>::iterator iter;
	for (iter = gameStateDic.begin(); iter != gameStateDic.end(); iter++) {
		if (iter->second == NULL)continue;
		delete iter->second;
	}
	for (iter = subGameStateDic.begin(); iter != subGameStateDic.end(); iter++) {
		if (iter->second == NULL)continue;
		delete iter->second;
	}

	gameStateDic.clear();
	subGameStateDic.clear();
}

void GameMgr::DoInit() {

	//ゲームステートをリストに追加する

	//===============================
	//追加松尾
	RegisterGameState(E_GameState::GameState_Game_Logo, new GameState_Logo(), true);
	//
	//==============
	RegisterGameState(E_GameState::GameState_Game_Title, new GameState_Title(), false);
	RegisterGameState(E_GameState::GameState_Game_Normal, new GameState_Game(), false);
	RegisterGameState(E_GameState::GameState_Game_End, new GameState_End(), false);

	RegisterSubGameState(E_GameState::GameState_Game_Menu, new GameState_Menu(), false);
	//RegisterSubGameState(E_GameState::GameState_Game_Lose, new GameState_Lose(), false);
	RegisterGameState(E_GameState::GameState_Game_Lose, new GameState_Lose(), false);
	//RegisterSubGameState(E_GameState::GameState_Game_End, new GameState_End(), false);

	//init all manager
	timer->Reset();

	sceneObjMgr->DoInit();//一回しか初期化しない

	effectMgr->DoInit();//一回しか初期化しない

	sceneMgr->DoInit();//一回しか初期化しない

	uiMgr->DoInit();//一回しか初期化しない

	dataMgr->DoInit();

	//デフォルトゲームステート　／デフォルトサブゲームステート　に切り替える
	SwitchGameState(defaultGameStateType, true);
	SwitchSubGameState(defaultSubGameStateType, true);
}

void GameMgr::DoUninit() {
	sceneMgr->UnInit();
	effectMgr->UnInit();
	sceneObjMgr->UnInit(true);
}

/// <summary>
/// 全体のアップデート処理の出発点
/// </summary>
/// <param name="deltatime"></param>
void GameMgr::DoUpdate(float deltatime) {
	inputMgr->DoUpdate(deltatime);//入力アップデート
	effectMgr->DoUpdate(deltatime * timer->gameTimer_1);//エフェクトアップデート
	sceneObjMgr->DoUpdate(deltatime);//ゲームオブジェクトアップデート
	//sceneObjMgr->DoLateUpdate(deltatime * timer->uiTimer);
	MainCamera::GetInstance()->DoUpdate(deltatime * timer->gameTimer_1);
	sceneMgr->DoUpdate(deltatime * timer->gameTimer_1);//シーンアップデート
	timer->DoUpdate(deltatime);

	if (curGameState != NULL) {
		curGameState->DoUpdate(deltatime);//ゲームステートのアップデート
	}
	if (curSubGameState != NULL) {
		curSubGameState->DoUpdate(deltatime);//サブゲームステートのアップデート
	}
}

void GameMgr::DoLateUpdate(float deltatime) {

}

/// <summary>
/// ゲームステートを切り替える
/// </summary>
/// <param name="gameState">　次のステート</param>
/// <param name="reset">　同じステートの場合、もう一度初期化するかどうか</param>
void GameMgr::SwitchGameState(E_GameState gameState, bool reset)
{
	if (gameState == E_GameState::GameState_None) {
		if (curGameState != NULL) {
			curGameState->Uninit();
			curGameState = NULL;
		}
		curGameStateType = E_GameState::GameState_None;
		return;
	}

	if (gameState == curGameStateType) {//次のステートが今のステートと同じであれば
		if (reset == false) {
			return;
		}
		else {
			if (curGameState != NULL) {
				curGameState->Uninit();
				curGameState->DoInit();
			}
			return;
		}
	}

	if (curGameState != NULL) {
		curGameState->Uninit();//今のステートの終了処理
		curGameState = NULL;
	}

	if (gameStateDic.find(gameState) == gameStateDic.end()) {//次のステートが見つからない場合
		return;
	}

	curGameState = gameStateDic[gameState];
	if (curGameState != NULL) {
		curGameState->DoInit();//次のステートを初期化する
		curGameStateType = gameState;
	}
}

/// <summary>
/// サブゲームステートを切り替える
/// </summary>
/// <param name="subGameState">　次のステート</param>
/// <param name="reset">　同じステートの場合、もう一度初期化するかどうか</param>
void GameMgr::SwitchSubGameState(E_GameState subGameState, bool reset)
{
	if (subGameState == E_GameState::GameState_None) {
		if (curSubGameState != NULL) {
			curSubGameState->Uninit();
			curSubGameState = NULL;
		}
		curSubGameStateType = E_GameState::GameState_None;
		return;
	}

	if (subGameState == curSubGameStateType) {//次のステートが今のステートと同じであれば
		if (reset == false) {
			return;
		}
		else {
			if (curSubGameState != NULL) {
				curSubGameState->Uninit();
				curSubGameState->DoInit();
			}
			return;
		}
	}

	if (curSubGameState != NULL) {
		curSubGameState->Uninit();//今のステートの終了処理
		curSubGameState = NULL;
	}

	if (subGameStateDic.find(subGameState) == subGameStateDic.end()) {//次のステートが見つからない場合
		return;
	}

	curSubGameState = subGameStateDic[subGameState];
	if (curSubGameState != NULL) {
		curSubGameState->DoInit();//次のステートを初期化する
		curSubGameStateType = subGameState;
	}
}

/// <summary>
/// 新しいステートを追加する
/// </summary>
/// <param name="gameStateType"></param>
/// <param name="gameState"></param>
/// <param name="setDefault"></param>
void GameMgr::RegisterGameState(E_GameState gameStateType, GameState* gameState, bool setDefault)
{
	if (gameState == NULL)return;
	if (gameStateDic.find(gameStateType) == gameStateDic.end()) {
		gameStateDic[gameStateType] = gameState;
		if (setDefault) {
			defaultGameStateType = gameStateType;
		}
	}
	else {
		//error
	}
}

/// <summary>
/// 新しいサブステートを追加する
/// </summary>
/// <param name="gameStateType"></param>
/// <param name="gameState"></param>
/// <param name="setDefault"></param>
void GameMgr::RegisterSubGameState(E_GameState gameStateType, GameState* gameState, bool setDefault)
{
	if (gameState == NULL)return;
	if (subGameStateDic.find(gameStateType) == subGameStateDic.end()) {
		subGameStateDic[gameStateType] = gameState;
		if (setDefault) {
			defaultSubGameStateType = gameStateType;
		}
	}
	else {
		//error
	}
}

/// <summary>
/// 今のステートを取得する
/// </summary>
/// <returns></returns>
E_GameState GameMgr::GetCurGameState()
{
	return curGameStateType;
}

/// <summary>
/// 今のサブステートを取得する
/// </summary>
/// <returns></returns>
E_GameState GameMgr::GetCurSubGameState() {
	return curSubGameStateType;
}

#pragma endregion game_manager

#pragma region timer_method

Timer::Timer() {
	Reset();
}

Timer::~Timer() {

}

void Timer::SetTimer(E_Timer timerType, bool limitForward) {
	switch (timerType) {
	case E_Timer::Timer_Game_1:
		gameTimer_1 = limitForward ? 1.0f : 0.0f;
		break;
	case E_Timer::Timer_Game_2:
		gameTimer_2 = limitForward ? 1.0f : 0.0f;
		break;
	case E_Timer::Timer_UI:
		uiTimer = limitForward ? 1.0f : 0.0f;
		break;
	default:
		break;
	}
}

void Timer::SetTimer(E_Timer timerType, float value)
{
	switch (timerType) {
	case E_Timer::Timer_Game_1:
		gameTimer_1 = value;
		break;
	case E_Timer::Timer_Game_2:
		gameTimer_2 = value;
		break;
	case E_Timer::Timer_UI:
		uiTimer = value;
		break;
	default:
		break;
	}
}

void Timer::Reset()
{
	gameTimer_1 = 1.0f;
	gameTimer_2 = 1.0f;
	uiTimer = 1.0f;
}

void Timer::DoUpdate(float deltatime)
{
}

#pragma endregion timer_method

//AI有限オートマトン
#pragma region fsm

FSMSystem::FSMSystem() :curState(NULL), parent(NULL),curStateType(E_FSMState::FSMState_Idle),defaultStateType(E_FSMState::FSMState_Idle) {

}

FSMSystem::FSMSystem(BaseCell* p) : parent(p), curState(NULL), curStateType(E_FSMState::FSMState_Idle), defaultStateType(E_FSMState::FSMState_Idle) {

}

FSMSystem::~FSMSystem() {
	if (stateDic.size() != 0) {
		map<E_FSMState, FSMState*>::iterator iter;
		for (iter = stateDic.begin(); iter != stateDic.end(); iter++) {
			if (iter->second != NULL) {
				delete iter->second;
			}
		}
	}
	stateDic.clear();

	curState = NULL;
}


void FSMSystem::DoInit() {
	//register state
	SwitchState(defaultStateType, true);
}

void FSMSystem::RegisterState(E_FSMState stateType, FSMState* state, bool setDefaultState) {
	if (state == NULL)return;

	if (stateDic.find(stateType) != stateDic.end()) {
		//error
		return;
	}

	stateDic[stateType] = state;

	if (setDefaultState) {
		defaultStateType = stateType;
	}
}

void FSMSystem::DoUpdate(float deltatime) {

	if (curState != NULL) {
		curState->DoUpdate(deltatime);
	}

}

void FSMSystem::SwitchState(E_FSMState targetStateType, bool resetState) {
	if (curStateType == targetStateType && resetState == false) {
		return;
	}

	if (stateDic.find(targetStateType) == stateDic.end()) {
		return;
	}

	if (curState != NULL) {
		curState->EndState();
		curState = NULL;
	}

	curState = stateDic[targetStateType];

	if (curState != NULL) {
		curState->DoInit();
		curState->StartState();
	}
}

void FSMSystem::SetParent(BaseCell* p)
{
	parent = p;
}

BaseCell* FSMSystem::GetParent() {
	return parent;
}


//state
FSMState::FSMState() :parentSystem(NULL), action(NULL), enable(false),stateType(E_FSMState::FSMState_Idle) {
	triggerDic.clear();
}

void FSMState::CheckStateTrigger(float deltatime) {
	if (triggerDic.size() == 0) {
		return;
	}

	map<E_FSMState, FSMTriggerGroup*>::iterator iter;
	for (iter = triggerDic.begin(); iter != triggerDic.end(); iter++) {
		if (iter->second == NULL)continue;
		if (iter->second->CheckTrigger(deltatime) == true) {
			//switch state
			if (parentSystem != NULL) {
				this->parentSystem->SwitchState(iter->first, true);
				return;
			}
		}
	}
}

FSMState::FSMState(FSMSystem* parent) :action(NULL), parentSystem(parent), enable(false), stateType(E_FSMState::FSMState_Idle) {

}

FSMState::~FSMState() {
	action = NULL;
}

void FSMState::DoInit() {
	map<E_FSMState, FSMTriggerGroup*>::iterator iter;
	for (iter = triggerDic.begin(); iter != triggerDic.end(); iter++) {
		if (iter->second == NULL)continue;
		iter->second->ResetTriggerState();
	}

	if (action != NULL) {
		(*action).Reset();
	}
}

void FSMState::DoUpdate(float deltatime) {
	if (enable == true) {
		DoAction(deltatime);

		//check state trigger
		CheckStateTrigger(deltatime);
	}
}

void FSMState::SetAction(FSMStateAction* a)
{
	if (a == NULL)return;
	action = a;
}

void FSMState::DoAction(float deltatime) {
	if (action != NULL) {
		(*action)(parentSystem->GetParent(), deltatime);
	}
}


void FSMState::StartState() {
	//start cb

	enable = true;
}

void FSMState::EndState() {
	//end cb

	enable = false;
}

void FSMState::PauseState(bool state)
{
	enable = state;
}

BaseCell* FSMState::GetParentCell() {
	if (parentSystem == NULL) {
		return NULL;
	}

	return parentSystem->GetParent();
}

void FSMState::RegisterTrigger(E_FSMState targetState, FSMTriggerGroup* triggerGroup)
{
	if (triggerGroup == NULL)return;

	if (triggerDic.find(targetState) == triggerDic.end()) {
		triggerDic[targetState] = triggerGroup;
	}
}


//trigger group
FSMTriggerGroup::FSMTriggerGroup() :checkTriggerType(E_CheckTriggerType::CheckTriggerType_AllTrue), parentSystem(NULL) {

}

FSMTriggerGroup::FSMTriggerGroup(E_CheckTriggerType checkType, FSMSystem* parent) : checkTriggerType(checkType), parentSystem(parent) {

}

FSMTriggerGroup::~FSMTriggerGroup()
{
	if (triggerArray.size() != 0) {
		vector<FSMStateTrigger*>::iterator iter;
		for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
			if ((*iter) != NULL) {
				delete (*iter);
			}
		}
	}
	triggerArray.clear();
}

E_CheckTriggerType FSMTriggerGroup::GetCheckType() {
	return checkTriggerType;
}

vector<FSMStateTrigger*>& FSMTriggerGroup::GetTriggerArray() {
	return triggerArray;
}

void FSMTriggerGroup::AddTrigger(FSMStateTrigger* trigger) {
	if (trigger == NULL)return;
	triggerArray.push_back(trigger);
}

bool FSMTriggerGroup::CheckTrigger(float deltatime) {
	if (triggerArray.size() == 0) {
		return false;
	}
	int falseSign = 0;
	int trueSign = 0;
	int allFalseSign = 1;
	int allTrueSign = 1;
	bool tempSign = 0;
	int tempSignInt = 0;
	vector<FSMStateTrigger*>::iterator iter;

	for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		tempSign = (*iter)->Check(parentSystem, deltatime);
		tempSignInt = tempSign == true ? 1 : 0;

		trueSign |= tempSignInt;
		allTrueSign &= tempSignInt;

		falseSign |= (~tempSignInt) & 1;
		allFalseSign &= (~tempSignInt) & 1;
	}

	switch (checkTriggerType) {
	case E_CheckTriggerType::CheckTriggerType_OneTrue:
		return trueSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_OneFalse:
		return falseSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AllTrue:
		return allTrueSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AllFalse:
		return allFalseSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AlwaysTrue:
		return true;
	case E_CheckTriggerType::CheckTriggerType_AlwaysFalse:
		return false;
	default:
		return false;
	}
	return true;
}

void FSMTriggerGroup::ResetTriggerState()
{
	vector<FSMStateTrigger*>::iterator iter;
	for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		(*iter)->ResetTriggerState();
	}
}


//trigger

FSMStateTrigger::FSMStateTrigger() :parentState(NULL), triggerAction(NULL)
{

}

FSMStateTrigger::~FSMStateTrigger()
{
	parentState = NULL;
	if (triggerAction != NULL) {
		delete triggerAction;
	}
	triggerAction = NULL;
}

void FSMStateTrigger::SetTriggerParam(FSMState* parent, FSMStateTriggerAction* action) {
	if (parent == NULL || action == NULL)return;

	parentState = parent;
	triggerAction = action;
}

bool FSMStateTrigger::Check(FSMSystem* parentSystem, float deltatime) {
	if (parentSystem == NULL)return false;

	//judge
	if (triggerAction == NULL || parentState == NULL)return false;

	return (*triggerAction)(parentState->GetParentCell(), deltatime);
}

void FSMStateTrigger::ResetTriggerState()
{
	if (triggerAction != NULL) {
		triggerAction->Reset();
	}
}

#pragma endregion fsm

#pragma region ai

#pragma region state_action

FSMStateAction::FSMStateAction() :init(false), setAnim(E_Anim::Anim_None),actionDone(false)
{

}

void FSMStateAction::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL) {
		return;
	}

	//do action
}

void FSMStateAction::Reset()
{
	init = false;
}
#pragma region enemy_base_action

void FSMState_Base_Action_Idle::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		parentCell->SetMovementState(E_MovementType::MovementType_None, false,true);
		init = true;
		actionDone = false;
	}
}

void FSMState_Base_Action_Move2Player::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		init = true;
		actionDone = false;
	}

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player == NULL)return;

	vector3s moveDir = player->GetTransform()->GetPos() - parentCell->GetTransform()->GetPos();
	moveDir.z = 0.0f;

	moveDir = VecNormalize(moveDir);
	moveDir = moveDir * (moveSpeed * deltatime);
	vector3s newPos = parentCell->GetTransform()->GetPos() + moveDir;
	parentCell->SetPos(newPos, true);

}

void FSMState_Base_Action_Dmg::operator()(BaseCell* parentCell, float delattime) {
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		init = true;
		actionDone = false;
	}
}

void FSMState_Base_Action_Partrol::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		parentCell->StartMovement(E_MovementType::MovementType_None,false,true);
		init = true;
		actionDone = false;
	}

}


#pragma endregion enemy_base_action

#pragma endregion state_action

#pragma region trigger_action

bool FSMStateTriggerAction::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL)return false;

	return false;
}

void FSMStateTriggerAction::Reset()
{
	init = false;
}

//idle time
bool FSMStateTrigger_Base_Trigger_IdleTime::operator()(BaseCell* parentCell, float deltatime)
{
	if (init == false) {
		timePass = 0.0f;
		init = true;
	}

	timePass += deltatime;
	if (timePass > idleTime) {
		return true;
	}

	return false;
}

//dist 2 player
bool FSMStateTrigger_Base_Trigger_Dist2Player::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return false;
	if (init == false) {
		init = true;
	}

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player == NULL)return false;

	vector3s cell2Player = player->GetTransform()->GetPos() - parentCell->GetTransform()->GetPos();
	cell2Player.z = 0.0f;

	float curDist = VecLengthS(cell2Player);
	if (curDist < dist * dist) {
		return true;
	}

	return false;
}

//dmg time
bool FSMStateTrigger_Base_Trigger_TakeDmg::operator()(BaseCell* parentCell, float deltatime)
{
	if (init == false) {
		init = true;
	}

	if (parentCell == NULL) {
		return true;
	}

	//if (parentCell->CheckCanMove()) {
	//	return false;
	//}
	//else {
	//	return true;
	//}

	return true;
}


#pragma endregion trigger_action


#pragma endregion ai

