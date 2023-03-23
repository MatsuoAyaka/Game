#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "MapGenerator.h"
#include "Item.h"
#include "Camera.h"
class Onigiri;

#pragma region scene

Scene::Scene() :sceneType(E_Scene::Scene_None), nextSceneType(E_Scene::Scene_None), defaultPlayerPos(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f),
 isSceneActive(false)
{
	curStageDic.clear();
	sceneObjDic.clear();
}

Scene::Scene(E_Scene sceneT) : sceneType(sceneT), nextSceneType(E_Scene::Scene_None),  defaultPlayerPos(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f),
 isSceneActive(false)
{
	curStageDic.clear();
	sceneObjDic.clear();
	ground = NULL;
}

Scene::~Scene() {
	//シーンの中のすべてのゲームオブジェクトを削除する
	UnregisterSceneObj(NULL, true);
	UnregisterMiniStage(NULL, true);
	ground = NULL;
}

void Scene::RegisterSceneObj(BaseCell* cell)
{
	if (cell == NULL)return;

	if (sceneObjDic.find(cell->GetID()) == sceneObjDic.end()) {
		sceneObjDic[cell->GetID()] = cell;
	}

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(cell, true);//SceneObjMgrに保存してもらう
}

void Scene::UnregisterSceneObj(BaseCell* cell, bool unregisterAll)
{
	if (unregisterAll == true) {
		map<int, BaseCell*>::iterator iter;
		for (iter = sceneObjDic.begin(); iter != sceneObjDic.end(); iter++) {
			if (iter->second == NULL)continue;
			//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(iter->second);
			iter->second->DeleteCell();
		}

		sceneObjDic.clear();
	}
	else {
		if (cell == NULL || sceneObjDic.size() == 0) return;

		map<int, BaseCell*>::iterator iter = sceneObjDic.find(cell->GetID());
		if (iter != sceneObjDic.end()) {
			//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(cell);//SceneObjMgrから削除する
			cell->DeleteCell();
			sceneObjDic.erase(iter);
		}
	}
}

void Scene::RegisterMiniStage(MiniStage* stage)
{
	if (stage == NULL)return;
	if (curStageDic.find(stage->GetStageID()) != curStageDic.end()) {
		return;//error
	}
	else {
		curStageDic[stage->GetStageID()] = stage;
		SetFrontStage(stage);
	}
}

void Scene::UnregisterMiniStage(MiniStage* stage, bool unregisterAll)
{
	if (unregisterAll == true) {
		map<int, MiniStage*>::iterator iter;
		for (iter = curStageDic.begin(); iter != curStageDic.end(); iter++) {
			if (iter->second == NULL)continue;
			GameMgr::GetInstance()->sceneMgr->stageMgr->RecycleStage(iter->second);
		}

		curStageDic.clear();
		m_frontStage = NULL;
		//temp
		GameMgr::GetInstance()->sceneMgr->stageMgr->ClearStage();
	}
	else {
		if (stage == NULL || curStageDic.size() == 0) return;
		
		map<int, MiniStage*>::iterator iter = curStageDic.find(stage->GetStageID());
		if (iter != curStageDic.end()) {
			curStageDic.erase(iter);
		}
		GameMgr::GetInstance()->sceneMgr->stageMgr->RecycleStage(stage);
	}
}

void Scene::DoUpdate(float deltatime) {
	int a = GameMgr::GetInstance()->sceneMgr->stageMgr->stageCounter;
	if (isSceneActive == true) {
		Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
		if (player != NULL && player->CheckState()==true) {
			vector3s playerPos = player->GetTransform()->GetPos();

			//create new stage judge
			
			//if (player->CurStage == m_frontStage || m_frontStage == NULL) {
			//	MiniStage* newStage = GameMgr::GetInstance()->sceneMgr->stageMgr->CreateStageRandom();
			//	RegisterMiniStage(newStage);
			//}
			
			//プレイヤーと最前列のステージとの距離によって前方のステージを生成する
			if (m_frontStage != NULL) {
				if (playerPos.x > m_frontStage->GetStagePos().x + STAGE_SIZE_X * 0.45f) {
 					MiniStage* newStage = GameMgr::GetInstance()->sceneMgr->stageMgr->CreateStageRandom();
					newStage->ShowStage(true);
					RegisterMiniStage(newStage);
				}
			}

			//プレイヤーとの距離によって後方のステージを削除する
			map<int, MiniStage*>::iterator iter;
			for (iter = curStageDic.begin(); iter != curStageDic.end(); iter++) {
				if (iter->second == NULL) {
					continue;
				}

				if (iter->second->GetStagePos().x + STAGE_SIZE_X * 3.5f < playerPos.x) {
					//GameMgr::GetInstance()->sceneMgr->stageMgr->RecycleStage(iter->second);
					//curStageDic.erase(iter);
					this->UnregisterMiniStage(iter->second,false);
					break;
				}
				else {
					iter->second->ShowStage(true);
				}

			}
		}
	}
}

void Scene::DoLateUpdate(float deltatime) {

}

void Scene::Load() {
	OnSceneLoad();
	isSceneActive = true;
}

void Scene::Unload() {
	//if (sceneObjDic.size() != 0) {
	//	map<int, BaseCell*>::iterator iter;
	//	for (iter = sceneObjDic.begin(); iter != sceneObjDic.end(); iter++) {
	//		GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(iter->second);//unregister from SceneObjMgr
	//		if (iter->second != NULL) {
	//			delete iter->second;
	//		}
	//	}
	//	sceneObjDic.clear();
	//}

	UnregisterSceneObj(NULL, true);
	UnregisterMiniStage(NULL, true);
	OnSceneUnload();
	isSceneActive = false;
}

void Scene::Reset()
{
	OnSceneUnload();
	if (sceneObjDic.size() != 0) {
		map<int, BaseCell*>::iterator iter;
		for (iter = sceneObjDic.begin(); iter != sceneObjDic.end(); iter++) {
			if (iter->second != NULL) {
				iter->second->Reset();
			}
		}
	}
	OnSceneLoad();
}

void Scene::OnSceneLoad() {
	GameMgr::GetInstance()->sceneObjMgr->DoInitCell();//ゲームオブジェクトの初期化
	GameMgr::GetInstance()->sceneObjMgr->InitPlayerObjState(defaultPlayerPos);
}

void Scene::OnSceneUnload() {
	ground = NULL;
	GameMgr::GetInstance()->sceneObjMgr->UninitPlayerObjState();
}

void Scene::SetSceneActive(bool state)
{
	if (state == isSceneActive)return;
	isSceneActive = state;
	//TODO
}

void Scene::SetNextScene(E_Scene nextScene) {
	nextSceneType = nextScene;
}

E_Scene Scene::GetNextScene()
{
	return nextSceneType;
}

E_Scene Scene::GetSceneType()
{
	return sceneType;
}

void Scene::SetDefaultPlayerPos(vector3s pos)
{
	defaultPlayerPos = pos;
}

vector3s Scene::GetDefaultPos()
{
	return defaultPlayerPos;
}

void Scene::SetFrontStage(MiniStage* _stage)
{
	m_frontStage = _stage;
}

void Scene::SetGround(Ground* g,E_EnemyType initEnemyType)
{
	ground = g;
	return;
	if (ground != NULL) {

		if (initEnemyType == E_EnemyType::Enemy_Monkey) {

			vector3s pos = ground->GetTransform()->GetPos();//ブロックの位置（ブロックの真ん中）
			vector2s size = ground->GetSize(true);
			pos.y -= size.y - 5.0f;//ブロックの上

			//NPC
			//1
			Monkey* npc_1 = new Monkey();
			npc_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
			npc_1->SetPos(pos, true);
			npc_1->SetRot(vector3s(180.0f, 0.0f, 0.0f));
			DrawData* npc_1_drawData = new DrawData();
			npc_1_drawData->shakeWithCamera = true;
			npc_1_drawData->SetSize(character_size);
			npc_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
			npc_1_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
			npc_1_drawData->tex_w_count = NPC_TEX_W_COUNT;
			npc_1_drawData->tex_texel_size = character_cell_tex_size;
			npc_1_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
			npc_1->SetDrawData(npc_1_drawData);
			Collider* npc_1_collider = new Collider();
			ColliderData* npc_1_colliderData =
				new ColliderData(0, npc_1_collider, false, E_ColliderType::Collider_Rect, npc_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
			npc_1_collider->AddColliderData(npc_1_colliderData);
			npc_1->RegisterComponent(E_Component::Component_Collider, npc_1_collider, true);
			//animation
			Animation* npc_1_AnimComponent = new Animation();
			if (npc_1->RegisterComponent(E_Component::Component_Animation, npc_1_AnimComponent) == false) {//register component first
				delete npc_1_AnimComponent;
				npc_1_AnimComponent = NULL;
			}
			else {
				//normal
				AnimClip* npc_1_IdleAnim = new AnimClip();
				int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
				int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
				npc_1_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
				npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_1_IdleAnim, true);
				AnimClip* npc_1_MoveAnim = new AnimClip();
				startTexIndex = NPC_MOVE_INDEX_Y * NPC_TEX_W_COUNT + NPC_MOVE_INDEX_X;
				endTexIndex = startTexIndex + NPC_MOVE_FRAME_COUNT;
				npc_1_MoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
				npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Move, npc_1_MoveAnim, false);
			}


			this->RegisterSceneObj(npc_1);
		}
	}
}



#pragma region test_scene

TestScene::TestScene() :Scene(E_Scene::Scene_TestScene) {

}

TestScene::~TestScene() {

}

