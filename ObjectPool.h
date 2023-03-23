#pragma once
#include "main.h"

class Effect;
class Effect_Hit;
enum E_Effect;

enum E_ObjCellType {
	ObjCellType_None,

	ObjCellType_Ground,
	ObjCellType_Ice,
	ObjCellType_Stone,
	ObjCellType_Wind,
	ObjCellType_SpringGround,
	ObjCellType_BeltConveyor,
	ObjCellType_MoveGround,
	ObjCellType_AppearGround,

	ObjCellType_Enemy_Hiyoko,
	ObjCellType_Enemy_Karasu,
	ObjCellType_Enemy_Nezumi,
	ObjCellType_Enemy_Monkey,

	ObjCellType_Boss,

	ObjCellType_Onigiri,
	ObjCellType_Sugar,
	ObjCellType_Pepper,
	ObjCellType_Pickled_Plums,
	ObjCellType_Miso_Soup,

	ObjCellType_Bullet,
	ObjCellType_Bullet_Fireball,

	ObjCellType_Effect_Hit,
	ObjCellType_Effect_Jump,
	ObjCellType_Effect_Rush,
	ObjCellType_Effect_Recover,
	ObjCellType_Effect_Button
};

class IObjectPool {
public:
	virtual BaseCell* GetCell() = 0;
	virtual void RecycleCell(BaseCell* _cell) = 0;
};

class ObjectPool : public IObjectPool {
private:
	E_ObjCellType m_cellType;//オブジェクトの種類

	int m_maxPoolSize;//プールの最大容量
	//bool m_dynamicSize;
	//int m_dynamicIncreaseSize;

	vector<BaseCell*> m_cellList;//プールオブジェクト配列
	ObjectPool() = delete;
public:

	ObjectPool(E_ObjCellType _cellType, int _initSize, int _maxSize);
	~ObjectPool() {
		m_cellList.clear();
	}

	BaseCell* GetCell();//プールの中にオブジェクトを取り出す

	void RecycleCell(BaseCell* _cell);//オブジェクトを回収する

	E_ObjCellType GetCellType() {
		return m_cellType;
	}
};

class ObjectPoolMgr {
private:
	map<E_ObjCellType, IObjectPool*> m_objPoolDic;

public:

	ObjectPoolMgr();
	~ObjectPoolMgr();

	void DoInit();

	BaseCell* GetCellFromPool(E_ObjCellType _cellType);//プールの中にオブジェクトを取り出す
	void RecycleCell(BaseCell* _cell);//オブジェクトを回収する

	void RegisterPool(E_ObjCellType _cellType, IObjectPool* _pool);


	static BaseCell* CreateNewCell(E_ObjCellType _cellType);//プールからオブジェクトがない場合、新しいのを作る

};

