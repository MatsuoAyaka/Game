#include "MapGenerator.h"
#include "Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "EventCenter.h"
#include <algorithm>
//sadhusaidhuiashdiuhasiuhdius

MapGenerator* MapGenerator::instance = NULL;
MapGenerator::MapGeneratorDeletor mapGeneratorDeletor;


#pragma region constDef
const int SmallGroundMinWidth = SCREEN_WIDTH / 6 - 50;
const int SmallGroundMaxWidth = SCREEN_WIDTH / 6 + 50;

const int MediumGroundMinWidth = SCREEN_WIDTH / 3 - 50;
const int MediumGroundMaxWidth = SCREEN_WIDTH / 3 + 50;

const int HugeGroundMinWidth = SCREEN_WIDTH - 50;
const int HugeGroundMaxWidth = SCREEN_WIDTH;

const int GroundMinHeight = SCREEN_HEIGHT / 8 - 30;
const int GroundMaxHeight = SCREEN_HEIGHT / 8 + 30;

const int BetweenGroundMaxDistance = 100;

const int GroundMinPosY = SCREEN_HEIGHT - 300;
const int GroundMaxPosY = SCREEN_HEIGHT - 400;

const int IceRandomXRange = 60;
const int IceRandomCreate = 50;
const int StoneRandomCreate = 80;
const D3DXCOLOR ice_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR stone_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR wind_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR springGround_color = D3DXCOLOR(0.1f, 0.85f, 0.1f, 1.0f);
const D3DXCOLOR beltConveyor_color = D3DXCOLOR(0.85f, 0.1f, 0.1f, 1.0f);
const D3DXCOLOR moveGround_color = D3DXCOLOR(0.85f, 0.1f, 0.85f, 1.0f);
const D3DXCOLOR appearGround_color = D3DXCOLOR(0.2f, 0.85f, 0.85f, 1.0f);


const int WIND_IDLE_INDEX_Y = 0;
const int WIND_TEX_W_COUNT = 3;
const int WIND_IDLE_INDEX_X = 0;
const int WIND_IDLE_FRAME_COUNT = 6;

typedef void (MapGenerator::* fp)(void);

#pragma endregion constDef

#pragma region Fake
void Invoke(fp _func, float _time)
{
	//if (Wait(_time))
	//	void _func();
	;
}
//Invoke(&MapGenerator::CreateObstacle, 2.0f);

BaseCell* GetObjFromPoolMgr(int _type)
{
	if (_type == 0)
	{
		//int randType = rand() % 3;

		//return new Ground((E_GroundType)randType);
		return new Ground(E_GroundType::Small);
	}

	if (_type == 1)
		return new Ice();

	if (_type == 2)
		return new Stone();

	if (_type == 3)
		return new Wind();

	if (_type == 4)
		return new SpringGround();

	if (_type == 5)
		return new BeltConveyor();

	if (_type == 6)
		return new MoveGround();

	if (_type == 7)
		return new AppearGround();
	return nullptr;
}

void MapGenerator::MapUpdate()
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		RemoveGround();
	}

	if (GetKeyboardTrigger(DIK_I))
	{
		CreateIce();
	}

	//テスト2022.11.10
	//距離によってブロックを削除(フレイムごとに　今配列の中で最初のブロックだけを処理する
	if (obstacleDic[E_MapObstacleType::Ground_Obs].empty() == false) {

		std::vector<BaseCell*> tempVec = obstacleDic[E_MapObstacleType::Ground_Obs];

		Ground* tempGround = (Ground*)tempVec.front();

		Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
		if (player != NULL && tempGround != NULL) {
			if (player->GetTransform()->GetPos().x - tempGround->GetTransform()->GetPos().x >= SCREEN_WIDTH) {
				RemoveGround();
			}
		}
	}

}

#pragma endregion Fake

#pragma region MapGenerator
MapGenerator::MapGenerator()
{
	obstacleDic.clear();
	curStageDic.clear();
}
MapGenerator::~MapGenerator()
{
	obstacleDic.clear();
}

BaseCell* MapGenerator::CreateItem(E_CellType _itemType)
{

	//return GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);

	BaseCell* tempCell = nullptr;

	switch (_itemType)
	{
	case E_CellType::CellType_Obstacle:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
		break;
	case E_CellType::CellType_Ice:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Ice, 0);
		break;
		//敵
	case E_CellType::CellType_Enemy:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Enemy, 1, E_EnemyType::Enemy_Hiyoko);
		break;
	case E_CellType::CellType_Enemy_Dumpling:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Enemy_Dumpling, 1, E_EnemyType::Enemy_Hiyoko);
		break;
	case E_CellType::CellType_Enemy_Ittanmomen:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Enemy_Ittanmomen, 1, E_EnemyType::Enemy_Karasu);
		break;
	case E_CellType::CellType_Enemy_Cornet:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Enemy_Cornet, 1, E_EnemyType::Enemy_Nezumi);
		break;
	case E_CellType::CellType_Enemy_Pasta:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Enemy_Pasta, 1, E_EnemyType::Enemy_Monkey);
		break;
	case E_CellType::CellType_Boss:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Boss, 0);
		break;

	case E_CellType::CellType_Wind:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Wind, 0);
		break;
	case E_CellType::CellType_BeltConveyor:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_BeltConveyor, 0);
		break;
	case E_CellType::CellType_SpringGround:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_SpringGround, 0);
		break;
	case E_CellType::CellType_MoveGround:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_MoveGround, 0);
		break;
	case E_CellType::CellType_AppearGround:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_AppearGround, 0);
		break;

	case E_CellType::CellType_Bullet:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Bullet, 0);
		break;
	case E_CellType::CellType_Bullet_Fireball:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Bullet_Fireball, 0);
		break;

	case E_CellType::CellType_Onigiri:
	case E_CellType::CellType_Sugar:
	case E_CellType::CellType_Pepper:
	case E_CellType::CellType_Pickled_Plums:
	case E_CellType::CellType_Miso_Soup:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(_itemType, 0);
		break;
	case E_CellType::CellType_Effect_Hit:
	case E_CellType::CellType_Effect_Jump:
	case E_CellType::CellType_Effect_Rush:
	case E_CellType::CellType_Effect_Recover:
	case E_CellType::CellType_Effect_Button:
		tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(_itemType,0);
		break;

	}

	return tempCell;
}
void MapGenerator::CreateGround()
{
 	Ground* tempGround = NULL;
	//tempGround = (Ground*)GetObjFromPoolMgr(0);
	BaseCell* tempCell = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
	if (tempCell == NULL)return;
	//tempGround = dynamic_cast<Ground*>(tempCell);
	tempGround = (Ground*)tempCell;

	if (tempGround == NULL) {
 		return;
	}

	//地面ブロックのサイズをランダム生成
	tempGround->ResetSize();

	//現在の地面リストの末尾からランダム位置に設置
	if (obstacleDic[E_MapObstacleType::Ground_Obs].empty())
	{
		tempGround->SetPos(vector3s(rand() % BetweenGroundMaxDistance + (int)(tempGround->GetSize(true).x / 2),
			GroundMinPosY + rand() % (GroundMaxPosY - GroundMinPosY),
			0), true);
	}
	else 
	{
		//リストの末尾ブロックの右端X座標
		int tempX = obstacleDic[E_MapObstacleType::Ground_Obs].back()->GetTransform()->GetPos().x + 
			obstacleDic[E_MapObstacleType::Ground_Obs].back()->GetSize(true).x;
		tempGround->SetPos(vector3s(tempX + rand() % BetweenGroundMaxDistance + (int)(tempGround->GetSize(true).x / 2),
			GroundMinPosY + rand() % (GroundMaxPosY - GroundMinPosY),
			0), true);
	}

	//sceneMgr->GetCurScene()->RegisterSceneObj(tempGround, false);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempGround);
	GameMgr::GetInstance()->sceneMgr->GetCurScene()->SetGround(tempGround,E_EnemyType::Enemy_Monkey);
	//GameMgr::GetInstance()->sceneObjMgr->RegisterCell(tempGround, false);
	obstacleDic[E_MapObstacleType::Ground_Obs].push_back(tempGround);
}