void TestScene::Load() {
	Unload();

	//default block
	MapGenerator::GetInstance()->CreateGround();
	if (this->ground != NULL) {
		vector3s curPos = vector3s(0, 0, 0);
		curPos = this->ground->GetTransform()->GetPos();
		curPos.y = curPos.y - this->ground->GetSize(true).y - 5.0f;
		defaultPlayerPos = curPos;
	}
	MapGenerator::GetInstance()->CreateGround();
	MapGenerator::GetInstance()->CreateGround();
	MapGenerator::GetInstance()->CreateGround();

	//MapGenerator::GetInstance()->CreateGround();
	//MapGenerator::GetInstance()->CreateGround();
	//MapGenerator::GetInstance()->CreateGround();

	//back ground
	BaseCell* bg = new TempBackground();
	vector3s pos = vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
	bg->SetPos(pos, true);
	bg->SetState(true);
	this->RegisterSceneObj(bg);

	MiniStage* newStage = GameMgr::GetInstance()->sceneMgr->stageMgr->CreateStageRandom();
	newStage->ShowStage(true);
	RegisterMiniStage(newStage);

	//TileData* tileData_sceneObj_redNormal = new TileData();
	//tileData_sceneObj_redNormal->cornerSize_1 = vector2s(24, 24);
	//tileData_sceneObj_redNormal->cornerSize_2 = vector2s(31, 98);
	//tileData_sceneObj_redNormal->borderTileType = E_TileType::TileType_Repeat;
	//tileData_sceneObj_redNormal->centerTileType = E_TileType::TileType_Stretch;
	//tileData_sceneObj_redNormal->texRawSize = vector2s(237.0f, 314.0f);
	//tileData_sceneObj_redNormal->cornerScale_1 = vector2s(0.5f, 0.5f);
	//tileData_sceneObj_redNormal->cornerScale_2 = vector2s(0.5f, 0.5f);
	//tileData_sceneObj_redNormal->centerTex = LoadTexture((char*)"data\\texture\\colony-export_4_4.png");
	//tileData_sceneObj_redNormal->centerTexSize = vector2s(182, 191);
	//tileData_sceneObj_redNormal->bottomBorderTex = LoadTexture((char*)"data\\texture\\colony-export_4_7.png");
	//tileData_sceneObj_redNormal->bottomBorderTexSize = vector2s(182, 98);
	//tileData_sceneObj_redNormal->topBorderTex = LoadTexture((char*)"data\\texture\\colony-export_4_1.png");
	//tileData_sceneObj_redNormal->topBorderTexSize = vector2s(182, 24);
	//tileData_sceneObj_redNormal->rightBorderTex = LoadTexture((char*)"data\\texture\\colony-export_4_5.png");
	//tileData_sceneObj_redNormal->rightBorderTexSize = vector2s(31, 191);
	//tileData_sceneObj_redNormal->leftBorderTex = LoadTexture((char*)"data\\texture\\colony-export_4_3.png");
	//tileData_sceneObj_redNormal->leftBorderTexSize = vector2s(24, 191);
	//tileData_sceneObj_redNormal->tb_repeatCount = vector2s(-1, -1);
	//tileData_sceneObj_redNormal->lr_repeatCount = vector2s(-1, -1);
	////tileData_sceneObj_redNormal->tb_texScale.y = 0.5f;
	//tileData_sceneObj_redNormal->tb_texScale = vector2s(0.5f, 0.5f);
	//tileData_sceneObj_redNormal->lr_texScale = vector2s(0.5f, 0.5f);

	vector2s obstacle_size = vector2s(0, 0);
#pragma region wall
	/*
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH * 0.5f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT*0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\colony-export_4.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(0, 0);
	wall_1_drawData->preComputeUVSize = vector2s(TEST_TEX_SIZE_X, TEST_TEX_SIZE_Y);
	wall_1_drawData->tex_texel_size = test_tex_size;
	//wall_1_drawData->SetColor(wall_color);
	wall_1_drawData->SetDrawDataType(E_DrawDataType::DrawData_Tile);
	wall_1_drawData->SetTileData(tileData_sceneObj_redNormal);
	wall_1->SetDrawData(wall_1_drawData);



	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);


	//2(wall_bottom
	obstacle_size.x = SCREEN_WIDTH - 300.0f;
	obstacle_size.y = 100.0f;
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_left_bottom
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//6(wall_scene_1
	obstacle_size.x = 200.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_6 = new Obstacle();
	wall_6->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_6->SetPos(vector3s(SCREEN_WIDTH - 400.0f, SCREEN_HEIGHT - obstacle_size.y * 0.5f - 100.0f, 0.0f), true);

	DrawData* wall_6_drawData = new DrawData();
	wall_6_drawData->shakeWithCamera = true;
	wall_6_drawData->SetSize(obstacle_size);
	wall_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_6_drawData->usePreComputeUV = true;
	wall_6_drawData->preComputeUV = vector2s(1, 1);
	wall_6_drawData->preComputeUVSize = vector2s(0, 0);
	wall_6_drawData->tex_texel_size = vector2s(1, 1);
	wall_6_drawData->SetColor(wall_color);
	wall_6->SetDrawData(wall_6_drawData);

	Collider* wall_6_collider = new Collider();
	ColliderData* wall_6_colliderData =
		new ColliderData(0, wall_6_collider, false, E_ColliderType::Collider_Rect, wall_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_6_collider->AddColliderData(wall_6_colliderData);

	wall_6->RegisterComponent(E_Component::Component_Collider, wall_6_collider, true);

	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 75.0f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(150.0f, 150.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);
	*/

#pragma endregion wall
	//this->RegisterSceneObj(wall_1);
	//this->RegisterSceneObj(wall_2);
	//this->RegisterSceneObj(wall_3);
	//this->RegisterSceneObj(wall_4);
	//this->RegisterSceneObj(wall_5);
	//this->RegisterSceneObj(wall_6);
	//this->RegisterSceneObj(targetPoint);

	

#pragma region enemy
	//実際に使ってみよう！
	//Enemy

	

#pragma endregion enemy


	Scene::Load();
}

void TestScene::Unload() {
	Scene::Unload();
	MapGenerator::GetInstance()->ClearList();
	//hide game obj

}

void TestScene::Reset() {
	Scene::Reset();
}

void TestScene::DoUpdate(float deltatime) {
	if (isSceneActive == false)return;
	//ブロックテスト
	//プレイヤーと前方の一番遠いブロックとの距離が画面の横のサイズの半分以下であれば、新しいブロックを生成する
	//if (ground != NULL) {
	//	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	//	if (player != NULL) {
	//		if (abs(ground->GetTransform()->GetPos().x - player->GetTransform()->GetPos().x) <= SCREEN_WIDTH * 0.5f) {
	//			int randomGroundValue = rand() % 30;
	//			if (randomGroundValue % 3 == 0) {
	//				MapGenerator::GetInstance()->CreateGround();
	//				MapGenerator::GetInstance()->CreateIce();
	//				vector3s itemPos = ground->GetTransform()->GetPos();
	//				itemPos.y -= (ground->GetSize(true).y * 0.5f + 50.0f);
	//				Onigiri* newItem = new Onigiri(itemPos,vector2s(100.0f,100.0f));
	//				this->RegisterSceneObj(newItem);
	//			}
	//			else {
	//				MapGenerator::GetInstance()->CreateGround();
	//			}
	//		}
	//	}
	//}
	//MapGenerator::GetInstance()->MapUpdate();


	int enemyCounter = GameMgr::GetInstance()->uiMgr->GetEnemyCounter();
	if (enemyCounter >= NEXT_STAGE_ENEMY_COUNT)
	{
		MainCamera::GetInstance()->ResetCamPos();
		GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_Boss, true, true);
		return;
	}
	Scene::DoUpdate(deltatime);
}

void TestScene::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void TestScene::OnSceneLoad() {
	GameMgr::GetInstance()->sceneObjMgr->InitCornetState(vector3s(-1000.0f,200.0f,0.0f));
	GameMgr::GetInstance()->uiMgr->DrawBar(true);
	GameMgr::GetInstance()->uiMgr->DrawScore(true);
	GameMgr::GetInstance()->uiMgr->ResetLifeUI();
	Scene::OnSceneLoad();
}

void TestScene::OnSceneUnload() {
	MapGenerator::GetInstance()->ClearList();
	GameMgr::GetInstance()->sceneObjMgr->UninitCornetState();
	GameMgr::GetInstance()->uiMgr->DrawBar(false);
	GameMgr::GetInstance()->uiMgr->DrawScore(false);
	Scene::OnSceneUnload();
}


#pragma endregion testScene

#pragma region scene_1

Scene_1::Scene_1() :Scene(E_Scene::Scene_Stage1)
{

}

Scene_1::~Scene_1() {

}

void Scene_1::DoUpdate(float deltatime) {

}

void Scene_1::DoLateUpdate(float deltatime) {

}


