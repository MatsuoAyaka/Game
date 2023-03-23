#include "ObjectPool.h"
#include "MapGenerator.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Item.h"
#include "Effect.h"

using namespace std;

#pragma region object_pool

ObjectPool::ObjectPool(E_ObjCellType _cellType, int _initSize, int _maxSize)
	:m_maxPoolSize(_maxSize),/* m_dynamicSize(true),*/ /*m_dynamicIncreaseSize(1),*/m_cellType(_cellType) {
	m_cellList.clear();

	int initSize = _initSize;
	if (initSize < 0) {
		initSize = 0;
	}
	else if (initSize > _maxSize) {
		initSize = _maxSize;
	}

	for (int index = 0; index < initSize; index++) {
		BaseCell* tempCell = ObjectPoolMgr::CreateNewCell(_cellType);
		if (tempCell == NULL)continue;
		tempCell->objCellType = m_cellType;
		RecycleCell(tempCell);
	}

}

inline BaseCell* ObjectPool::GetCell()
{
	if (m_cellList.size() == 0) {
		BaseCell* cell = ObjectPoolMgr::CreateNewCell(m_cellType);//プールの中にオブジェクトがない場合、新しいのを作る
		if (cell != NULL) {
			cell->objCellType = m_cellType;
		}
		return cell;
	}

	//プールの配列の先頭のオブジェクトを戻り値として
	vector<BaseCell*>::iterator frontIter = m_cellList.begin();

	BaseCell* cell = *frontIter;

	m_cellList.erase(frontIter);

	return cell;
}

void ObjectPool::RecycleCell(BaseCell* _cell)
{
	if (_cell == NULL)return;
	if (m_cellList.size() > m_maxPoolSize) {//プールのサイズが限界になった場合、リサイクルではなくオブジェクトを削除する(delete)
		//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell(_cell);
		GameMgr::GetInstance()->sceneObjMgr->DeleteCell(_cell);
		return;
	}

	if (_cell->objCellType == E_ObjCellType::ObjCellType_Effect_Hit || _cell->objCellType == E_ObjCellType::ObjCellType_Effect_Jump) {
		for (auto& temp : m_cellList) {
			if (temp->GetID() == _cell->GetID()) {
				int a = 0;
			}
		}
	}

	_cell->OnCellRecycle();
	m_cellList.push_back(_cell);//プールの配列の最後列に追加する
}

#pragma endregion object_pool

#pragma region object_pool_manager
ObjectPoolMgr::ObjectPoolMgr() {

}

ObjectPoolMgr::	~ObjectPoolMgr() {
	if (m_objPoolDic.empty() == false) {
		map<E_ObjCellType, IObjectPool*>::iterator iter;
		for (iter = m_objPoolDic.begin(); iter != m_objPoolDic.end(); iter++) {
			if (iter->second != NULL) {
				delete iter->second;
			}
		}

		m_objPoolDic.clear();
	}
}