void MapGenerator::CreateIce()
{
	Ground* tempGround;
	Ice* tempIce;
	
	if (obstacleDic[E_MapObstacleType::Ground_Obs].empty())
		return;

	//氷が付着する地面をゲット
	//メモリプールから氷のインスタンスをゲット
	tempGround = (Ground*)obstacleDic[E_MapObstacleType::Ground_Obs][obstacleDic[E_MapObstacleType::Ground_Obs].size()-1];
	if (tempGround->iceCreated == true) {
		return;
	}
	tempIce = (Ice*)GetObjFromPoolMgr(1);


	tempIce->SetGround(tempGround);
	tempIce->ResetSize();

	//氷の位置をリセット
	vector3s groundPos = tempGround->GetTransform()->GetPos();
	vector3s offset = vector3s(0.0f, tempGround->GetSize(true).y / 2 + tempIce->GetSize(true).y / 2, 0.0f);
	tempIce->SetPos(groundPos - offset, true);

	//GameMgr::GetInstance()->sceneObjMgr->RegisterCell(tempIce, false);
	//GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempGround);
	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempIce);

	obstacleDic[E_MapObstacleType::Ice_Obs].push_back(tempIce);
}

void MapGenerator::CreateStone()
{
	Ground* tempGround = nullptr;
	Stone* tempStone;

	if (obstacleDic[E_MapObstacleType::Ground_Obs].empty())
		return;

	//石が付着する地面をゲット
	for (auto& ground : obstacleDic[E_MapObstacleType::Ground_Obs])
	{
		Ground* g = (Ground*)ground;
		if (!g->GetBStone())
		{
			tempGround = g;
			//石設置済みフラグ
			g->SetBStone(true);

			//ランダム生成
			if (rand() % 100 > StoneRandomCreate)
				return;
			break;
		}
		else
		{
			continue;
		}
	}

	//ループ終了時に地面が見つからなかった場合、return
	if (tempGround == nullptr)
		return;

	//メモリプールから石のインスタンスをゲット
	tempStone = (Stone*)GetObjFromPoolMgr(2);

	tempStone->SetGround(tempGround);

	vector3s groundPos = tempGround->GetTransform()->GetPos();
	vector3s offset = vector3s(0.0f, tempGround->GetSize(true).y / 2 + tempStone->GetSize(true).y / 2, 0.0f);
	tempStone->SetPos(groundPos - offset, true);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempStone);
	obstacleDic[E_MapObstacleType::Stone_Obs].push_back(tempStone);
}

void MapGenerator::CreateWind(vector3s pos)
{
	//メモリプールから風のインスタンスをゲット
	Wind* tempWind = (Wind*)GetObjFromPoolMgr(3);
	tempWind->SetPos(pos, true);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempWind);
	obstacleDic[E_MapObstacleType::Wind_Obs].push_back(tempWind);
}

void MapGenerator::CreateSpringGround(vector3s pos)
{
	//メモリプールからパネル地面のインスタンスをゲット
	SpringGround* tempSpringGround = (SpringGround*)GetObjFromPoolMgr(4);
	tempSpringGround->SetPos(pos, true);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempSpringGround);
	obstacleDic[E_MapObstacleType::SpringGround_Obs].push_back(tempSpringGround);
}
void MapGenerator::CreateBeltConveyor(vector3s pos)
{
	//メモリプールからベルトコンベアのインスタンスをゲット
	BeltConveyor* tempBeltConveyor = (BeltConveyor*)GetObjFromPoolMgr(5);
	tempBeltConveyor->SetPos(pos, true);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempBeltConveyor);
	obstacleDic[E_MapObstacleType::BeltConveyor_Obs].push_back(tempBeltConveyor);
}
void MapGenerator::CreateMoveGround(vector3s pos, vector3s start, vector3s end)
{
	MoveGround* tempMoveGround = (MoveGround*)GetObjFromPoolMgr(6);
	tempMoveGround->SetPos(pos, true);
	((Movement_Axis*)tempMoveGround->GetMovementDic().begin()->second)->ResetStartEndPos(start, end);

	tempMoveGround->ResetSpeed();
	tempMoveGround->SetVec(VecNormalize(end - start));

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempMoveGround);
	obstacleDic[E_MapObstacleType::MoveGround_Obs].push_back(tempMoveGround);
}
void MapGenerator::CreateAppearGround(vector3s pos)
{
	AppearGround* tempAppearGround = (AppearGround*)GetObjFromPoolMgr(7);
	tempAppearGround->SetPos(pos, true);

	GameMgr::GetInstance()->sceneMgr->GetCurScene()->RegisterSceneObj(tempAppearGround);
	obstacleDic[E_MapObstacleType::AppearGround_Obs].push_back(tempAppearGround);
}
void MapGenerator::RemoveGround()
{
	if (obstacleDic[E_MapObstacleType::Ground_Obs].empty())
		return;

	TestScene* curScene;// = GameMgr::GetInstance()->sceneMgr->GetCurScene();

	std::vector<BaseCell*> tempVec = obstacleDic[E_MapObstacleType::Ground_Obs];

	Ground* tempGround = (Ground*)tempVec.front();

	if (obstacleDic[E_MapObstacleType::Ice_Obs].empty() == false && tempGround->ice!=NULL) {
		std::vector<BaseCell*> tempIceVec = obstacleDic[E_MapObstacleType::Ice_Obs];
		Ice* tempIce = (Ice*)tempIceVec.front();
 		GameMgr::GetInstance()->sceneMgr->GetCurScene()->UnregisterSceneObj(tempIce, false);

		obstacleDic[E_MapObstacleType::Ice_Obs].erase(obstacleDic[E_MapObstacleType::Ice_Obs].begin());

	}

	//curScene->UnregisterSceneObj(obstacleDic[E_MapObstacleType::Ground_Obs].front(), false);
	//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(tempGround);
	GameMgr::GetInstance()->sceneMgr->GetCurScene()->UnregisterSceneObj(tempGround, false);
	//tempGround->DeleteCell();

	//リスト先頭要素を取り出し
	//tempVec.erase(remove(tempVec.begin(), tempVec.end(), tempGround), tempVec.end());
	obstacleDic[E_MapObstacleType::Ground_Obs].erase(obstacleDic[E_MapObstacleType::Ground_Obs].begin());
}

