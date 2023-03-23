#pragma once
#include "main.h"

#define STAGE_START_POS_X 1920
#define STAGE_START_POS_Y 0
#define STAGE_SIZE_X 1920
#define STAGE_SIZE_Y 1080

#define TEST_ARRAY_NUM 7

class Ice;

enum E_MapObstacleType
{
	Ground_Obs,
	Ice_Obs,
	Stone_Obs,
	Wind_Obs,
	SpringGround_Obs,
	BeltConveyor_Obs,
	MoveGround_Obs,
	AppearGround_Obs,
};
enum E_GroundType
{
	Small,
	Medium,
	Huge,
};

class MiniStage;

class MapGenerator
{
private:
	MapGenerator();
	MapGenerator(const MapGenerator&) = delete;
	MapGenerator& operator=(MapGenerator&) = delete;
	~MapGenerator();
public:

	BaseCell* CreateItem(E_CellType _itemType);
	void CreateGround();
	void CreateIce();
	void CreateStone();
	void CreateWind(vector3s pos);
	void CreateSpringGround(vector3s pos);
	void CreateBeltConveyor(vector3s pos);
	void CreateMoveGround(vector3s pos, vector3s start, vector3s end);
	void CreateAppearGround(vector3s pos);

	void RemoveGround();
	void RemoveIce();
	void RemoveStone();

	void RemoveAll();
	void ClearList();
	void MapUpdate();

private:
	static MapGenerator* instance;

	std::map<E_MapObstacleType, std::vector<BaseCell*>> obstacleDic;
	map<int, MiniStage*> curStageDic;

public:
	static MapGenerator* GetInstance() {
		if (instance == NULL) {
			instance = new MapGenerator();
		}
		return instance;
	}


	class MapGeneratorDeletor {
	public:
		~MapGeneratorDeletor() {
			if (MapGenerator::instance != NULL) {
				delete MapGenerator::instance;
			}
		}
	};

	static MapGeneratorDeletor deletor;
};

class Ground : public Obstacle
{
public:
	bool iceCreated = false;//22.11.10
	Ice* ice;
public:
	Ground(E_GroundType _type);
	~Ground();
	void ResetSize();

	void SetBIce(bool _b);
	bool GetBIce() const;

	void SetBStone(bool _b);
	bool GetBStone() const;

	void Recycle();
	void OnCellInit();
	void OnCellRecycle();

	void DeleteCell();

private:
	E_GroundType m_type;

	bool bIce;
	bool bStone;
};

class Ice : public Obstacle
{
public:
	Ice();
	~Ice();
	void SetGround(Ground* _ground);
	void ResetSize();

	void SetParent(BaseCell* _parent);
	void Reset();
private:
	Ground* m_ground;
};

class Stone : public Obstacle
{
public:
	Stone();
	~Stone();

	void SetGround(Ground* _ground);

	void SetParent(BaseCell* _parent);
	void Reset();
private:
	Ground* m_ground;
};

class Wind : public BaseCell
{
public:
	Wind();
	~Wind();

	void DoUpdate(float deltatime);
	void Reset();
};

class SpringGround : public BaseCell
{
public:
	SpringGround();
	~SpringGround();

	void DoUpdate(float deltatime);
	void Reset();

private:
	void ResetSize();
};

class BeltConveyor : public BaseCell
{
public:
	BeltConveyor();
	~BeltConveyor();

	void DoUpdate(float deltatime);

	float GetSpeed()const;

	//23.1
	void SetDrawDataSize(vector2s _size);

private:
	float m_speed = 1.5f;
};

class MoveGround : public Obstacle
{
public:
	MoveGround();
	~MoveGround();

	void DoUpdate(float deltatime);

	void SetStart(vector3s start) { m_start = start; }
	void SetEnd(vector3s end) { m_end = end; }

	void SetVec(vector3s vec) { m_vec = vec; }
	vector3s GetVec();

	void ResetSpeed();
	float GetSpeed() { return m_speed; }


private:
	vector3s m_vec;
	vector3s m_start;
	vector3s m_end;

	float m_speed;
	float m_movetime;
};

class AppearGround : public Obstacle
{
public:
	AppearGround();
	~AppearGround();

	void DoUpdate(float deltatime);
};

class TempBackground :public BaseCell {
public:
	TempBackground();
	~TempBackground();
};

class MiniStage {
protected:
	bool m_showState;//ステージの表示/非表示
	bool m_stageState;//ステージが使われてるかどうか
	vector3s m_stagePos;//ステージの相対位置　-  画面の中の位置　=　オブジェクトの位置　+　相対位置
	StageData* m_data;//CSVファイルから読み込んだデータ
	int m_stageID;
	map<int, BaseCell*> m_stageItemDic;//ステージのオブジェクトのインスタンスリスト
	map<int, vector<int>> m_bindIDDic;
public:
	MiniStage();
	virtual ~MiniStage();

	virtual void Load();//ステージをインスタンス化（データによってオブジェクトを生成）
	virtual void Unload();
	virtual void Reload();

	virtual void ResetStage();
	virtual void ShowStage(bool _show);

	void SetStagePos(vector3s _pos);
	vector3s GetStagePos();
	int GetStageID() {
		return m_stageID;
	}
	void SetStageID(int _id) {
		m_stageID = _id;
	}

	void SetStageState(bool _state) {
		m_stageState = _state;
	}

	bool GetStageState() {
		return m_stageState;
	}

	void SetStageData(StageData* _data);
	StageData* GetStageData();
};

class StageManager {
private:
	vector2s stageSize;//ミニステージのサイズ - vector2（画面の横のサイズ、画面の縦のサイズ）
	vector3s startPos;
	map<int, vector<MiniStage*>> stageDic;//今使ってるミニステージのリスト

	//===========23.1 テスト用：ステージを順番に生成させる
	int m_testArray[TEST_ARRAY_NUM] = {6 ,1,2, 3, 4, 7,  6 };//ステージIDリスト
	int m_curIndex = 0;
	//===========
public:
	int stageCounter;
public:
	StageManager();
	~StageManager();

	void ClearStage();
	void ResetStage();

	MiniStage* CreateStageRandom();//ミニステージをランダムで作る
	void RecycleStage(MiniStage* stage);//（プレイヤー後ろの）ミニステージを削除する

};