void Scene_1::Load() {
	Unload();

	vector2s obstacle_size = vector2s(0, 0);
#pragma region obstacle

#pragma region wall
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH;
	obstacle_size.y = 100.0f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(1, 1);
	wall_1_drawData->preComputeUVSize = vector2s(0, 0);
	wall_1_drawData->tex_texel_size = vector2s(1, 1);
	wall_1_drawData->SetColor(wall_color);
	wall_1->SetDrawData(wall_1_drawData);

	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);

	//2(wall_bottom
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_left_bottom
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//6(wall_right_bottom
	Obstacle* wall_6 = new Obstacle();
	wall_6->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_6->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_6_drawData = new DrawData();
	wall_6_drawData->shakeWithCamera = true;
	wall_6_drawData->SetSize(obstacle_size);
	wall_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_6_drawData->usePreComputeUV = true;
	wall_6_drawData->preComputeUV = vector2s(1, 1);
	wall_6_drawData->preComputeUVSize = vector2s(0, 0);
	wall_6_drawData->tex_texel_size = vector2s(1, 1);
	wall_6_drawData->SetColor(wall_color);
	wall_6->SetDrawData(wall_6_drawData);

	Collider* wall_6_collider = new Collider();
	ColliderData* wall_6_colliderData =
		new ColliderData(0, wall_6_collider, false, E_ColliderType::Collider_Rect, wall_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_6_collider->AddColliderData(wall_6_colliderData);

	wall_6->RegisterComponent(E_Component::Component_Collider, wall_6_collider, true);


	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(100.0f, 100.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);

#pragma endregion wall

#pragma region scene_obstacle

	//1
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(320.0f, 300.0f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(obstacle_size);
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1_drawData->SetColor(obstacle_color);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//movement
	Movement_Breakable* obstacle_1_breakable_movement = new Movement_Breakable(obstacle_1, true, false, false, WALL_HP, 0.0f);
	obstacle_1->AddMovement(obstacle_1_breakable_movement);


	//2
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(650.0f, 420.0f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(obstacle_size);
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2_drawData->SetColor(obstacle_color);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//movement
	Movement_Breakable* obstacle_2_breakable_movement = new Movement_Breakable(obstacle_2, true, false, false, WALL_HP, 0.0f);
	obstacle_2->AddMovement(obstacle_2_breakable_movement);


	//3
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_3 = new Obstacle();
	obstacle_3->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_3->SetPos(vector3s(980.0f, 300.0f, 0.0f), true);

	DrawData* obstacle_3_drawData = new DrawData();
	obstacle_3_drawData->shakeWithCamera = true;
	obstacle_3_drawData->SetSize(obstacle_size);
	obstacle_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_3_drawData->usePreComputeUV = true;
	obstacle_3_drawData->preComputeUV = vector2s(1, 1);
	obstacle_3_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_3_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_3_drawData->SetColor(obstacle_color);
	obstacle_3->SetDrawData(obstacle_3_drawData);

	Collider* obstacle_3_collider = new Collider();
	ColliderData* obstacle_3_colliderData =
		new ColliderData(0, obstacle_3_collider, false, E_ColliderType::Collider_Rect, obstacle_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_3_collider->AddColliderData(obstacle_3_colliderData);

	obstacle_3->RegisterComponent(E_Component::Component_Collider, obstacle_3_collider, true);

	//movement
	Movement_Breakable* obstacle_3_breakable_movement = new Movement_Breakable(obstacle_3, true, false, false, WALL_HP, 0.0f);
	obstacle_3->AddMovement(obstacle_3_breakable_movement);


#pragma endregion scene_obstacle

#pragma endregion obstacle

	this->RegisterSceneObj(wall_1);
	this->RegisterSceneObj(wall_2);
	this->RegisterSceneObj(wall_3);
	this->RegisterSceneObj(wall_4);
	this->RegisterSceneObj(wall_5);
	this->RegisterSceneObj(wall_6);
	this->RegisterSceneObj(targetPoint);

	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);
	this->RegisterSceneObj(obstacle_3);

	Scene::Load();
}


void Scene_1::Unload() {

	Scene::Unload();
}

void Scene_1::Reset()
{
	Scene::Reset();
}

void Scene_1::OnSceneLoad() {
	Scene::OnSceneLoad();

}

void Scene_1::OnSceneUnload() {
	Scene::OnSceneUnload();

}

#pragma endregion scene_1

#pragma region scene_2

Scene_2::Scene_2() :Scene(E_Scene::Scene_Stage2) {

}

Scene_2::~Scene_2() {

}


void Scene_2::Load() {

	Unload();

	vector2s obstacle_size = vector2s(0, 0);
#pragma region obstacle

#pragma region wall
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH;
	obstacle_size.y = 100.0f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(1, 1);
	wall_1_drawData->preComputeUVSize = vector2s(0, 0);
	wall_1_drawData->tex_texel_size = vector2s(1, 1);
	wall_1_drawData->SetColor(wall_color);
	wall_1->SetDrawData(wall_1_drawData);

	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);

	//2(wall_bottom
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_left_bottom
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//6(wall_right_bottom
	Obstacle* wall_6 = new Obstacle();
	wall_6->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_6->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_6_drawData = new DrawData();
	wall_6_drawData->shakeWithCamera = true;
	wall_6_drawData->SetSize(obstacle_size);
	wall_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_6_drawData->usePreComputeUV = true;
	wall_6_drawData->preComputeUV = vector2s(1, 1);
	wall_6_drawData->preComputeUVSize = vector2s(0, 0);
	wall_6_drawData->tex_texel_size = vector2s(1, 1);
	wall_6_drawData->SetColor(wall_color);
	wall_6->SetDrawData(wall_6_drawData);

	Collider* wall_6_collider = new Collider();
	ColliderData* wall_6_colliderData =
		new ColliderData(0, wall_6_collider, false, E_ColliderType::Collider_Rect, wall_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_6_collider->AddColliderData(wall_6_colliderData);

	wall_6->RegisterComponent(E_Component::Component_Collider, wall_6_collider, true);


	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(100.0f, 100.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);



#pragma endregion wall

#pragma region scene_obstacle

	//1
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(320.0f, 300.0f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(obstacle_size);
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1_drawData->SetColor(obstacle_color);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//movement
	Movement_Breakable* obstacle_1_breakable_movement = new Movement_Breakable(obstacle_1, true, false, false, WALL_HP, 0.0f);
	obstacle_1->AddMovement(obstacle_1_breakable_movement);


	//2
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(650.0f, 420.0f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(obstacle_size);
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2_drawData->SetColor(obstacle_color);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//movement
	Movement_Breakable* obstacle_2_breakable_movement = new Movement_Breakable(obstacle_2, true, false, false, WALL_HP, 0.0f);
	obstacle_2->AddMovement(obstacle_2_breakable_movement);


	//3
	obstacle_size.x = 80.0f;
	obstacle_size.y = 350.0f;
	Obstacle* obstacle_3 = new Obstacle();
	obstacle_3->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_3->SetPos(vector3s(980.0f, 300.0f, 0.0f), true);

	DrawData* obstacle_3_drawData = new DrawData();
	obstacle_3_drawData->shakeWithCamera = true;
	obstacle_3_drawData->SetSize(obstacle_size);
	obstacle_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_3_drawData->usePreComputeUV = true;
	obstacle_3_drawData->preComputeUV = vector2s(1, 1);
	obstacle_3_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_3_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_3_drawData->SetColor(obstacle_color);
	obstacle_3->SetDrawData(obstacle_3_drawData);

	Collider* obstacle_3_collider = new Collider();
	ColliderData* obstacle_3_colliderData =
		new ColliderData(0, obstacle_3_collider, false, E_ColliderType::Collider_Rect, obstacle_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_3_collider->AddColliderData(obstacle_3_colliderData);

	obstacle_3->RegisterComponent(E_Component::Component_Collider, obstacle_3_collider, true);

	//movement
	Movement_Breakable* obstacle_3_breakable_movement = new Movement_Breakable(obstacle_3, true, false, false, WALL_HP, 0.0f);
	obstacle_3->AddMovement(obstacle_3_breakable_movement);

#pragma endregion scene_obstacle
#pragma endregion obstacle

	this->RegisterSceneObj(wall_1);
	this->RegisterSceneObj(wall_2);
	this->RegisterSceneObj(wall_3);
	this->RegisterSceneObj(wall_4);
	this->RegisterSceneObj(wall_5);
	this->RegisterSceneObj(wall_6);
	this->RegisterSceneObj(targetPoint);

	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);
	this->RegisterSceneObj(obstacle_3);

	Scene::Load();
}

void Scene_2::Unload() {
	Scene::Unload();
}

void Scene_2::Reset() {
	Scene::Reset();
}

void Scene_2::DoUpdate(float deltatime) {
	Scene::DoUpdate(deltatime);
}

void Scene_2::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void Scene_2::OnSceneLoad() {
	Scene::OnSceneLoad();

}

void Scene_2::OnSceneUnload() {
	Scene::OnSceneUnload();

}

#pragma endregion scene_2

#pragma region scene_3

Scene_3::Scene_3() :Scene(E_Scene::Scene_Stage3) {

}

Scene_3::~Scene_3() {

}

void Scene_3::Load() {
	Unload();
	vector2s obstacle_size = vector2s(0, 0);
#pragma region wall
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH;
	obstacle_size.y = 100.0f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(1, 1);
	wall_1_drawData->preComputeUVSize = vector2s(0, 0);
	wall_1_drawData->tex_texel_size = vector2s(1, 1);
	wall_1_drawData->SetColor(wall_color);
	wall_1->SetDrawData(wall_1_drawData);

	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);

	//2(wall_bottom
	obstacle_size.x = SCREEN_WIDTH * 0.8f;
	obstacle_size.y = 100.0f;
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_left_bottom
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 180.0f, SCREEN_HEIGHT - 75.0f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(100.0f, 100.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);


#pragma endregion wall
	this->RegisterSceneObj(wall_1);
	this->RegisterSceneObj(wall_2);
	this->RegisterSceneObj(wall_3);
	this->RegisterSceneObj(wall_4);
	this->RegisterSceneObj(wall_5);
	this->RegisterSceneObj(targetPoint);

#pragma region obstacle
	//1
	obstacle_size.x = 200.0f;
	obstacle_size.y = 300.0f;
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT - 120.0f - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(obstacle_size);
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1_drawData->SetColor(obstacle_color);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//movement
	Movement_Breakable* obstacle_1_breakable_movement = new Movement_Breakable(obstacle_1, true, false, false, WALL_HP, 0.0f);
	obstacle_1->AddMovement(obstacle_1_breakable_movement);

	//2
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(SCREEN_WIDTH - 110.0f - obstacle_size.x * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(obstacle_size);
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2_drawData->SetColor(obstacle_color);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//movement
	Movement_Breakable* obstacle_2_breakable_movement = new Movement_Breakable(obstacle_2, true, false, false, WALL_HP, 0.0f);
	obstacle_2->AddMovement(obstacle_2_breakable_movement);


	//NPC
	//1
	Obstacle* npc_1 = new Obstacle();
	npc_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_1->SetPos(vector3s(SCREEN_WIDTH * 0.5f - 300.0f, SCREEN_HEIGHT - 230.0f, 0.0f), true);
	npc_1->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_1_drawData = new DrawData();
	npc_1_drawData->shakeWithCamera = true;
	npc_1_drawData->SetSize(character_size);
	npc_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_1_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_1_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_1_drawData->tex_texel_size = character_cell_tex_size;
	npc_1_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_1->SetDrawData(npc_1_drawData);

	Collider* npc_1_collider = new Collider();
	ColliderData* npc_1_colliderData =
		new ColliderData(0, npc_1_collider, false, E_ColliderType::Collider_Rect, npc_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_1_collider->AddColliderData(npc_1_colliderData);

	npc_1->RegisterComponent(E_Component::Component_Collider, npc_1_collider, true);

	//animation
	Animation* npc_1_AnimComponent = new Animation();
	if (npc_1->RegisterComponent(E_Component::Component_Animation, npc_1_AnimComponent) == false) {//register component first
		delete npc_1_AnimComponent;
		npc_1_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_1_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_1_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_1_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_1_breakable_movement = new Movement_Breakable(npc_1, true, false, false, NPC_HP, 0.0f);
	npc_1->AddMovement(npc_1_breakable_movement);


	//2
	Obstacle* npc_2 = new Obstacle();
	npc_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_2->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f - 100.0f, 0.0f), true);

	DrawData* npc_2_drawData = new DrawData();
	npc_2_drawData->shakeWithCamera = true;
	npc_2_drawData->SetSize(character_size);
	npc_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_2_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_2_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_2_drawData->tex_texel_size = character_cell_tex_size;
	npc_2_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_2->SetDrawData(npc_2_drawData);

	Collider* npc_2_collider = new Collider();
	ColliderData* npc_2_colliderData =
		new ColliderData(0, npc_2_collider, false, E_ColliderType::Collider_Rect, npc_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_2_collider->AddColliderData(npc_2_colliderData);

	npc_2->RegisterComponent(E_Component::Component_Collider, npc_2_collider, true);

	//animation
	Animation* npc_2_AnimComponent = new Animation();
	if (npc_2->RegisterComponent(E_Component::Component_Animation, npc_2_AnimComponent) == false) {//register component first
		delete npc_2_AnimComponent;
		npc_2_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_2_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_2_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_2_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_2_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_2_breakable_movement = new Movement_Breakable(npc_2, true, false, false, NPC_HP, 0.0f);
	npc_2->AddMovement(npc_2_breakable_movement);


	//3
	Obstacle* npc_3 = new Obstacle();
	npc_3->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_3->SetPos(vector3s(SCREEN_WIDTH * 0.65f, 160.0f, 0.0f), true);
	npc_3->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_3_drawData = new DrawData();
	npc_3_drawData->shakeWithCamera = true;
	npc_3_drawData->SetSize(character_size);
	npc_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_3_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_3_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_3_drawData->tex_texel_size = character_cell_tex_size;
	npc_3_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_3->SetDrawData(npc_3_drawData);

	Collider* npc_3_collider = new Collider();
	ColliderData* npc_3_colliderData =
		new ColliderData(0, npc_3_collider, false, E_ColliderType::Collider_Rect, npc_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_3_collider->AddColliderData(npc_3_colliderData);

	npc_3->RegisterComponent(E_Component::Component_Collider, npc_3_collider, true);

	//animation
	Animation* npc_3_AnimComponent = new Animation();
	if (npc_3->RegisterComponent(E_Component::Component_Animation, npc_3_AnimComponent) == false) {//register component first
		delete npc_3_AnimComponent;
		npc_3_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_3_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_3_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_3_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_3_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_3_breakable_movement = new Movement_Breakable(npc_3, true, false, false, NPC_HP, 0.0f);
	npc_3->AddMovement(npc_3_breakable_movement);

#pragma endregion obstacle

	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);

	this->RegisterSceneObj(npc_1);
	this->RegisterSceneObj(npc_2);
	this->RegisterSceneObj(npc_3);

	this->RegisterSceneObj(targetPoint);

	Scene::Load();
}

void Scene_3::Unload() {
	Scene::Unload();

	//hide game obj

}

void Scene_3::Reset() {
	Scene::Reset();
}

void Scene_3::DoUpdate(float deltatime) {
	Scene::DoUpdate(deltatime);
}

void Scene_3::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void Scene_3::OnSceneLoad() {
	Scene::OnSceneLoad();


	//show hud
	//GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	//GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);
	//GameMgr::GetInstance()->inputMgr->SetInputState(true);

	//GameMgr::GetInstance()->cellSpawnerMgr->StartSpawn();

}

void Scene_3::OnSceneUnload() {
	Scene::OnSceneUnload();

}


#pragma endregion scene_3

#pragma region scene_4

Scene_4::Scene_4() :Scene(E_Scene::Scene_Stage4) {

}

Scene_4::~Scene_4() {

}

void Scene_4::Load() {
	Unload();

	vector2s obstacle_size = vector2s(0, 0);
#pragma region wall
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH - 200.0f;
	obstacle_size.y = 100.0f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(obstacle_size.x * 0.5f + 200.0f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(1, 1);
	wall_1_drawData->preComputeUVSize = vector2s(0, 0);
	wall_1_drawData->tex_texel_size = vector2s(1, 1);
	wall_1_drawData->SetColor(wall_color);
	wall_1->SetDrawData(wall_1_drawData);

	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);

	//2(wall_bottom
	obstacle_size.x = SCREEN_WIDTH;
	obstacle_size.y = 100.0f;
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_right_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right_bottom
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//6(wall_right_bottom
	//Obstacle* wall_6 = new Obstacle();
	//wall_6->SetObstacleType(E_Obstacle::Obstacle_Const);
	//wall_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	//wall_6->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	//DrawData* wall_6_drawData = new DrawData();
	//wall_6_drawData->shakeWithCamera = true;
	//wall_6_drawData->SetSize(obstacle_size);
	//wall_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	//wall_6_drawData->usePreComputeUV = true;
	//wall_6_drawData->preComputeUV = vector2s(1, 1);
	//wall_6_drawData->preComputeUVSize = vector2s(0, 0);
	//wall_6_drawData->tex_texel_size = vector2s(1, 1);
	//wall_6_drawData->SetColor(wall_color);
	//wall_6->SetDrawData(wall_6_drawData);

	//Collider* wall_6_collider = new Collider();
	//ColliderData* wall_6_colliderData =
	//	new ColliderData(0, wall_6_collider, false, E_ColliderType::Collider_Rect, wall_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	//wall_6_collider->AddColliderData(wall_6_colliderData);

	//wall_6->RegisterComponent(E_Component::Component_Collider, wall_6_collider, true);


	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 70.0f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(100.0f, 100.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);


#pragma endregion wall
	this->RegisterSceneObj(wall_1);
	this->RegisterSceneObj(wall_2);
	this->RegisterSceneObj(wall_3);
	this->RegisterSceneObj(wall_4);
	this->RegisterSceneObj(wall_5);
	this->RegisterSceneObj(targetPoint);

#pragma region obstacle
	//1
	obstacle_size.x = 150.0f;
	obstacle_size.y = 300.0f;
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(SCREEN_WIDTH * 0.3f, 120.0f + obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(obstacle_size);
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1_drawData->SetColor(obstacle_color);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//movement
	Movement_Breakable* obstacle_1_breakable_movement = new Movement_Breakable(obstacle_1, true, false, false, WALL_HP, 0.0f);
	obstacle_1->AddMovement(obstacle_1_breakable_movement);

	//2
	obstacle_size.x = 400.0f;
	obstacle_size.y = 150.0f;
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(SCREEN_WIDTH * 0.5f + obstacle_size.x * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(obstacle_size);
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2_drawData->SetColor(obstacle_color);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//movement
	Movement_Breakable* obstacle_2_breakable_movement = new Movement_Breakable(obstacle_2, true, false, false, WALL_HP, 0.0f);
	obstacle_2->AddMovement(obstacle_2_breakable_movement);


	//NPC
	//1
	Obstacle* npc_1 = new Obstacle();
	npc_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_1->SetPos(vector3s(SCREEN_WIDTH * 0.3f - 120.0f, 150.0f, 0.0f), true);
	npc_1->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_1_drawData = new DrawData();
	npc_1_drawData->shakeWithCamera = true;
	npc_1_drawData->SetSize(character_size);
	npc_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_1_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_1_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_1_drawData->tex_texel_size = character_cell_tex_size;
	npc_1_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_1->SetDrawData(npc_1_drawData);

	Collider* npc_1_collider = new Collider();
	ColliderData* npc_1_colliderData =
		new ColliderData(0, npc_1_collider, false, E_ColliderType::Collider_Rect, npc_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_1_collider->AddColliderData(npc_1_colliderData);

	npc_1->RegisterComponent(E_Component::Component_Collider, npc_1_collider, true);

	//animation
	Animation* npc_1_AnimComponent = new Animation();
	if (npc_1->RegisterComponent(E_Component::Component_Animation, npc_1_AnimComponent) == false) {//register component first
		delete npc_1_AnimComponent;
		npc_1_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_1_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_1_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_1_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_1_breakable_movement = new Movement_Breakable(npc_1, true, false, false, NPC_HP, 0.0f);
	npc_1->AddMovement(npc_1_breakable_movement);


	//2
	Obstacle* npc_2 = new Obstacle();
	npc_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_2->SetPos(vector3s(120.0f, SCREEN_HEIGHT - 180.0f, 0.0f), true);

	DrawData* npc_2_drawData = new DrawData();
	npc_2_drawData->shakeWithCamera = true;
	npc_2_drawData->SetSize(character_size);
	npc_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_2_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_2_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_2_drawData->tex_texel_size = character_cell_tex_size;
	npc_2_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_2->SetDrawData(npc_2_drawData);

	Collider* npc_2_collider = new Collider();
	ColliderData* npc_2_colliderData =
		new ColliderData(0, npc_2_collider, false, E_ColliderType::Collider_Rect, npc_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_2_collider->AddColliderData(npc_2_colliderData);

	npc_2->RegisterComponent(E_Component::Component_Collider, npc_2_collider, true);

	//animation
	Animation* npc_2_AnimComponent = new Animation();
	if (npc_2->RegisterComponent(E_Component::Component_Animation, npc_2_AnimComponent) == false) {//register component first
		delete npc_2_AnimComponent;
		npc_2_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_2_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_2_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_2_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_2_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_2_breakable_movement = new Movement_Breakable(npc_2, true, false, false, NPC_HP, 0.0f);
	npc_2->AddMovement(npc_2_breakable_movement);


	//3
	Obstacle* npc_3 = new Obstacle();
	npc_3->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_3->SetPos(vector3s(SCREEN_WIDTH * 0.85f, 160.0f, 0.0f), true);
	npc_3->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_3_drawData = new DrawData();
	npc_3_drawData->shakeWithCamera = true;
	npc_3_drawData->SetSize(character_size);
	npc_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_3_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_3_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_3_drawData->tex_texel_size = character_cell_tex_size;
	npc_3_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_3->SetDrawData(npc_3_drawData);

	Collider* npc_3_collider = new Collider();
	ColliderData* npc_3_colliderData =
		new ColliderData(0, npc_3_collider, false, E_ColliderType::Collider_Rect, npc_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_3_collider->AddColliderData(npc_3_colliderData);

	npc_3->RegisterComponent(E_Component::Component_Collider, npc_3_collider, true);

	//animation
	Animation* npc_3_AnimComponent = new Animation();
	if (npc_3->RegisterComponent(E_Component::Component_Animation, npc_3_AnimComponent) == false) {//register component first
		delete npc_3_AnimComponent;
		npc_3_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_3_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_3_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_3_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_3_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_3_breakable_movement = new Movement_Breakable(npc_3, true, false, false, NPC_HP, 0.0f);
	npc_3->AddMovement(npc_3_breakable_movement);

	//4
	Obstacle* npc_4 = new Obstacle();
	npc_4->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_4->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - 250.0f, 0.0f), true);
	npc_4->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_4_drawData = new DrawData();
	npc_4_drawData->shakeWithCamera = true;
	npc_4_drawData->SetSize(character_size);
	npc_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_4_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_4_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_4_drawData->tex_texel_size = character_cell_tex_size;
	npc_4_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_4->SetDrawData(npc_4_drawData);

	Collider* npc_4_collider = new Collider();
	ColliderData* npc_4_colliderData =
		new ColliderData(0, npc_4_collider, false, E_ColliderType::Collider_Rect, npc_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_4_collider->AddColliderData(npc_4_colliderData);

	npc_4->RegisterComponent(E_Component::Component_Collider, npc_4_collider, true);

	//animation
	Animation* npc_4_AnimComponent = new Animation();
	if (npc_4->RegisterComponent(E_Component::Component_Animation, npc_4_AnimComponent) == false) {//register component first
		delete npc_4_AnimComponent;
		npc_4_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_4_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_4_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_4_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_4_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_4_breakable_movement = new Movement_Breakable(npc_4, true, false, false, NPC_HP, 0.0f);
	npc_4->AddMovement(npc_4_breakable_movement);


	//5
	Obstacle* npc_5 = new Obstacle();
	npc_5->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_5->SetPos(vector3s(SCREEN_WIDTH * 0.4f, 150.0f, 0.0f), true);

	DrawData* npc_5_drawData = new DrawData();
	npc_5_drawData->shakeWithCamera = true;
	npc_5_drawData->SetSize(character_size);
	npc_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_5_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_5_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_5_drawData->tex_texel_size = character_cell_tex_size;
	npc_5_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_5->SetDrawData(npc_5_drawData);

	Collider* npc_5_collider = new Collider();
	ColliderData* npc_5_colliderData =
		new ColliderData(0, npc_5_collider, false, E_ColliderType::Collider_Rect, npc_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_5_collider->AddColliderData(npc_5_colliderData);

	npc_5->RegisterComponent(E_Component::Component_Collider, npc_5_collider, true);

	//animation
	Animation* npc_5_AnimComponent = new Animation();
	if (npc_5->RegisterComponent(E_Component::Component_Animation, npc_5_AnimComponent) == false) {//register component first
		delete npc_5_AnimComponent;
		npc_5_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_5_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_5_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_5_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_5_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_5_breakable_movement = new Movement_Breakable(npc_5, true, false, false, NPC_HP, 0.0f);
	npc_5->AddMovement(npc_5_breakable_movement);

	//6
	Obstacle* npc_6 = new Obstacle();
	npc_6->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_6->SetPos(vector3s(SCREEN_WIDTH * 0.3f - 120.0f, SCREEN_HEIGHT * 0.5f + 50.0f, 0.0f), true);
	npc_6->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_6_drawData = new DrawData();
	npc_6_drawData->shakeWithCamera = true;
	npc_6_drawData->SetSize(character_size);
	npc_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_6_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_6_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_6_drawData->tex_texel_size = character_cell_tex_size;
	npc_6_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_6->SetDrawData(npc_6_drawData);

	Collider* npc_6_collider = new Collider();
	ColliderData* npc_6_colliderData =
		new ColliderData(0, npc_6_collider, false, E_ColliderType::Collider_Rect, npc_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_6_collider->AddColliderData(npc_6_colliderData);

	npc_6->RegisterComponent(E_Component::Component_Collider, npc_6_collider, true);

	//animation
	Animation* npc_6_AnimComponent = new Animation();
	if (npc_6->RegisterComponent(E_Component::Component_Animation, npc_6_AnimComponent) == false) {//register component first
		delete npc_6_AnimComponent;
		npc_6_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_6_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_6_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_6_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_6_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_6_breakable_movement = new Movement_Breakable(npc_6, true, false, false, NPC_HP, 0.0f);
	npc_6->AddMovement(npc_6_breakable_movement);


#pragma endregion obstacle

	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);

	this->RegisterSceneObj(npc_1);
	this->RegisterSceneObj(npc_2);
	this->RegisterSceneObj(npc_3);
	this->RegisterSceneObj(npc_4);
	this->RegisterSceneObj(npc_5);
	this->RegisterSceneObj(npc_6);

	this->RegisterSceneObj(targetPoint);

	Scene::Load();
}

void Scene_4::Unload() {
	Scene::Unload();

	//hide game obj

}

void Scene_4::Reset() {
	Scene::Reset();
}

void Scene_4::DoUpdate(float deltatime) {
	Scene::DoUpdate(deltatime);
}

void Scene_4::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void Scene_4::OnSceneLoad() {
	Scene::OnSceneLoad();


	//show hud
	//GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	//GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);
	//GameMgr::GetInstance()->inputMgr->SetInputState(true);

	//GameMgr::GetInstance()->cellSpawnerMgr->StartSpawn();

}

void Scene_4::OnSceneUnload() {
	Scene::OnSceneUnload();

}


#pragma endregion scene_4

#pragma region scene_5

Scene_5::Scene_5() :Scene(E_Scene::Scene_Stage5) {

}

Scene_5::~Scene_5() {

}

void Scene_5::Load() {
	Unload();
	vector2s obstacle_size = vector2s(0, 0);
#pragma region wall
	//1(wall_top
	obstacle_size.x = SCREEN_WIDTH;
	obstacle_size.y = 100.0f;
	Obstacle* wall_1 = new Obstacle();
	wall_1->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_1->SetPos(vector3s(SCREEN_WIDTH * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_1_drawData = new DrawData();
	wall_1_drawData->shakeWithCamera = true;
	wall_1_drawData->SetSize(obstacle_size);
	wall_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_1_drawData->usePreComputeUV = true;
	wall_1_drawData->preComputeUV = vector2s(1, 1);
	wall_1_drawData->preComputeUVSize = vector2s(0, 0);
	wall_1_drawData->tex_texel_size = vector2s(1, 1);
	wall_1_drawData->SetColor(wall_color);
	wall_1->SetDrawData(wall_1_drawData);

	Collider* wall_1_collider = new Collider();
	ColliderData* wall_1_colliderData =
		new ColliderData(0, wall_1_collider, false, E_ColliderType::Collider_Rect, wall_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_1_collider->AddColliderData(wall_1_colliderData);

	wall_1->RegisterComponent(E_Component::Component_Collider, wall_1_collider, true);

	//2(wall_bottom
	obstacle_size.x = SCREEN_WIDTH - 300.0f;
	obstacle_size.y = 100.0f;
	Obstacle* wall_2 = new Obstacle();
	wall_2->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_2->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_2_drawData = new DrawData();
	wall_2_drawData->shakeWithCamera = true;
	wall_2_drawData->SetSize(obstacle_size);
	wall_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_2_drawData->usePreComputeUV = true;
	wall_2_drawData->preComputeUV = vector2s(1, 1);
	wall_2_drawData->preComputeUVSize = vector2s(0, 0);
	wall_2_drawData->tex_texel_size = vector2s(1, 1);
	wall_2_drawData->SetColor(wall_color);
	wall_2->SetDrawData(wall_2_drawData);

	Collider* wall_2_collider = new Collider();
	ColliderData* wall_2_colliderData =
		new ColliderData(0, wall_2_collider, false, E_ColliderType::Collider_Rect, wall_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_2_collider->AddColliderData(wall_2_colliderData);

	wall_2->RegisterComponent(E_Component::Component_Collider, wall_2_collider, true);

	//3(wall_left_top
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_3 = new Obstacle();
	wall_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_3->SetPos(vector3s(obstacle_size.x * 0.5f, obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_3_drawData = new DrawData();
	wall_3_drawData->shakeWithCamera = true;
	wall_3_drawData->SetSize(obstacle_size);
	wall_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_3_drawData->usePreComputeUV = true;
	wall_3_drawData->preComputeUV = vector2s(1, 1);
	wall_3_drawData->preComputeUVSize = vector2s(0, 0);
	wall_3_drawData->tex_texel_size = vector2s(1, 1);
	wall_3_drawData->SetColor(wall_color);
	wall_3->SetDrawData(wall_3_drawData);

	Collider* wall_3_collider = new Collider();
	ColliderData* wall_3_colliderData =
		new ColliderData(0, wall_3_collider, false, E_ColliderType::Collider_Rect, wall_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_3_collider->AddColliderData(wall_3_colliderData);

	wall_3->RegisterComponent(E_Component::Component_Collider, wall_3_collider, true);

	//4(wall_left_bottom
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT * 0.5f - 70.0f;
	Obstacle* wall_4 = new Obstacle();
	wall_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_4->SetPos(vector3s(obstacle_size.x * 0.5f, SCREEN_HEIGHT - obstacle_size.y * 0.5f, 0.0f), true);

	DrawData* wall_4_drawData = new DrawData();
	wall_4_drawData->shakeWithCamera = true;
	wall_4_drawData->SetSize(obstacle_size);
	wall_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_4_drawData->usePreComputeUV = true;
	wall_4_drawData->preComputeUV = vector2s(1, 1);
	wall_4_drawData->preComputeUVSize = vector2s(0, 0);
	wall_4_drawData->tex_texel_size = vector2s(1, 1);
	wall_4_drawData->SetColor(wall_color);
	wall_4->SetDrawData(wall_4_drawData);

	Collider* wall_4_collider = new Collider();
	ColliderData* wall_4_colliderData =
		new ColliderData(0, wall_4_collider, false, E_ColliderType::Collider_Rect, wall_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_4_collider->AddColliderData(wall_4_colliderData);

	wall_4->RegisterComponent(E_Component::Component_Collider, wall_4_collider, true);

	//5(wall_right
	obstacle_size.x = 100.0f;
	obstacle_size.y = SCREEN_HEIGHT;
	Obstacle* wall_5 = new Obstacle();
	wall_5->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_5->SetPos(vector3s(SCREEN_WIDTH - obstacle_size.x * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* wall_5_drawData = new DrawData();
	wall_5_drawData->shakeWithCamera = true;
	wall_5_drawData->SetSize(obstacle_size);
	wall_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_5_drawData->usePreComputeUV = true;
	wall_5_drawData->preComputeUV = vector2s(1, 1);
	wall_5_drawData->preComputeUVSize = vector2s(0, 0);
	wall_5_drawData->tex_texel_size = vector2s(1, 1);
	wall_5_drawData->SetColor(wall_color);
	wall_5->SetDrawData(wall_5_drawData);

	Collider* wall_5_collider = new Collider();
	ColliderData* wall_5_colliderData =
		new ColliderData(0, wall_5_collider, false, E_ColliderType::Collider_Rect, wall_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_5_collider->AddColliderData(wall_5_colliderData);

	wall_5->RegisterComponent(E_Component::Component_Collider, wall_5_collider, true);

	//6(wall_scene_1
	obstacle_size.x = 200.0f;
	obstacle_size.y = 300.0f;
	Obstacle* wall_6 = new Obstacle();
	wall_6->SetObstacleType(E_Obstacle::Obstacle_Const);
	wall_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	wall_6->SetPos(vector3s(SCREEN_WIDTH - 400.0f, SCREEN_HEIGHT - obstacle_size.y * 0.5f - 100.0f, 0.0f), true);

	DrawData* wall_6_drawData = new DrawData();
	wall_6_drawData->shakeWithCamera = true;
	wall_6_drawData->SetSize(obstacle_size);
	wall_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	wall_6_drawData->usePreComputeUV = true;
	wall_6_drawData->preComputeUV = vector2s(1, 1);
	wall_6_drawData->preComputeUVSize = vector2s(0, 0);
	wall_6_drawData->tex_texel_size = vector2s(1, 1);
	wall_6_drawData->SetColor(wall_color);
	wall_6->SetDrawData(wall_6_drawData);

	Collider* wall_6_collider = new Collider();
	ColliderData* wall_6_colliderData =
		new ColliderData(0, wall_6_collider, false, E_ColliderType::Collider_Rect, wall_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	wall_6_collider->AddColliderData(wall_6_colliderData);

	wall_6->RegisterComponent(E_Component::Component_Collider, wall_6_collider, true);


	//targetPoint
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 75.0f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(150.0f, 150.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);


#pragma endregion wall
	this->RegisterSceneObj(wall_1);
	this->RegisterSceneObj(wall_2);
	this->RegisterSceneObj(wall_3);
	this->RegisterSceneObj(wall_4);
	this->RegisterSceneObj(wall_5);
	this->RegisterSceneObj(wall_6);
	this->RegisterSceneObj(targetPoint);

#pragma region obstacle
	//1
	obstacle_size.x = 110.0f;
	obstacle_size.y = 250.0f;
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT - obstacle_size.y * 0.5f - 120.0f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(obstacle_size);
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1_drawData->SetColor(obstacle_color);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//movement
	Movement_Breakable* obstacle_1_breakable_movement = new Movement_Breakable(obstacle_1, true, false, false, WALL_HP, 0.0f);
	obstacle_1->AddMovement(obstacle_1_breakable_movement);

	//2
	obstacle_size.x = 100.0f;
	obstacle_size.y = 300.0f;
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(SCREEN_WIDTH * 0.45f, obstacle_size.y * 0.5f + 120.0f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(obstacle_size);
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2_drawData->SetColor(obstacle_color);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//movement
	Movement_Breakable* obstacle_2_breakable_movement = new Movement_Breakable(obstacle_2, true, false, false, WALL_HP, 0.0f);
	obstacle_2->AddMovement(obstacle_2_breakable_movement);


	//NPC
	//1
	Obstacle* npc_1 = new Obstacle();
	npc_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);

	npc_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_1->SetPos(vector3s(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.5f + 20.0f, 0.0f), true);
	npc_1->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_1_drawData = new DrawData();
	npc_1_drawData->shakeWithCamera = true;
	npc_1_drawData->SetSize(character_size);
	npc_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_1_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_1_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_1_drawData->tex_texel_size = character_cell_tex_size;
	npc_1_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_1->SetDrawData(npc_1_drawData);

	Collider* npc_1_collider = new Collider();
	ColliderData* npc_1_colliderData =
		new ColliderData(0, npc_1_collider, false, E_ColliderType::Collider_Rect, npc_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_1_collider->AddColliderData(npc_1_colliderData);

	npc_1->RegisterComponent(E_Component::Component_Collider, npc_1_collider, true);

	//animation
	Animation* npc_1_AnimComponent = new Animation();
	if (npc_1->RegisterComponent(E_Component::Component_Animation, npc_1_AnimComponent) == false) {//register component first
		delete npc_1_AnimComponent;
		npc_1_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_1_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_1_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_1_IdleAnim, true);

		AnimClip* npc_1_MoveAnim = new AnimClip();
		startTexIndex = NPC_MOVE_INDEX_Y * NPC_TEX_W_COUNT + NPC_MOVE_INDEX_X;
		endTexIndex = startTexIndex + NPC_MOVE_FRAME_COUNT;
		npc_1_MoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Move, npc_1_MoveAnim, false);
	}
	//ai
	AIComponent* npc_1_ai_component = new AIComponent();
	npc_1->RegisterComponent(E_Component::Component_AI, npc_1_ai_component);
	SimplePatrolAI* npc_1_simple_ai = new SimplePatrolAI(npc_1,
		E_PatrolType::PatrolType_LoopCircle, npc_1->GetTransform()->GetPos(), 1.0f, true);
	npc_1_ai_component->SetAI(npc_1_simple_ai);
	//set path
	npc_1_simple_ai->AddPatrolPoint(npc_1->GetTransform()->GetPos());
	npc_1_simple_ai->AddPatrolPoint(vector3s(0.0f, 150.0f, 0.0f) + npc_1->GetTransform()->GetPos());

	//movement
	Movement_Breakable* npc_1_breakable_movement = new Movement_Breakable(npc_1, true, false, false, NPC_HP, 0.0f);
	npc_1->AddMovement(npc_1_breakable_movement);


	//2
	Obstacle* npc_2 = new Obstacle();
	npc_2->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_2->SetPos(vector3s(SCREEN_WIDTH * 0.20f, 180.0f, 0.0f), true);

	DrawData* npc_2_drawData = new DrawData();
	npc_2_drawData->shakeWithCamera = true;
	npc_2_drawData->SetSize(character_size);
	npc_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_2_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_2_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_2_drawData->tex_texel_size = character_cell_tex_size;
	npc_2_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_2->SetDrawData(npc_2_drawData);

	Collider* npc_2_collider = new Collider();
	ColliderData* npc_2_colliderData =
		new ColliderData(0, npc_2_collider, false, E_ColliderType::Collider_Rect, npc_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_2_collider->AddColliderData(npc_2_colliderData);

	npc_2->RegisterComponent(E_Component::Component_Collider, npc_2_collider, true);

	//animation
	Animation* npc_2_AnimComponent = new Animation();
	if (npc_2->RegisterComponent(E_Component::Component_Animation, npc_2_AnimComponent) == false) {//register component first
		delete npc_2_AnimComponent;
		npc_2_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_2_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_2_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_2_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_2_IdleAnim, true);

		AnimClip* npc_2_MoveAnim = new AnimClip();
		startTexIndex = NPC_MOVE_INDEX_Y * NPC_TEX_W_COUNT + NPC_MOVE_INDEX_X;
		endTexIndex = startTexIndex + NPC_MOVE_FRAME_COUNT;
		npc_2_MoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		npc_2_AnimComponent->RegisterAnim(E_Anim::Anim_Move, npc_2_MoveAnim, false);
	}

	//ai
	AIComponent* npc_2_ai_component = new AIComponent();
	npc_2->RegisterComponent(E_Component::Component_AI, npc_2_ai_component);
	SimplePatrolAI* npc_2_simple_ai = new SimplePatrolAI(npc_2,
		E_PatrolType::PatrolType_LoopCircle, npc_2->GetTransform()->GetPos(), 1.0f, true);
	npc_2_ai_component->SetAI(npc_2_simple_ai);
	//set path
	npc_2_simple_ai->AddPatrolPoint(npc_2->GetTransform()->GetPos());
	npc_2_simple_ai->AddPatrolPoint(vector3s(180.0f, 0.0f, 0.0f) + npc_2->GetTransform()->GetPos());

	//movement
	Movement_Breakable* npc_2_breakable_movement = new Movement_Breakable(npc_2, true, false, false, NPC_HP, 0.0f);
	npc_2->AddMovement(npc_2_breakable_movement);

	//3
	Obstacle* npc_3 = new Obstacle();
	npc_3->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_3->SetPos(vector3s(SCREEN_WIDTH * 0.20f + 200.0f, SCREEN_HEIGHT - 140.0f, 0.0f), true);

	DrawData* npc_3_drawData = new DrawData();
	npc_3_drawData->shakeWithCamera = true;
	npc_3_drawData->SetSize(character_size);
	npc_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_3_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_3_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_3_drawData->tex_texel_size = character_cell_tex_size;
	npc_3_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_3->SetDrawData(npc_3_drawData);

	Collider* npc_3_collider = new Collider();
	ColliderData* npc_3_colliderData =
		new ColliderData(0, npc_3_collider, false, E_ColliderType::Collider_Rect, npc_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_3_collider->AddColliderData(npc_3_colliderData);

	npc_3->RegisterComponent(E_Component::Component_Collider, npc_3_collider, true);

	//animation
	Animation* npc_3_AnimComponent = new Animation();
	if (npc_3->RegisterComponent(E_Component::Component_Animation, npc_3_AnimComponent) == false) {//register component first
		delete npc_3_AnimComponent;
		npc_3_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_3_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_3_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_3_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_3_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_3_breakable_movement = new Movement_Breakable(npc_3, true, false, false, NPC_HP, 0.0f);
	npc_3->AddMovement(npc_3_breakable_movement);

	//4
	Obstacle* npc_4 = new Obstacle();
	npc_4->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_4->SetPos(vector3s(SCREEN_WIDTH * 0.75f, 150.0f, 0.0f), true);

	DrawData* npc_4_drawData = new DrawData();
	npc_4_drawData->shakeWithCamera = true;
	npc_4_drawData->SetSize(character_size);
	npc_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_4_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_4_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_4_drawData->tex_texel_size = character_cell_tex_size;
	npc_4_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_4->SetDrawData(npc_4_drawData);

	Collider* npc_4_collider = new Collider();
	ColliderData* npc_4_colliderData =
		new ColliderData(0, npc_4_collider, false, E_ColliderType::Collider_Rect, npc_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_4_collider->AddColliderData(npc_4_colliderData);

	npc_4->RegisterComponent(E_Component::Component_Collider, npc_4_collider, true);

	//animation
	Animation* npc_4_AnimComponent = new Animation();
	if (npc_4->RegisterComponent(E_Component::Component_Animation, npc_4_AnimComponent) == false) {//register component first
		delete npc_4_AnimComponent;
		npc_4_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_4_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_4_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_4_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_4_IdleAnim, true);

		AnimClip* npc_4_MoveAnim = new AnimClip();
		startTexIndex = NPC_MOVE_INDEX_Y * NPC_TEX_W_COUNT + NPC_MOVE_INDEX_X;
		endTexIndex = startTexIndex + NPC_MOVE_FRAME_COUNT;
		npc_4_MoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		npc_4_AnimComponent->RegisterAnim(E_Anim::Anim_Move, npc_4_MoveAnim, false);
	}

	//ai
	AIComponent* npc_4_ai_component = new AIComponent();
	npc_4->RegisterComponent(E_Component::Component_AI, npc_4_ai_component);

	/*
	SimplePatrolAI* npc_4_simple_ai = new SimplePatrolAI(npc_4,
		E_PatrolType::PatrolType_LoopCircle, npc_4->GetTransform()->GetPos(), 1.0f, true);
	npc_4_ai_component->SetAI(npc_4_simple_ai);
	//set path
	npc_4_simple_ai->AddPatrolPoint(npc_4->GetTransform()->GetPos());
	npc_4_simple_ai->AddPatrolPoint(vector3s(0.0f, 100.0f, 0.0f) + npc_4->GetTransform()->GetPos());
	*/
	FSMSystem* fsmSystem = npc_4_ai_component->GetFSMSystem();
		//state
	FSMState* idleState = new FSMState(fsmSystem);
	FSMState* moveState = new FSMState(fsmSystem);
	FSMState_Base_Action_Idle* baseAction_idle = new FSMState_Base_Action_Idle();
	FSMState_Base_Action_Partrol* baseAction_partrol = new FSMState_Base_Action_Partrol();
	baseAction_idle->setAnim = E_Anim::Anim_Idle;
	idleState->SetAction(baseAction_idle);
	baseAction_partrol->setAnim = E_Anim::Anim_Move;
	moveState->SetAction(baseAction_partrol);


		//trigger action
	FSMStateTrigger_Base_Trigger_Dist2Player* idle2Move_trigger_action_dist2Player = new FSMStateTrigger_Base_Trigger_Dist2Player();
	idle2Move_trigger_action_dist2Player->dist = 400.0f;
	FSMStateTrigger_Base_Trigger_Dist2Player* move2Idle_trigger_action_dist2Player = new FSMStateTrigger_Base_Trigger_Dist2Player();
	move2Idle_trigger_action_dist2Player->dist = 400.0f;
		//trigger
	FSMStateTrigger* idle2Move_trigger_dist = new FSMStateTrigger();
	idle2Move_trigger_dist->SetTriggerParam(idleState, idle2Move_trigger_action_dist2Player);
	FSMTriggerGroup* idle2Move_trigger_group = new FSMTriggerGroup(E_CheckTriggerType::CheckTriggerType_AllTrue,fsmSystem);
	idle2Move_trigger_group->AddTrigger(idle2Move_trigger_dist);

	FSMStateTrigger* move2Idle_trigger_dist = new FSMStateTrigger();
	move2Idle_trigger_dist->SetTriggerParam(moveState, move2Idle_trigger_action_dist2Player);
	FSMTriggerGroup* move2Idle_trigger_group = new FSMTriggerGroup(E_CheckTriggerType::CheckTriggerType_AllFalse, fsmSystem);
	move2Idle_trigger_group->AddTrigger(move2Idle_trigger_dist);

	idleState->RegisterTrigger(E_FSMState::FSMState_MoveToPlayer, idle2Move_trigger_group);
	moveState->RegisterTrigger(E_FSMState::FSMState_Idle, move2Idle_trigger_group);

	fsmSystem->RegisterState(E_FSMState::FSMState_Idle, idleState, true);
	fsmSystem->RegisterState(E_FSMState::FSMState_MoveToPlayer, moveState, false);

	//movement
	//Movement_Breakable* npc_4_breakable_movement = new Movement_Breakable(npc_4, true, false, false, NPC_HP, 0.0f);
	//npc_4->AddMovement(npc_4_breakable_movement);
	Movement_Axis* npc_4_axisMove_movement = new Movement_Axis(npc_4, true, true, true, false, 2.0f, npc_4->GetTransform()->GetPos(),
		vector3s(0, 0, 0), vector3s(100.0f, 0.0f, 0.0f));
	npc_4->AddMovement(npc_4_axisMove_movement);

	//5
	Obstacle* npc_5 = new Obstacle();
	npc_5->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_5->SetPos(vector3s(SCREEN_WIDTH * 0.6f - 15.0f, SCREEN_HEIGHT * 0.5f - 100.0f, 0.0f), true);
	npc_5->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* npc_5_drawData = new DrawData();
	npc_5_drawData->shakeWithCamera = true;
	npc_5_drawData->SetSize(character_size);
	npc_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_5_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_5_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_5_drawData->tex_texel_size = character_cell_tex_size;
	npc_5_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_5->SetDrawData(npc_5_drawData);

	Collider* npc_5_collider = new Collider();
	ColliderData* npc_5_colliderData =
		new ColliderData(0, npc_5_collider, false, E_ColliderType::Collider_Rect, npc_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_5_collider->AddColliderData(npc_5_colliderData);

	npc_5->RegisterComponent(E_Component::Component_Collider, npc_5_collider, true);

	//animation
	Animation* npc_5_AnimComponent = new Animation();
	if (npc_5->RegisterComponent(E_Component::Component_Animation, npc_5_AnimComponent) == false) {//register component first
		delete npc_5_AnimComponent;
		npc_5_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_5_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_5_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_5_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_5_IdleAnim, true);
	}

	//movement
	Movement_Breakable* npc_5_breakable_movement = new Movement_Breakable(npc_5, true, false, false, NPC_HP, 0.0f);
	npc_5->AddMovement(npc_5_breakable_movement);

#pragma endregion obstacle

	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);

	this->RegisterSceneObj(npc_1);
	this->RegisterSceneObj(npc_2);
	this->RegisterSceneObj(npc_3);
	this->RegisterSceneObj(npc_4);
	this->RegisterSceneObj(npc_5);

	this->RegisterSceneObj(targetPoint);
	Scene::Load();
}

void Scene_5::Unload() {
	Scene::Unload();

	//hide game obj

}

void Scene_5::Reset() {
	Scene::Reset();
}

void Scene_5::DoUpdate(float deltatime) {
	Scene::DoUpdate(deltatime);
}

void Scene_5::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void Scene_5::OnSceneLoad() {
	Scene::OnSceneLoad();


	//show hud
	//GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	//GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);
	//GameMgr::GetInstance()->inputMgr->SetInputState(true);

	//GameMgr::GetInstance()->cellSpawnerMgr->StartSpawn();

}

void Scene_5::OnSceneUnload() {
	Scene::OnSceneUnload();

}


#pragma endregion scene_5

#pragma region scene_6

Scene_6::Scene_6() :Scene(E_Scene::Scene_Stage6) {

}

Scene_6::~Scene_6() {

}

void Scene_6::Load() {
	Unload();
#pragma region obstacle
	//1
	Obstacle* obstacle_1 = new Obstacle();
	obstacle_1->SetObstacleType(E_Obstacle::Obstacle_Mirror);
	obstacle_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_1->SetPos(vector3s(700.0f, 300.0f, 0.0f), true);

	DrawData* obstacle_1_drawData = new DrawData();
	obstacle_1_drawData->shakeWithCamera = true;
	obstacle_1_drawData->SetSize(vector2s(200, 500.0f));
	obstacle_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_1_drawData->usePreComputeUV = true;
	obstacle_1_drawData->preComputeUV = vector2s(1, 1);
	obstacle_1_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_1_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_1->SetDrawData(obstacle_1_drawData);

	Collider* obstacle_1_collider = new Collider();
	ColliderData* obstacle_1_colliderData =
		new ColliderData(0, obstacle_1_collider, false, E_ColliderType::Collider_Rect, obstacle_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_1_collider->AddColliderData(obstacle_1_colliderData);

	obstacle_1->RegisterComponent(E_Component::Component_Collider, obstacle_1_collider, true);

	//2
	Obstacle* obstacle_2 = new Obstacle();
	obstacle_2->SetObstacleType(E_Obstacle::Obstacle_Mirror);
	obstacle_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_2->SetPos(vector3s(100.0f, 700.0f, 0.0f), true);

	DrawData* obstacle_2_drawData = new DrawData();
	obstacle_2_drawData->shakeWithCamera = true;
	obstacle_2_drawData->SetSize(vector2s(200, 500.0f));
	obstacle_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_2_drawData->usePreComputeUV = true;
	obstacle_2_drawData->preComputeUV = vector2s(1, 1);
	obstacle_2_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_2_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_2->SetDrawData(obstacle_2_drawData);

	Collider* obstacle_2_collider = new Collider();
	ColliderData* obstacle_2_colliderData =
		new ColliderData(0, obstacle_2_collider, false, E_ColliderType::Collider_Rect, obstacle_2_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_2_collider->AddColliderData(obstacle_2_colliderData);

	obstacle_2->RegisterComponent(E_Component::Component_Collider, obstacle_2_collider, true);

	//3
	Obstacle* obstacle_3 = new Obstacle();
	obstacle_3->SetObstacleType(E_Obstacle::Obstacle_Const);
	obstacle_3->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_3->SetPos(vector3s(700.0f, 900.0f, 0.0f), true);

	DrawData* obstacle_3_drawData = new DrawData();
	obstacle_3_drawData->shakeWithCamera = true;
	obstacle_3_drawData->SetSize(vector2s(200, 500.0f));
	obstacle_3_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_3_drawData->usePreComputeUV = true;
	obstacle_3_drawData->preComputeUV = vector2s(1, 1);
	obstacle_3_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_3_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_3_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	obstacle_3->SetDrawData(obstacle_3_drawData);

	Collider* obstacle_3_collider = new Collider();
	ColliderData* obstacle_3_colliderData =
		new ColliderData(0, obstacle_3_collider, false, E_ColliderType::Collider_Rect, obstacle_3_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_3_collider->AddColliderData(obstacle_3_colliderData);

	obstacle_3->RegisterComponent(E_Component::Component_Collider, obstacle_3_collider, true);

	//4
	Obstacle* obstacle_4 = new Obstacle();
	obstacle_4->SetObstacleType(E_Obstacle::Obstacle_Const);
	obstacle_4->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_4->SetPos(vector3s(350, 500.0f, 0.0f), true);

	DrawData* obstacle_4_drawData = new DrawData();
	obstacle_4_drawData->shakeWithCamera = true;
	obstacle_4_drawData->SetSize(vector2s(70.0f, 120.0f));
	obstacle_4_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_4_drawData->usePreComputeUV = true;
	obstacle_4_drawData->preComputeUV = vector2s(1, 1);
	obstacle_4_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_4_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_4_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	obstacle_4->SetDrawData(obstacle_4_drawData);

	Collider* obstacle_4_collider = new Collider();
	ColliderData* obstacle_4_colliderData =
		new ColliderData(0, obstacle_4_collider, false, E_ColliderType::Collider_Rect, obstacle_4_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_4_collider->AddColliderData(obstacle_4_colliderData);

	obstacle_4->RegisterComponent(E_Component::Component_Collider, obstacle_4_collider, true);

	//5
	Obstacle* obstacle_5 = new Obstacle();
	obstacle_5->SetObstacleType(E_Obstacle::Obstacle_ChargeMovement);
	obstacle_5->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_5->SetPos(vector3s(350.0f, 100.0f, 0.0f), true);

	DrawData* obstacle_5_drawData = new DrawData();
	obstacle_5_drawData->shakeWithCamera = true;
	obstacle_5_drawData->SetSize(vector2s(70.0f, 120.0f));
	obstacle_5_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_5_drawData->usePreComputeUV = true;
	obstacle_5_drawData->preComputeUV = vector2s(1, 1);
	obstacle_5_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_5_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_5_drawData->SetColor(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	obstacle_5->SetDrawData(obstacle_5_drawData);

	Collider* obstacle_5_collider = new Collider();
	ColliderData* obstacle_5_colliderData =
		new ColliderData(0, obstacle_5_collider, false, E_ColliderType::Collider_Rect, obstacle_5_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_5_collider->AddColliderData(obstacle_5_colliderData);

	obstacle_5->RegisterComponent(E_Component::Component_Collider, obstacle_5_collider, true);

	//movement
	Movement_Axis* obstacle_5_axis_movement = new Movement_Axis(obstacle_5, true, false, true, false, 1.0f, obstacle_5->GetTransform()->GetPos(),
		vector3s(0, 0, 0), vector3s(0, 200, 0));
	obstacle_5->AddMovement(obstacle_5_axis_movement);


	//6
	Obstacle* obstacle_6 = new Obstacle();
	obstacle_6->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	obstacle_6->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	obstacle_6->SetPos(vector3s(900.0f, 100.0f, 0.0f), true);

	DrawData* obstacle_6_drawData = new DrawData();
	obstacle_6_drawData->shakeWithCamera = true;
	obstacle_6_drawData->SetSize(vector2s(70.0f, 120.0f));
	obstacle_6_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	obstacle_6_drawData->usePreComputeUV = true;
	obstacle_6_drawData->preComputeUV = vector2s(1, 1);
	obstacle_6_drawData->preComputeUVSize = vector2s(0, 0);
	obstacle_6_drawData->tex_texel_size = vector2s(1, 1);
	obstacle_6_drawData->SetColor(obstacle_color);
	obstacle_6->SetDrawData(obstacle_6_drawData);

	Collider* obstacle_6_collider = new Collider();
	ColliderData* obstacle_6_colliderData =
		new ColliderData(0, obstacle_6_collider, false, E_ColliderType::Collider_Rect, obstacle_6_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	obstacle_6_collider->AddColliderData(obstacle_6_colliderData);

	obstacle_6->RegisterComponent(E_Component::Component_Collider, obstacle_6_collider, true);

	//movement
	Movement_Breakable* obstacle_6_breakable_movement = new Movement_Breakable(obstacle_6, true, false, false, WALL_HP, 0.0f);
	obstacle_6->AddMovement(obstacle_6_breakable_movement);

	//NPC
	//7
	vector3s tempNpc_1_pos = vector3s(900.0f, 300.0f, 0.0f);
	Obstacle* npc_1 = new Obstacle();
	npc_1->SetObstacleType(E_Obstacle::Obstacle_Breakable);
	npc_1->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	npc_1->SetPos(vector3s(900.0f, 300.0f, 0.0f), true);

	DrawData* npc_1_drawData = new DrawData();
	npc_1_drawData->shakeWithCamera = true;
	npc_1_drawData->SetSize(character_size);
	npc_1_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\character_package.png"));
	npc_1_drawData->texIndex = vector2s(NPC_IDLE_INDEX_X, NPC_IDLE_INDEX_Y);
	npc_1_drawData->tex_w_count = NPC_TEX_W_COUNT;
	npc_1_drawData->tex_texel_size = character_cell_tex_size;
	npc_1_drawData->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	npc_1->SetDrawData(npc_1_drawData);

	Collider* npc_1_collider = new Collider();
	ColliderData* npc_1_colliderData =
		new ColliderData(0, npc_1_collider, false, E_ColliderType::Collider_Rect, npc_1_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	npc_1_collider->AddColliderData(npc_1_colliderData);

	npc_1->RegisterComponent(E_Component::Component_Collider, npc_1_collider, true);

	//animation
	Animation* npc_1_AnimComponent = new Animation();
	if (npc_1->RegisterComponent(E_Component::Component_Animation, npc_1_AnimComponent) == false) {//register component first
		delete npc_1_AnimComponent;
		npc_1_AnimComponent = NULL;
	}
	else {
		//normal
		AnimClip* npc_1_IdleAnim = new AnimClip();
		//int startTexIndex = NPC_TEX_INDEX_Y * NPC_TEX_W_COUNT + NPC_TEX_INDEX_X - 1;
		int startTexIndex = NPC_IDLE_INDEX_Y * NPC_TEX_W_COUNT + NPC_IDLE_INDEX_X;
		int endTexIndex = startTexIndex + NPC_IDLE_FRAME_COUNT;
		npc_1_IdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Idle, npc_1_IdleAnim, true);

		AnimClip* npc_1_MoveAnim = new AnimClip();
		startTexIndex = NPC_MOVE_INDEX_Y * NPC_TEX_W_COUNT + NPC_MOVE_INDEX_X;
		endTexIndex = startTexIndex + NPC_MOVE_FRAME_COUNT;
		npc_1_MoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		npc_1_AnimComponent->RegisterAnim(E_Anim::Anim_Move, npc_1_MoveAnim, false);
	}

	//ai
	AIComponent* npc_1_ai_component = new AIComponent();
	npc_1->RegisterComponent(E_Component::Component_AI, npc_1_ai_component);
	SimplePatrolAI* npc_1_simple_ai = new SimplePatrolAI(npc_1,
		E_PatrolType::PatrolType_LoopCircle, npc_1->GetTransform()->GetPos(), 1.0f, true);
	npc_1_ai_component->SetAI(npc_1_simple_ai);
	//set path
	npc_1_simple_ai->AddPatrolPoint(npc_1->GetTransform()->GetPos());
	npc_1_simple_ai->AddPatrolPoint(vector3s(0.0f, 100.0f, 0.0f) + tempNpc_1_pos);
	npc_1_simple_ai->AddPatrolPoint(vector3s(100.0f, 100.0f, 0.0f) + tempNpc_1_pos);

	//movement
	Movement_Breakable* npc_1_breakable_movement = new Movement_Breakable(npc_1, true, false, false, NPC_HP, 0.0f);
	npc_1->AddMovement(npc_1_breakable_movement);


	//target point
	BaseCell* targetPoint = new BaseCell();
	targetPoint->SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);
	targetPoint->SetCellType(E_CellType::CellType_TargetPoint);
	targetPoint->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	targetPoint->SetPos(vector3s(SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT * 0.5f, 0.0f), true);

	DrawData* targetPoint_drawData = new DrawData();
	targetPoint_drawData->shakeWithCamera = true;
	targetPoint_drawData->SetSize(vector2s(100.0f, 100.0f));
	targetPoint_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\targetArrow.png"));
	targetPoint_drawData->usePreComputeUV = true;
	targetPoint_drawData->preComputeUV = vector2s(0, 0);
	targetPoint_drawData->preComputeUVSize = vector2s(1, 1);
	targetPoint_drawData->tex_texel_size = vector2s(1, 1);
	//targetPoint_drawData->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.1f));
	targetPoint->SetDrawData(targetPoint_drawData);

	Collider* targetPoint_collider = new Collider();
	ColliderData* targetPoint_colliderData =
		new ColliderData(0, targetPoint_collider, false, E_ColliderType::Collider_Rect, targetPoint_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	targetPoint_collider->AddColliderData(targetPoint_colliderData);

	targetPoint->RegisterComponent(E_Component::Component_Collider, targetPoint_collider, true);


#pragma endregion obstacle



	this->RegisterSceneObj(obstacle_1);
	this->RegisterSceneObj(obstacle_2);
	this->RegisterSceneObj(obstacle_3);
	this->RegisterSceneObj(obstacle_4);
	this->RegisterSceneObj(obstacle_5);
	this->RegisterSceneObj(obstacle_6);
	this->RegisterSceneObj(npc_1);

	this->RegisterSceneObj(targetPoint);
	Scene::Load();
}

void Scene_6::Unload() {
	Scene::Unload();

	//hide game obj

}

void Scene_6::Reset() {
	Scene::Reset();
}

void Scene_6::DoUpdate(float deltatime) {
	Scene::DoUpdate(deltatime);
}

void Scene_6::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void Scene_6::OnSceneLoad() {
	Scene::OnSceneLoad();


	//show hud
	//GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	//GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);
	//GameMgr::GetInstance()->inputMgr->SetInputState(true);

	//GameMgr::GetInstance()->cellSpawnerMgr->StartSpawn();

}

void Scene_6::OnSceneUnload() {
	Scene::OnSceneUnload();

}


#pragma endregion scene_6



//=================================
// 追加松尾
#pragma region bossscene

BossScene::BossScene() :Scene(E_Scene::Scene_Boss) {

}

BossScene::~BossScene() {

}

void BossScene::Load() {
	Unload();
	vector2s obstacle_size = vector2s(0, 0);
	MainCamera::GetInstance()->SetScrollState(false);
#pragma region wall

	//back ground
	BaseCell* bg = new TempBackground();
	vector3s pos = vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
	bg->SetPos(pos, true);
	bg->SetState(true);
	this->RegisterSceneObj(bg);



	obstacle_size.x = 200.0;
	obstacle_size.y = 100.0f;

	//1(wall_down
	BaseCell* wallcell = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_1 = (Obstacle*)wallcell;
	wall_1->GetTransform()->SetPos(vector3s(100, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_1->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell2 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_2 = (Obstacle*)wallcell2;
	wall_2->GetTransform()->SetPos(vector3s(320, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_2->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell3 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_3 = (Obstacle*)wallcell3;
	wall_3->GetTransform()->SetPos(vector3s(540, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_3->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell4 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_4 = (Obstacle*)wallcell4;
	wall_4->GetTransform()->SetPos(vector3s(760, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_4->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell5 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_5 = (Obstacle*)wallcell5;
	wall_5->GetTransform()->SetPos(vector3s(980, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_5->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell6 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_6 = (Obstacle*)wallcell6;
	wall_6->GetTransform()->SetPos(vector3s(1200, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_6->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell7 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_7 = (Obstacle*)wallcell7;
	wall_7->GetTransform()->SetPos(vector3s(1420, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_7->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell8 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_8 = (Obstacle*)wallcell8;
	wall_8->GetTransform()->SetPos(vector3s(1640, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_8->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	BaseCell* wallcell9 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_9 = (Obstacle*)wallcell9;
	wall_9->GetTransform()->SetPos(vector3s(1860, SCREEN_HEIGHT - obstacle_size.y, 0.0f));
	wall_9->GetTransform()->SetScale(vector3s(0.5, 1.2, 1));

	//2(wall_left,right
	BaseCell* wallcell10 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_10 = (Obstacle*)wallcell10;
	wall_10->GetTransform()->SetPos(vector3s(38, SCREEN_HEIGHT - 268, 0.0f));
	wall_10->GetTransform()->SetScale(vector3s(0.4, 2.5, 1));


	BaseCell* wallcell11 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_11 = (Obstacle*)wallcell11;
	wall_11->GetTransform()->SetPos(vector3s(1860, SCREEN_HEIGHT - 268, 0.0f));
	wall_11->GetTransform()->SetScale(vector3s(0.4, 2.5, 1));

	//2(wall_up
	BaseCell* wallcell12 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_12 = (Obstacle*)wallcell12;
	wall_12->GetTransform()->SetPos(vector3s(500, SCREEN_HEIGHT - 500, 0.0f));
	wall_12->GetTransform()->SetScale(vector3s(0.4, 0.9, 1));


	BaseCell* wallcell13 = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Ground);
	Obstacle* wall_13 = (Obstacle*)wallcell13;
	wall_13->GetTransform()->SetPos(vector3s(1300, SCREEN_HEIGHT - 500, 0.0f));
	wall_13->GetTransform()->SetScale(vector3s(0.4, 0.9, 1));

	//Item
	BaseCell* onigiricell = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Onigiri);
	Onigiri* onigiri = (Onigiri*)onigiricell;
	onigiri->GetTransform()->SetPos(vector3s(900, SCREEN_HEIGHT - 800, 0.0f));
	//onigiri->GetTransform()->SetScale(vector3s(0.5, 0.5, 1));
	onigiri->Reset();
	m_onigiri = onigiri;

	wall[0] = wall_1;
	wall[1] = wall_2;
	wall[2] = wall_3;
	wall[3] = wall_4;
	wall[4] = wall_5;
	wall[5] = wall_6;
	wall[6] = wall_7;
	wall[7] = wall_8;
	wall[8] = wall_9;
	wall[9] = wall_10;
	wall[10] = wall_11;
	wall[11] = wall_12;
	wall[12] = wall_13;

#pragma endregion wall


#pragma region boss

	Boss* boss = new Boss();
	boss->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	boss->SetPos(vector3s(SCREEN_WIDTH * 0.50f, SCREEN_HEIGHT * 0.78, 0.0f), true);
	boss->SetRot(vector3s(0.0f, 0.0f, 0.0f));
	DrawData* boss_drawData = new DrawData();
	boss_drawData->shakeWithCamera = true;
	boss_drawData->SetSize(vector2s(500.0f, 350.0f));
	boss_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\BossAniSheet.png"));
	boss_drawData->texIndex = vector2s(TEX_BOSS_ANIM_IDLE_START_INDEX_X, TEX_BOSS_ANIM_IDLE_START_INDEX_Y);
	boss_drawData->tex_w_count = TEX_BOSS_W_COUNT;
	boss_drawData->tex_texel_size = boss_cell_tex_size;
	boss->SetDrawData(boss_drawData);
	//===================アニメーション
	Animation* animation = new Animation();
	boss->RegisterComponent(E_Component::Component_Animation, animation, true);

	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_BOSS_ANIM_IDLE_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_IDLE_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_BOSS_ANIM_IDLE_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	AnimClip* moveAnim = new AnimClip();
	startTexIndex = TEX_BOSS_ANIM_MOVE_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_MOVE_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_BOSS_ANIM_MOVE_FRAME_COUNT;
	moveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Move, moveAnim, true);

	AnimClip* flyAnim = new AnimClip();
	startTexIndex = TEX_BOSS_ANIM_FLY_START_INDEX_Y * TEX_BOSS_W_COUNT + TEX_BOSS_ANIM_FLY_START_INDEX_X;
	endTexIndex = startTexIndex + TEX_BOSS_ANIM_FLY_FRAME_COUNT;
	flyAnim->SetAnimParam(E_Anim::Anim_Fly, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Fly, flyAnim, true);


	Collider* boss_collider = new Collider();
	ColliderData* boss_colliderData = new ColliderData(0, boss_collider, false, E_ColliderType::Collider_Rect, boss_drawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));

	boss_collider->AddColliderData(boss_colliderData);
	boss->RegisterComponent(E_Component::Component_Collider, boss_collider, true);


	enemyboss = boss;
	this->RegisterSceneObj(boss);


#pragma endregion boss

	for (int index = 0; index < 13; index++) {
		this->RegisterSceneObj(wall[index]);
	}
	this->RegisterSceneObj(m_onigiri);
	this->RegisterSceneObj(enemyboss);

	Scene::Load();
}

void BossScene::Unload() {

	Scene::Unload();
	//hide game obj
	for (int index = 0; index < 13; index++) {
		if (wall[index] != NULL) {
			wall[index] = nullptr;
		}
	}
	enemyboss = nullptr;
	m_onigiri = nullptr;
}

void BossScene::Reset() {
	Scene::Reset();
}

void BossScene::DoUpdate(float deltatime) {

	OnigiriAppear();
	BlockAppear();
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	vector3s playerPos = player->GetTransform()->GetPos();
	if (playerPos.x < 10)
		playerPos.x = 10;
	if (playerPos.x > SCREEN_WIDTH - 80)
		player->GetTransform()->SetPos(vector3s(SCREEN_WIDTH - 80, playerPos.y, playerPos.z));
	if (playerPos.y > 850)
		player->GetTransform()->SetPos(vector3s(playerPos.x, 850, playerPos.z));



	if (enemyboss->GetStatus() == BOSS_STS_WEAKENING)
	{
		wall[11]->SetState(false);
		wall[12]->SetState(false);
	}
	else
	{
		wall[11]->SetState(true);
		wall[12]->SetState(true);
	}


	Scene::DoUpdate(deltatime);
}

void BossScene::BlockAppear() {


	if (setFrame > 60 * 7 && enemyboss->GetStatus() != BOSS_STS_WEAKENING)
	{
		for (int i = 0; i < 13; i++)
		{
			wall[i]->SetState(true);
			setFrame = 0;
		}

	}
	setFrame++;

}

void BossScene::OnigiriAppear()
{

	if (setFrame2 > 60 * 30)
	{
		if (m_onigiri != nullptr) {
			//BaseCell* onigiricell = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Onigiri);
			//BaseCell* onigiri = (BaseCell*)onigiricell;
			m_onigiri->GetTransform()->SetPos(vector3s(900, SCREEN_HEIGHT - 800, 0.0f));
			//onigiri->GetTransform()->SetScale(vector3s(0.5, 0.5, 1));
			m_onigiri->Reset();
			m_onigiri->SetState(true);
		}
		else {
			BaseCell* onigiricell = ObjectPoolMgr::CreateNewCell(E_ObjCellType::ObjCellType_Onigiri);
			Onigiri* onigiri = (Onigiri*)onigiricell;
			onigiri->GetTransform()->SetPos(vector3s(900, SCREEN_HEIGHT - 800, 0.0f));
			//onigiri->GetTransform()->SetScale(vector3s(0.5, 0.5, 1));
			onigiri->Reset();
			m_onigiri = onigiri;
			m_onigiri->SetState(true);
			this->RegisterSceneObj(m_onigiri);
		}

		setFrame2 = 0;
	}
	setFrame2++;
}


void BossScene::DoLateUpdate(float deltatime) {
	Scene::DoLateUpdate(deltatime);
}

void BossScene::OnSceneLoad() {
	GameMgr::GetInstance()->sceneObjMgr->UninitCornetState();
	GameMgr::GetInstance()->uiMgr->DrawBar(false);
	GameMgr::GetInstance()->uiMgr->DrawScore(false);
	//23.1.25--陳　
	//===============テスト用
	GameMgr::GetInstance()->uiMgr->ResetLifeUI();
	//===============

	Scene::OnSceneLoad();
}

void BossScene::OnSceneUnload() {
	Scene::OnSceneUnload();
}


#pragma endregion bossScene


#pragma endregion scene