void MapGenerator::RemoveIce()
{
	if (obstacleDic[E_MapObstacleType::Ice_Obs].empty())
		return;

	Scene* curScene = GameMgr::GetInstance()->sceneMgr->GetCurScene();

	std::vector<BaseCell*> tempVec = obstacleDic[E_MapObstacleType::Ice_Obs];

	curScene->UnregisterSceneObj(obstacleDic[E_MapObstacleType::Ice_Obs].front(), false);

	obstacleDic[E_MapObstacleType::Ice_Obs].erase(obstacleDic[E_MapObstacleType::Ice_Obs].begin());
}

void MapGenerator::RemoveStone()
{
	if (obstacleDic[E_MapObstacleType::Stone_Obs].empty())
		return;

	Scene* curScene = GameMgr::GetInstance()->sceneMgr->GetCurScene();

	std::vector<BaseCell*> tempVec = obstacleDic[E_MapObstacleType::Stone_Obs];

	curScene->UnregisterSceneObj(obstacleDic[E_MapObstacleType::Stone_Obs].front(), false);

	obstacleDic[E_MapObstacleType::Stone_Obs].erase(obstacleDic[E_MapObstacleType::Stone_Obs].begin());
}

void MapGenerator::RemoveAll()
{
	for (auto& vec : obstacleDic)
	{
		switch (vec.first)
		{
		case E_MapObstacleType::Ground_Obs:
			for (auto& stuff : vec.second)
				RemoveGround();
			break;
		case E_MapObstacleType::Ice_Obs:
			for (auto& stuff : vec.second)
				RemoveIce();
			break;
		case E_MapObstacleType::Stone_Obs:
			for (auto& stuff : vec.second)
				RemoveStone();
			break;
		default:
			break;
		}
	}
}

void MapGenerator::ClearList()
{
	obstacleDic.clear();
}

#pragma endregion MapGenerator

#pragma region Ground
Ground::Ground(E_GroundType _type) : m_type(_type), bIce(false), bStone(false)
{
	objCellType = E_ObjCellType::ObjCellType_Ground;

	ice = NULL;
	iceCreated = false;

	TileData* tileData_ground = new TileData();
	tileData_ground->cornerSize_1 = vector2s(88.0f, 102.0f);
	tileData_ground->cornerSize_2 = vector2s(87.0f, 72.0f);
	tileData_ground->borderTileType = E_TileType::TileType_Repeat;
	tileData_ground->centerTileType = E_TileType::TileType_Repeat;
	//tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->cornerScale_1 = vector2s(0.5f, 0.5f);
	tileData_ground->cornerScale_2 = vector2s(0.5f, 0.5f);
	tileData_ground->topBorderTex = LoadTexture((char*)"data\\texture\\block_soil_top.png");
	//tileData_ground->topBorderTexSize = vector2s(320, 98);
	tileData_ground->topBorderTexSize = vector2s(146.0f, 102.0f);
	tileData_ground->bottomBorderTex = LoadTexture((char*)"data\\texture\\block_soil_down.png");
	tileData_ground->bottomBorderTexSize = vector2s(145.0f, 72.0f);
	tileData_ground->rightBorderTex = LoadTexture((char*)"data\\texture\\block_soil_right.png");
	tileData_ground->rightBorderTexSize = vector2s(87.0f, 144.0f);
	tileData_ground->leftBorderTex = LoadTexture((char*)"data\\texture\\block_soil_left.png");
	tileData_ground->leftBorderTexSize = vector2s(88.0f, 144.0f);
	tileData_ground->centerTex = LoadTexture((char*)"data\\texture\\block_soil_center.png");
	tileData_ground->centerTexSize = vector2s(144.0f, 144.0f);
	//tileData_ground->centerTexSize = vector2s(292, 320);

	tileData_ground->tb_repeatCount = vector2s(-1, -1);
	tileData_ground->lr_repeatCount = vector2s(-1, -1);

	tileData_ground->tb_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->lr_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->center_texScale = vector2s(0.5f, 0.5f);

	SetObstacleType(E_Obstacle::Obstacle_Const);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(500.0f, 100.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\block.png"));
	groundDrawData->usePreComputeUV = true;
	groundDrawData->preComputeUV = vector2s(0, 0);
	groundDrawData->preComputeUVSize = vector2s(320.0f, 320.0f);
	groundDrawData->tex_texel_size = vector2s(1 / 320.0f, 1 / 320.0f);
	//groundDrawData->SetColor(wall_color);
	groundDrawData->SetDrawDataType(E_DrawDataType::DrawData_Tile);
	groundDrawData->SetTileData(tileData_ground);
	SetDrawData(groundDrawData);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, true, E_ColliderType::Collider_Rect, 
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);
}

Ground::~Ground()
{
	ice = NULL;
}

void Ground::ResetSize()
{
	vector2s ground_size = vector2s(0, 0);

	//呼び出された時に、地面ブロックの幅を一定範囲内ランダム生成
	switch (m_type)
	{
	case E_GroundType::Small:
		ground_size.x = SmallGroundMinWidth + (rand() % (SmallGroundMaxWidth - SmallGroundMinWidth));
		break;
	case E_GroundType::Medium:
		ground_size.x = MediumGroundMinWidth + (rand() % (MediumGroundMaxWidth - MediumGroundMinWidth));
		break;
	case E_GroundType::Huge:
		ground_size.x = HugeGroundMinWidth + (rand() % (HugeGroundMaxWidth - HugeGroundMinWidth));
		break;
	default:
		break;
	}

	//地面ブロックの丈を一定範囲内ランダム生成
	ground_size.y = GroundMinHeight + (rand() % (GroundMaxHeight - GroundMinHeight));

	DrawData* drawData = GetDrawData();

	this->GetID();

	Collider* groundCollider = (Collider*)GetComponent(E_Component::Component_Collider);

	drawData->SetSize(ground_size);
	groundCollider->SetColliderSize(ground_size, 0);

}