void ObjectPoolMgr::DoInit() {
	//メモリプールを作る
	ObjectPool* groundPool = new ObjectPool(E_ObjCellType::ObjCellType_Ground, 10, 10);
	RegisterPool(E_ObjCellType::ObjCellType_Ground, (IObjectPool*)groundPool);

	ObjectPool* icePool = new ObjectPool(E_ObjCellType::ObjCellType_Ice, 6, 6);
	RegisterPool(E_ObjCellType::ObjCellType_Ice, (IObjectPool*)icePool);

	ObjectPool* stonePool = new ObjectPool(E_ObjCellType::ObjCellType_Stone, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Stone, (IObjectPool*)stonePool);

	ObjectPool* windPool = new ObjectPool(E_ObjCellType::ObjCellType_Wind, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Wind, (IObjectPool*)windPool);

	ObjectPool* springGroundPool = new ObjectPool(E_ObjCellType::ObjCellType_SpringGround, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_SpringGround, (IObjectPool*)springGroundPool);

	ObjectPool* beltConveyorPool = new ObjectPool(E_ObjCellType::ObjCellType_BeltConveyor, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_BeltConveyor, (IObjectPool*)beltConveyorPool);

	ObjectPool* moveGroundPool = new ObjectPool(E_ObjCellType::ObjCellType_MoveGround, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_MoveGround, (IObjectPool*)moveGroundPool);

	ObjectPool* appearGroundPool = new ObjectPool(E_ObjCellType::ObjCellType_AppearGround, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_AppearGround, (IObjectPool*)appearGroundPool);

	ObjectPool* enemy_hiyokoPool = new ObjectPool(E_ObjCellType::ObjCellType_Enemy_Hiyoko, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Enemy_Hiyoko, (IObjectPool*)enemy_hiyokoPool);

	ObjectPool* enemy_monkeyPool = new ObjectPool(E_ObjCellType::ObjCellType_Enemy_Monkey, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Enemy_Monkey, (IObjectPool*)enemy_monkeyPool);

	ObjectPool* enemy_karasuPool = new ObjectPool(E_ObjCellType::ObjCellType_Enemy_Karasu, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Enemy_Karasu, (IObjectPool*)enemy_karasuPool);

	ObjectPool* bossPool = new ObjectPool(E_ObjCellType::ObjCellType_Boss, 1, 1);
	RegisterPool(E_ObjCellType::ObjCellType_Boss, (IObjectPool*)bossPool);

	ObjectPool* bulletPool = new ObjectPool(E_ObjCellType::ObjCellType_Bullet, 3, 20);
	RegisterPool(E_ObjCellType::ObjCellType_Bullet, (IObjectPool*)bulletPool);

	ObjectPool* bulletFireballPool = new ObjectPool(E_ObjCellType::ObjCellType_Bullet_Fireball, 3, 20);
	RegisterPool(E_ObjCellType::ObjCellType_Bullet_Fireball, (IObjectPool*)bulletFireballPool);

	ObjectPool* onigiriPool = new ObjectPool(E_ObjCellType::ObjCellType_Onigiri, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Onigiri, (IObjectPool*)onigiriPool);

	ObjectPool* hitEffectPool = new ObjectPool(E_ObjCellType::ObjCellType_Effect_Hit, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Effect_Hit, (IObjectPool*)hitEffectPool);

	ObjectPool* jumpEffectPool = new ObjectPool(E_ObjCellType::ObjCellType_Effect_Jump, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Effect_Jump, (IObjectPool*)jumpEffectPool);

	ObjectPool* rushEffectPool = new ObjectPool(E_ObjCellType::ObjCellType_Effect_Rush, 9, 9);
	RegisterPool(E_ObjCellType::ObjCellType_Effect_Rush, (IObjectPool*)rushEffectPool);

	ObjectPool* recoverEffectPool = new ObjectPool(E_ObjCellType::ObjCellType_Effect_Recover, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Effect_Recover, (IObjectPool*)recoverEffectPool);

	ObjectPool* buttonEffectPool = new ObjectPool(E_ObjCellType::ObjCellType_Effect_Button, 3, 3);
	RegisterPool(E_ObjCellType::ObjCellType_Effect_Button, (IObjectPool*)buttonEffectPool);
}

BaseCell* ObjectPoolMgr::GetCellFromPool(E_ObjCellType _cellType)
{
	if (m_objPoolDic.find(_cellType) == m_objPoolDic.end()) {//該当するメモリプールがない場合
		return ObjectPoolMgr::CreateNewCell(_cellType);//プールの中にオブジェクトがない場合、新しいのを作る
	}
	BaseCell* cell = m_objPoolDic[_cellType]->GetCell();//プールから取り出す
	if (cell != NULL) cell->OnCellInit();//初期化
	return cell;
}