void Ground::SetBIce(bool _b)
{
	bIce = _b;
}
bool Ground::GetBIce() const
{
	return bIce;
}
void Ground::SetBStone(bool _b)
{
	bStone = _b;
}
bool Ground::GetBStone() const
{
	return bStone;
}

void Ground::Recycle() {
 	GameMgr::GetInstance()->sceneObjMgr->RecycleCell(this);
}

void Ground::OnCellInit()
{
	ice = NULL;
	bIce = false;
	this->SetState(true);
}
void Ground::OnCellRecycle()
{
	ice = NULL;
	bIce = false;
	Obstacle::OnCellRecycle();
	this->SetState(false);
}
void Ground::DeleteCell()
{
	this->Recycle();
}
#pragma endregion Ground

#pragma region Ice
Ice::Ice()
{
	m_ground = NULL;

	cellType = E_CellType::CellType_Ice;
	SetObstacleType(E_Obstacle::Obstacle_Const);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(100.0f, 50.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	//groundDrawData->usePreComputeUV = true;
	//groundDrawData->preComputeUV = vector2s(0, 0);
	//groundDrawData->preComputeUVSize = vector2s(1, 1);
	//groundDrawData->tex_texel_size = vector2s(1, 1);
	groundDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ICE_START_INDEX_X, TEX_ENVIRONMENT_ICE_START_INDEX_Y);
	groundDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	groundDrawData->tex_texel_size = environment_item_cell_tex_size;
	groundDrawData->SetColor(ice_color);
	SetDrawData(groundDrawData);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, false, E_ColliderType::Collider_Rect,
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);
}
Ice::~Ice()
{
	m_ground = NULL;
}
void Ice::SetGround(Ground* _ground)
{
	m_ground = _ground;

	//22.11.10
	if (_ground != NULL) {
		_ground->ice = this;
		_ground->iceCreated = true;
	}
}
void Ice::ResetSize()
{
	if (m_ground == nullptr)
		return;

	//size
	vector2s ice_size = m_ground->GetSize(true);

	//ice_size.x += rand() % IceRandomXRange - IceRandomXRange / 2;
	float xSizeScale = (rand() % 20 + 80) * 0.01f;
	ice_size.x *= xSizeScale;
	if ((m_ground->GetSize(true).x - ice_size.x) < IceRandomXRange) {
		ice_size.x += GetAbsRandomRangeValueI(m_ground->GetSize(true).x - ice_size.x);
	}
	else {
		ice_size.x += GetAbsRandomRangeValueI(IceRandomXRange);
	}
	if (ice_size.x < 0) {
		ice_size.x = IceRandomXRange;
	}
	ice_size.y = 100.0f;
	//リセットしたサイズをDrawDataおよびColliderへ設置
	DrawData* drawData = GetDrawData();
	Collider* iceCollider = (Collider*)GetComponent(E_Component::Component_Collider);
	drawData->SetSize(ice_size);
	iceCollider->SetColliderSize(ice_size, 0);

	//pos
	vector3s groundPos = m_ground->GetTransform()->GetPos();
	vector3s offset = vector3s(0.0f, m_ground->GetSize(true).y *0.3f /*+ this->GetSize(true).y * 0.5f * 0.6f*/, 0.0f);
	this->SetPos(groundPos - offset, true);
}

void Ice::SetParent(BaseCell* _parent)
{
	if (_parent != NULL && _parent->GetCellType() == E_CellType::CellType_Obstacle) {
		//SetGround((Ground*)_parent);
		m_ground = (Ground*)_parent;
	}

	Obstacle::SetParent(_parent);
}

void Ice::Reset()
{
	Obstacle::Reset();
	ResetSize();
}

#pragma endregion Ice

#pragma region Stone
Stone::Stone()
{
	SetObstacleType(E_Obstacle::Obstacle_Const);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	DrawData* stoneDrawData = new DrawData();
	stoneDrawData->shakeWithCamera = true;
	stoneDrawData->SetSize(vector2s(100.0f, 100.0f));
	stoneDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\SmallRock.png"));
	stoneDrawData->usePreComputeUV = true;
	stoneDrawData->preComputeUV = vector2s(0, 0);
	stoneDrawData->preComputeUVSize = vector2s(1, 1);
	stoneDrawData->tex_texel_size = vector2s(1, 1);
	stoneDrawData->SetColor(stone_color);
	SetDrawData(stoneDrawData);

	Collider* stoneCollider = new Collider();
	ColliderData* stoneColliderData = new ColliderData(0, stoneCollider, false, E_ColliderType::Collider_Rect,
		stoneDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	stoneCollider->AddColliderData(stoneColliderData);

	RegisterComponent(E_Component::Component_Collider, stoneCollider, true);
}
Stone::~Stone()
{
	m_ground = NULL;
}
void Stone::SetGround(Ground* _ground)
{
	m_ground = _ground;
}
void Stone::SetParent(BaseCell* _parent)
{
	Obstacle::SetParent(_parent);
}
void Stone::Reset()
{
	Obstacle::Reset();
}
#pragma endregion Stone

#pragma region Wind
Wind::Wind()
{
	SetCellType(E_CellType::CellType_Wind);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_1);
	DrawData* windDrawData = new DrawData();
	windDrawData->shakeWithCamera = true;
	windDrawData->SetSize(vector2s(300.0f, 300.0f));
	windDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\EnvironmentItemAniSheet.png"));
	windDrawData->texIndex = vector2s(TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_X, TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_Y);
	windDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	//windDrawData->usePreComputeUV = true;
	//windDrawData->preComputeUV = vector2s(0, 0);
	//windDrawData->preComputeUVSize = vector2s(1, 1);
	windDrawData->tex_texel_size = environment_item_cell_tex_size;
	windDrawData->SetColor(wind_color);
	SetDrawData(windDrawData);

	Collider* windCollider = new Collider();
	ColliderData* windColliderData = new ColliderData(0, windCollider, true, E_ColliderType::Collider_Rect,
		windDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	windCollider->AddColliderData(windColliderData);
	RegisterComponent(E_Component::Component_Collider, windCollider, true);

	Animation* wind_AnimComponent = new Animation();
	RegisterComponent(E_Component::Component_Animation, wind_AnimComponent);

	AnimClip* wind_Anim = new AnimClip();
	int startTexIndex = TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_Y * TEX_ENVIRONMENT_ITEM_CELL_W_COUNT + TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_ENVIRONMENT_ITEM_WIND_FRAME_COUNT;
	wind_Anim->SetAnimParam(E_Anim::Anim_Wind, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);
	wind_AnimComponent->RegisterAnim(E_Anim::Anim_Wind, wind_Anim, true);
}

Wind::~Wind()
{
}

void Wind::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);

	GameMgr::GetInstance()->sceneObjMgr->CheckWindCollision(this);
}

void Wind::Reset()
{
	BaseCell::Reset();
}

void SceneObjMgr::CheckWindCollision(Wind* wind)
{
	if (wind == NULL)
		return;

	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Enemy)continue;

			if (Collider::CollisionDetect(wind, iter->second)) {
				//Transform::AdjustPosition(monkey, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				EventCenter::GetInstance()->EventTrigger<void>("WindContactPlayer");
			}
		}
	}
}
#pragma endregion Wind


#pragma region SpringGround
SpringGround::SpringGround()
{
	SetCellType(E_CellType::CellType_SpringGround);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(200.0f, 50.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\mochi.png"));
	//groundDrawData->usePreComputeUV = true;
	groundDrawData->texIndex = vector2s(TEX_MOCHI_ANIM_START_INDEX_X, TEX_MOCHI_ANIM_START_INDEX_Y);
	groundDrawData->tex_w_count = TEX_MOCHI_COUNT_W;
	groundDrawData->tex_texel_size = mochi_tex_size;
	//groundDrawData->SetColor(springGround_color);
	SetDrawData(groundDrawData);

	//===========================アニメーション
	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_MOCHI_ANIM_START_INDEX_Y * TEX_MOCHI_COUNT_W + TEX_MOCHI_ANIM_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_MOCHI_ANIM_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.8f, true, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, true, E_ColliderType::Collider_Rect,
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);
}

SpringGround::~SpringGround()
{
}

void SpringGround::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);

	GameMgr::GetInstance()->sceneObjMgr->CheckSpringGroundCollision(this);
}

void SpringGround::Reset()
{
	BaseCell::Reset();
	ResetSize();
}

void SpringGround::ResetSize()
{
	vector2s size = this->GetSize(false);

	//コライダーのサイズを調整
	vector2s colliderSize = vector2s(size.x, size.y * MOCHI_GROUND_COLLIDER_Y_FACTOR);
	vector3s offset = vector3s(0.0f, size.y * MOCHI_GROUND_COLLIDER_Y_OFFSET_FACTOR,0.0f);

	Collider* collider = (Collider*)GetComponent(E_Component::Component_Collider);
	collider->SetColliderSize(colliderSize, 0);
	collider->SetColliderOffset(offset, 0);
}

void SceneObjMgr::CheckSpringGroundCollision(SpringGround* springGround)
{
	return;
	if (springGround == NULL)
		return;

	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Enemy)continue;

			if (Collider::CollisionDetect(springGround, iter->second)) {
				EventCenter::GetInstance()->EventTrigger<void, BaseCell*>("SpringContactPlayer", springGround);
				//((Player*)iter->second)->SetCurGround(springGround);
			}
		}
	}
}

#pragma endregion SpringGround

#pragma region BeltConveyor

BeltConveyor::BeltConveyor()
{
	//SetObstacleType(E_Obstacle::Obstacle_Const);
	SetCellType(E_CellType::CellType_BeltConveyor);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(600.0f, 20.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\belt_conveyor.png"));
	groundDrawData->texIndex = vector2s(TEX_BELTCONVEYOR_START_INDEX_X, TEX_BELTCONVEYOR_START_INDEX_Y);
	groundDrawData->tex_w_count = TEX_ENVIRONMENT_ITEM_CELL_W_COUNT;
	groundDrawData->tex_texel_size = belt_conveyor_cell_tex_size;
	//groundDrawData->SetColor(beltConveyor_color);
	SetDrawData(groundDrawData);

	Animation* animation = new Animation();
	RegisterComponent(E_Component::Component_Animation, animation, true);
	AnimClip* idleAnim = new AnimClip();
	int startTexIndex = TEX_BELTCONVEYOR_START_INDEX_Y * TEX_ENVIRONMENT_ITEM_CELL_W_COUNT + TEX_BELTCONVEYOR_START_INDEX_X;
	int endTexIndex = startTexIndex + TEX_BELTCONVEYOR_ANIM_FRAME_COUNT;
	idleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 1.5f, false, true);
	animation->RegisterAnim(E_Anim::Anim_Idle, idleAnim, true);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, true, E_ColliderType::Collider_Rect,
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);
}

BeltConveyor::~BeltConveyor()
{

}

void BeltConveyor::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);

	GameMgr::GetInstance()->sceneObjMgr->CheckBeltConveyorCollision(this);
}

float BeltConveyor::GetSpeed() const
{
	return m_speed;
}

void BeltConveyor::SetDrawDataSize(vector2s _size)
{
	if (drawData == NULL)return;
	int wCount = (int)(_size.x / TEX_BELTCONVEYOR_CELL_SCALE_SIZE_X + 0.5f);
	if (wCount == TEX_BELTCONVEYOR_W_COUNT) {
		wCount += 1;
	}
	float frameSize_x = (float)wCount / TEX_BELTCONVEYOR_W_COUNT;
	drawData->SetSize(vector2s(_size.x, TEX_BELTCONVEYOR_CELL_SCALE_SIZE_Y));
	drawData->tex_w_count = wCount;
	drawData->tex_texel_size.x = frameSize_x;
}

void SceneObjMgr::CheckBeltConveyorCollision(BeltConveyor* beltConveyor)
{
	if (beltConveyor == NULL)
		return;

	//プレイヤーとの当たり判定
	if (cellTypeDic.find(E_CellType::CellType_Player) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Player];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}
			if (iter->second->GetCellType() == E_CellType::CellType_Enemy)continue;

			if (Collider::CollisionDetect(beltConveyor, iter->second)) {
				EventCenter::GetInstance()->EventTrigger<void, float, bool>("BeltConveyorContactPlayer", beltConveyor->GetSpeed(), false);
			}
		}
	}
}
#pragma endregion BeltConveyor