void ObjectPoolMgr::RecycleCell(BaseCell* _cell)
{
	if (_cell == NULL)return;

	if (m_objPoolDic.find(_cell->objCellType) == m_objPoolDic.end()) {
		//GameMgr::GetInstance()->sceneObjMgr->UnRegisterCell((BaseCell*)_cell);
		GameMgr::GetInstance()->sceneObjMgr->DeleteCell((BaseCell*)_cell);
	}
	else {
		m_objPoolDic[_cell->objCellType]->RecycleCell(_cell);
	}
}


void ObjectPoolMgr::RegisterPool(E_ObjCellType _cellType, IObjectPool* _pool) {
	if (m_objPoolDic.find(_cellType) == m_objPoolDic.end()) {
		m_objPoolDic[_cellType] = _pool;
	}
	else {
		//error
	}
}

BaseCell* ObjectPoolMgr::CreateNewCell(E_ObjCellType _cellType) {
	BaseCell* tempCell = nullptr;

	switch (_cellType)
	{
	case E_ObjCellType::ObjCellType_Ground:
		tempCell = new Ground(E_GroundType::Small);
		break;
	case E_ObjCellType::ObjCellType_Ice:
		tempCell = new Ice();
		break;
	case E_ObjCellType::ObjCellType_Stone:
		tempCell = new Stone();
		break;
	case E_ObjCellType::ObjCellType_Enemy_Hiyoko:
		tempCell = new Hiyoko();
		break;
	case E_ObjCellType::ObjCellType_Enemy_Monkey:
		tempCell = new Monkey();
		break;
	case E_ObjCellType::ObjCellType_Enemy_Karasu:
		tempCell = new Karasu();
		break;
	case E_ObjCellType::ObjCellType_Enemy_Nezumi:
		tempCell = new Hiyoko(1);
		break;
	case E_ObjCellType::ObjCellType_Boss:
		tempCell = new Boss();
		break;
	case E_ObjCellType::ObjCellType_Wind:
		tempCell = new Wind();
		break;
	case E_ObjCellType::ObjCellType_SpringGround:
		tempCell = new SpringGround();
		break;
	case E_ObjCellType::ObjCellType_BeltConveyor:
		tempCell = new BeltConveyor();
		break;
	case E_ObjCellType::ObjCellType_MoveGround:
		tempCell = new MoveGround();
		break;
	case E_ObjCellType::ObjCellType_AppearGround:
		tempCell = new AppearGround();
		break;
	case E_ObjCellType::ObjCellType_Bullet:
		tempCell = new Bullet(rightVector);
		break;
	case E_ObjCellType::ObjCellType_Bullet_Fireball:
		tempCell = new EnemyBullet(vector3s(0,0,0));
		break;
	case E_ObjCellType::ObjCellType_Onigiri:
		tempCell = new Onigiri();
		break;
	case E_ObjCellType::ObjCellType_Sugar:
		tempCell = new Sugar();
		break;
	case E_ObjCellType::ObjCellType_Pepper:
		tempCell = new Pepper();
		break;
	case E_ObjCellType::ObjCellType_Pickled_Plums:
		tempCell = new PickledPlums();
		break;
	case E_ObjCellType::ObjCellType_Miso_Soup:
		tempCell = new MisoSoup();
		break;
	case E_ObjCellType::ObjCellType_Effect_Hit:
		tempCell = new EffectHit();
		break;
	case E_ObjCellType::ObjCellType_Effect_Jump:
		tempCell = new EffectJump();
		break;
	case E_ObjCellType::ObjCellType_Effect_Rush:
		tempCell = new EffectRush();
		break;
	case E_ObjCellType::ObjCellType_Effect_Recover:
		tempCell = new EffectRecover();
		break;
	case E_ObjCellType::ObjCellType_Effect_Button:
		tempCell = new EffectButton();
		break;
	default:
		return nullptr;
	}
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(tempCell, true);
	return (BaseCell*)tempCell;
}

#pragma endregion object_pool_manager