#pragma region MoveGround
MoveGround::MoveGround() : m_vec(vector3s(0.0f, 0.0f, 0.0f)), m_speed(0.0f), m_movetime(2.0f)
{
	SetCellType(E_CellType::CellType_MoveGround);
	//SetObstacleType(E_Obstacle::Obstacle_Const);
	//SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	//SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	//DrawData* groundDrawData = new DrawData();
	//groundDrawData->shakeWithCamera = true;
	//groundDrawData->SetSize(vector2s(100.0f, 100.0f));
	//groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	//groundDrawData->usePreComputeUV = true;
	//groundDrawData->preComputeUV = vector2s(1, 1);
	//groundDrawData->preComputeUVSize = vector2s(0, 0);
	//groundDrawData->tex_texel_size = vector2s(1, 1);
	//groundDrawData->SetColor(moveGround_color);
	//SetDrawData(groundDrawData);

	TileData* tileData_ground = new TileData();
	tileData_ground->cornerSize_1 = vector2s(88.0f, 102.0f);
	tileData_ground->cornerSize_2 = vector2s(87.0f, 72.0f);
	tileData_ground->borderTileType = E_TileType::TileType_Repeat;
	tileData_ground->centerTileType = E_TileType::TileType_Repeat;
	//tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->cornerScale_1 = vector2s(0.5f, 0.5f);
	tileData_ground->cornerScale_2 = vector2s(0.5f, 0.5f);
	tileData_ground->topBorderTex = LoadTexture((char*)"data\\texture\\block_soil_top.png");
	//tileData_ground->topBorderTexSize = vector2s(320, 98);
	tileData_ground->topBorderTexSize = vector2s(146.0f, 102.0f);
	tileData_ground->bottomBorderTex = LoadTexture((char*)"data\\texture\\block_soil_down.png");
	tileData_ground->bottomBorderTexSize = vector2s(145.0f, 72.0f);
	tileData_ground->rightBorderTex = LoadTexture((char*)"data\\texture\\block_soil_right.png");
	tileData_ground->rightBorderTexSize = vector2s(87.0f, 144.0f);
	tileData_ground->leftBorderTex = LoadTexture((char*)"data\\texture\\block_soil_left.png");
	tileData_ground->leftBorderTexSize = vector2s(88.0f, 144.0f);
	tileData_ground->centerTex = LoadTexture((char*)"data\\texture\\block_soil_center.png");
	tileData_ground->centerTexSize = vector2s(144.0f, 144.0f);
	//tileData_ground->centerTexSize = vector2s(292, 320);

	tileData_ground->tb_repeatCount = vector2s(-1, -1);
	tileData_ground->lr_repeatCount = vector2s(-1, -1);

	tileData_ground->tb_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->lr_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->center_texScale = vector2s(0.5f, 0.5f);

	SetObstacleType(E_Obstacle::Obstacle_Const);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(500.0f, 100.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\block.png"));
	groundDrawData->usePreComputeUV = true;
	groundDrawData->preComputeUV = vector2s(0, 0);
	groundDrawData->preComputeUVSize = vector2s(320.0f, 320.0f);
	groundDrawData->tex_texel_size = vector2s(1 / 320.0f, 1 / 320.0f);
	//groundDrawData->SetColor(wall_color);
	groundDrawData->SetDrawDataType(E_DrawDataType::DrawData_Tile);
	groundDrawData->SetTileData(tileData_ground);
	SetDrawData(groundDrawData);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, true, E_ColliderType::Collider_Rect,
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);

	m_start = vector3s(0.0f, 0.0f, 0.0f);
	m_end = vector3s(-500.0f,0.0f, 0.0f);

	Movement_Axis* moveGround_axisMove_movement = new Movement_Axis(this, true, true, true, true, m_movetime, GetTransform()->GetPos(), m_start, m_end);
	AddMovement(moveGround_axisMove_movement);
}

MoveGround::~MoveGround()
{

}
void MoveGround::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);
}
vector3s MoveGround::GetVec()
{
	return transform->GetMovement();
}
void MoveGround::ResetSpeed()
{
	m_speed = VecDist(m_start, m_end) / m_movetime;
}

#pragma endregion MoveGround

#pragma region AppearGround
AppearGround::AppearGround()
{
	cellType = E_CellType::CellType_AppearGround;
	//SetObstacleType(E_Obstacle::Obstacle_Const);
	//SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	//SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);

	//DrawData* groundDrawData = new DrawData();
	//groundDrawData->shakeWithCamera = true;
	//groundDrawData->SetSize(vector2s(100.0f, 100.0f));
	//groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));
	//groundDrawData->usePreComputeUV = true;
	//groundDrawData->preComputeUV = vector2s(1, 1);
	//groundDrawData->preComputeUVSize = vector2s(0, 0);
	//groundDrawData->tex_texel_size = vector2s(1, 1);
	//groundDrawData->SetColor(appearGround_color);
	//SetDrawData(groundDrawData);

	TileData* tileData_ground = new TileData();
	tileData_ground->cornerSize_1 = vector2s(88.0f, 102.0f);
	tileData_ground->cornerSize_2 = vector2s(87.0f, 72.0f);
	tileData_ground->borderTileType = E_TileType::TileType_Repeat;
	tileData_ground->centerTileType = E_TileType::TileType_Repeat;
	//tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->texRawSize = vector2s(320.0f, 320.0f);
	tileData_ground->cornerScale_1 = vector2s(0.5f, 0.5f);
	tileData_ground->cornerScale_2 = vector2s(0.5f, 0.5f);
	tileData_ground->topBorderTex = LoadTexture((char*)"data\\texture\\block_soil_top.png");
	//tileData_ground->topBorderTexSize = vector2s(320, 98);
	tileData_ground->topBorderTexSize = vector2s(146.0f, 102.0f);
	tileData_ground->bottomBorderTex = LoadTexture((char*)"data\\texture\\block_soil_down.png");
	tileData_ground->bottomBorderTexSize = vector2s(145.0f, 72.0f);
	tileData_ground->rightBorderTex = LoadTexture((char*)"data\\texture\\block_soil_right.png");
	tileData_ground->rightBorderTexSize = vector2s(87.0f, 144.0f);
	tileData_ground->leftBorderTex = LoadTexture((char*)"data\\texture\\block_soil_left.png");
	tileData_ground->leftBorderTexSize = vector2s(88.0f, 144.0f);
	tileData_ground->centerTex = LoadTexture((char*)"data\\texture\\block_soil_center.png");
	tileData_ground->centerTexSize = vector2s(144.0f, 144.0f);
	//tileData_ground->centerTexSize = vector2s(292, 320);

	tileData_ground->tb_repeatCount = vector2s(-1, -1);
	tileData_ground->lr_repeatCount = vector2s(-1, -1);

	tileData_ground->tb_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->lr_texScale = vector2s(0.5f, 0.5f);
	tileData_ground->center_texScale = vector2s(0.5f, 0.5f);

	SetObstacleType(E_Obstacle::Obstacle_Const);
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	SetPos(vector3s(SCREEN_WIDTH * 0.5f, 100.0f, 0.0f), true);
	SetDrawLayer(E_DrawLayer::DrawLayer_Scene_3);

	DrawData* groundDrawData = new DrawData();
	groundDrawData->shakeWithCamera = true;
	groundDrawData->SetSize(vector2s(500.0f, 100.0f));
	groundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\block.png"));
	groundDrawData->usePreComputeUV = true;
	groundDrawData->preComputeUV = vector2s(0, 0);
	groundDrawData->preComputeUVSize = vector2s(320.0f, 320.0f);
	groundDrawData->tex_texel_size = vector2s(1 / 320.0f, 1 / 320.0f);
	//groundDrawData->SetColor(wall_color);
	groundDrawData->SetDrawDataType(E_DrawDataType::DrawData_Tile);
	groundDrawData->SetTileData(tileData_ground);
	SetDrawData(groundDrawData);

	Collider* groundCollider = new Collider();
	ColliderData* groundColliderData = new ColliderData(0, groundCollider, true, E_ColliderType::Collider_Rect,
		groundDrawData->GetSize(), vector3s(0.0f, 0.0f, 0.0f));
	groundCollider->AddColliderData(groundColliderData);

	RegisterComponent(E_Component::Component_Collider, groundCollider, true);


	Movement_Appear* appearGround_axisMove_movement = new Movement_Appear(this, true, true, true, 2.0f, 2.0f);
	//Movement_Axis* appearGround_axisMove_movement1 = new Movement_Axis(this, true, true, true, true, 3.0f, GetTransform()->GetPos(),
	//	vector3s(0.0f, 0.0f, 0.0f), vector3s(-500.0f, 600.0f, 0.0f));
	AddMovement(appearGround_axisMove_movement);
}

AppearGround::~AppearGround()
{

}

void AppearGround::DoUpdate(float deltatime)
{
	BaseCell::DoUpdate(deltatime);
}
#pragma endregion AppearGround

#pragma region mini_stage

MiniStage::MiniStage():m_stageState(false),m_stagePos(vector2s(STAGE_SIZE_X * 0.5f, STAGE_SIZE_Y * 0.5f)),m_data(NULL),m_stageID(-1),m_showState(false)
{
	m_stageItemDic.clear();
	m_bindIDDic.clear();
}

MiniStage::~MiniStage()
{
	for (auto cur : m_stageItemDic) {
		if (cur.second != NULL) {
			cur.second->DeleteCell();
		}
	}
	m_stageItemDic.clear();
	m_bindIDDic.clear();
}

void MiniStage::Load()
{
	if (m_data == NULL)return;

	for (auto curItemData : m_data->itemDataList) {
		if (curItemData == NULL || curItemData->id == -1)continue;

		if (m_stageItemDic.find(curItemData->id) == m_stageItemDic.end()) {

			E_CellType itemType = (E_CellType)curItemData->type;

			BaseCell* newItem = MapGenerator::GetInstance()->CreateItem(itemType);
			if (newItem == NULL)continue;

			vector3s pos = m_stagePos + curItemData->pos;
			pos.x += GetRandomRangeValueI(curItemData->posRandomRange.x);
			pos.y += GetRandomRangeValueI(curItemData->posRandomRange.y);

			vector3s rot = curItemData->rot;
			rot.x += GetRandomRangeValueI(curItemData->rotRandomRange.x);
			rot.y += GetRandomRangeValueI(curItemData->rotRandomRange.y);
			rot.z += GetRandomRangeValueI(curItemData->rotRandomRange.z);

			vector3s scale = curItemData->scale;
			scale.x += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.x);
			scale.y += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.y);
			scale.z += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.z);

			vector2s size = curItemData->size;
			size.x += GetAbsRandomRangeValueI(curItemData->sizeRandomRange.x);
			size.y += GetAbsRandomRangeValueI(curItemData->sizeRandomRange.y);

			newItem->SetPos(pos, true);
			newItem->UpdateMovementData();
			newItem->SetRot(rot);
			newItem->SetScale(scale);
			newItem->SetDrawDataSize(size);
			Collider* itemCollider = (Collider*)newItem->GetComponent(E_Component::Component_Collider);
			if (itemCollider != NULL && itemCollider->GetColliderData(0) != NULL) {
				itemCollider->GetColliderData(0)->size = size;
			}

			newItem->RecordCurState();

			m_stageItemDic[curItemData->id] = newItem;

			if (curItemData->bindID != -1) {
				if (m_bindIDDic.find(curItemData->bindID) == m_bindIDDic.end()) {
					m_bindIDDic[curItemData->bindID].clear();
				}
				m_bindIDDic[curItemData->bindID].push_back(curItemData->id);
			}

			//random type
			//random probability

		}
		else {
			//error : already had same id object
		}

	}

	map<int, vector<int>>::iterator idIter;
	vector<int>::iterator idArrayIter;
	map<int, BaseCell*>::iterator cellIter;
	for (idIter = m_bindIDDic.begin(); idIter != m_bindIDDic.end(); idIter++) {
		if (idIter->second.empty() == true)continue;
		int parentId = idIter->first;

		if (m_stageItemDic.find(parentId) == m_stageItemDic.end() || m_stageItemDic[parentId] == NULL)continue;
		BaseCell* parentCell = m_stageItemDic[parentId];

		for (idArrayIter = idIter->second.begin(); idArrayIter != idIter->second.end(); idArrayIter++) {
			int childId = (*idArrayIter);

			if (childId == parentId)continue;
			if (m_stageItemDic.find(childId) == m_stageItemDic.end() || m_stageItemDic[childId] == NULL)continue;
			BaseCell* childCell = m_stageItemDic[childId];

			childCell->SetParent(parentCell);
		}
	}

	for (auto cur : m_stageItemDic) {
		if (cur.second == NULL)continue;
		cur.second->Reset();
	}

	//back ground
	BaseCell* bg = new TempBackground();
	vector3s pos = m_stagePos + vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
	bg->SetPos(pos, true);
	bg->SetState(false);
	m_stageItemDic[999] = bg;
}

void MiniStage::Unload()
{
	for (auto cur : m_stageItemDic) {
		if (cur.second != NULL) {
			delete cur.second;
		}
	}

	m_stageItemDic.clear();
}

void MiniStage::Reload()
{
	//random adjust
	for (auto curItemData : m_data->itemDataList) {
		if (curItemData == NULL || curItemData->id == -1)continue;

		if (m_stageItemDic.find(curItemData->id) != m_stageItemDic.end()) {
			BaseCell* curItem = m_stageItemDic.find(curItemData->id)->second;
			if (curItem == NULL)continue;

			vector3s pos = m_stagePos + curItemData->pos;
			pos.x += GetRandomRangeValueI(curItemData->posRandomRange.x);
			pos.y += GetRandomRangeValueI(curItemData->posRandomRange.y);

			vector3s rot = curItemData->rot;
			rot.x += GetRandomRangeValueI(curItemData->rotRandomRange.x);
			rot.y += GetRandomRangeValueI(curItemData->rotRandomRange.y);
			rot.z += GetRandomRangeValueI(curItemData->rotRandomRange.z);

			vector3s scale = curItemData->scale;
			scale.x += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.x);
			scale.y += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.y);
			scale.z += GetAbsRandomRangeValueI(curItemData->scaleRandomRange.z);

			vector2s size = curItemData->size;
			size.x += GetAbsRandomRangeValueI(curItemData->sizeRandomRange.x);
			size.y += GetAbsRandomRangeValueI(curItemData->sizeRandomRange.y);

			curItem->SetPos(pos, true);
			curItem->UpdateMovementData();
			curItem->SetRot(rot);
			curItem->SetScale(scale);
			curItem->GetDrawData()->SetSize(size);
			Collider* itemCollider = (Collider*)curItem->GetComponent(E_Component::Component_Collider);
			if (itemCollider != NULL && itemCollider->GetColliderData(0) != NULL) {
				itemCollider->GetColliderData(0)->size = size;
			}

			curItem->RecordCurState();

			//random type
			//random probability

		}
		else {
			//error : already had same id object
		}

	}


	map<int, vector<int>>::iterator idIter;
	vector<int>::iterator idArrayIter;
	map<int, BaseCell*>::iterator cellIter;
	for (idIter = m_bindIDDic.begin(); idIter != m_bindIDDic.end(); idIter++) {
		if (idIter->second.empty() == true)continue;
		int parentId = idIter->first;

		if (m_stageItemDic.find(parentId) == m_stageItemDic.end() || m_stageItemDic[parentId] == NULL)continue;
		BaseCell* parentCell = m_stageItemDic[parentId];

		for (idArrayIter = idIter->second.begin(); idArrayIter != idIter->second.end(); idArrayIter++) {
			int childId = (*idArrayIter);

			if (childId == parentId)continue;
			if (m_stageItemDic.find(childId) == m_stageItemDic.end() || m_stageItemDic[childId] == NULL)continue;
			BaseCell* childCell = m_stageItemDic[childId];

			childCell->SetParent(parentCell);
		}
	}

	for (auto cur : m_stageItemDic) {
		if (cur.second == NULL)continue;
		cur.second->Reset();
	}
}

void MiniStage::ResetStage()
{

}

void MiniStage::ShowStage(bool _show)
{
	if (_show == m_showState) {
		return;
	}
	for (auto cur : m_stageItemDic) {
		if (cur.second != NULL) {
			cur.second->SetState(_show);
		}
	}
}

void MiniStage::SetStagePos(vector3s _pos)
{
	m_stagePos = _pos;
	for (auto cur : m_stageItemDic) {
		if (cur.second != NULL) {
			vector3s pos = cur.second->GetTransform()->GetPos();
			pos = pos + m_stagePos;
			cur.second->SetPos(pos, true);
		}
	}
}

vector3s MiniStage::GetStagePos()
{
	return m_stagePos;
}

void MiniStage::SetStageData(StageData* _data)
{
	if (_data == NULL)return;
	m_data = _data;
}

StageData* MiniStage::GetStageData()
{
	return m_data;
}


#pragma endregion mini_stage

StageManager::StageManager():startPos(vector3s(STAGE_START_POS_X,STAGE_START_POS_Y,0)),stageSize(vector2s(STAGE_SIZE_X,STAGE_SIZE_Y)),stageCounter(0)
{
	stageDic.clear();
}

StageManager::~StageManager()
{
	ClearStage();
}

void StageManager::ClearStage()
{
	for (auto temp : stageDic) {
		if (temp.second.empty() == true) {
			continue;
		}
		for (auto tempStage : temp.second) {
			delete tempStage;
		}
	}

	stageDic.clear();

	stageCounter = 0;
}

void StageManager::ResetStage()
{

}

MiniStage* StageManager::CreateStageRandom()
{
	map<int, StageData*>& stageDataDic = GameMgr::GetInstance()->dataMgr->GetStageDataDic();
	if (stageDataDic.empty() == true) {
		return NULL;
	}


	//================テスト用、ボスステージに切り替え
	int enemyCounter = GameMgr::GetInstance()->uiMgr->GetEnemyCounter();
	if (enemyCounter >= NEXT_STAGE_ENEMY_COUNT)
	{
		//tempStage->SetStageData(stageDataDic[6]);
		return nullptr;
	}

	MiniStage* tempStage = new MiniStage();

	int length = stageDataDic.size();
	tempStage->SetStageData(stageDataDic[rand() % length + 1]);
	//tempStage->SetStageData(stageDataDic[8]);

	//=======================23.1 テスト用：ステージを順番に生成させる
	//int index = m_curIndex % TEST_ARRAY_NUM + 1;
	//tempStage->SetStageData(stageDataDic[m_testArray[index]]);
	//m_curIndex++;
	//=======================

	vector3s stagePos = startPos;
	stagePos.x += (stageCounter * STAGE_SIZE_X);
	tempStage->SetStagePos(stagePos);
	tempStage->SetStageID(stageCounter);
	tempStage->Load();

	stageDic[tempStage->GetStageID()].push_back(tempStage);

	stageCounter++;

	return tempStage;
}

void StageManager::RecycleStage(MiniStage* stage)
{
	if (stage == NULL)return;
	stage->SetStageState(false);
	stage->ShowStage(false);

	//temp
	if (stageDic.find(stage->GetStageID()) != stageDic.end()) {
		vector<MiniStage*>::iterator iter;
		for (iter = stageDic[stage->GetStageID()].begin(); iter != stageDic[stage->GetStageID()].end();iter++) {
			if ((*iter) == stage) {
				stageDic[stage->GetStageID()].erase(iter);
				break;
			}
		}
	}

	delete stage;
}

#pragma region temp_11_26

TempBackground::TempBackground()
{
	cellType = E_CellType::CellType_BG;
	//m_use = true;
	drawLayer = E_DrawLayer::DrawLayer_Bottom;
	SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());

	DrawData* backgroundDrawData = new DrawData();
	backgroundDrawData->shakeWithCamera = true;
	backgroundDrawData->SetSize(vector2s(1920,1080));
	backgroundDrawData->SetTextureNo(LoadTexture((char*)"data\\texture\\background.png"));
	backgroundDrawData->usePreComputeUV = true;
	backgroundDrawData->preComputeUV = vector2s(0, 0);
	backgroundDrawData->preComputeUVSize = vector2s(1, 1);
	backgroundDrawData->tex_texel_size = vector2s(1, 1);
	this->SetDrawData(backgroundDrawData);

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(this, false);
}

TempBackground::~TempBackground()
{
}

#pragma endregion temp_11_26
