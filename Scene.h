#pragma once
#include "main.h"
#include "MapGenerator.h"
class MapGenerator;
class Ground;
class MiniStage;

enum E_EnemyType;

#pragma region scene

enum E_Scene {
	Scene_None = -1,
	Scene_TestScene = 1,

	Scene_Stage1 = 11,//movement
	Scene_Stage2 = 12,//movement + laser
	Scene_Stage3 = 13,//npc
	Scene_Stage4 = 14,//npc
	Scene_Stage5 = 15,//npc(movement
	Scene_Stage6 = 16,
	Scene_End = 17,
	Scene_Boss = 18,	//追加松尾
};

enum E_SpawnType {
	SpawnType_Sequence,
	SpawnType_Parallel,
};

enum E_SpawnData {
	SpawnData_Level_0 = 0,
	SpawnData_Level_1 = 1,
	SpawnData_Level_2 = 2,
};

class Scene {
protected:

	Ground* ground;
	E_Scene sceneType;
	E_Scene nextSceneType;

	bool isSceneActive;

	map<int, BaseCell*> sceneObjDic;
	map<int, MiniStage*> curStageDic;

	MiniStage* m_frontStage;

	vector3s defaultPlayerPos;

	Scene();
	Scene(E_Scene sceneT);

public:

	virtual ~Scene();

	void RegisterSceneObj(BaseCell* cell);
	void UnregisterSceneObj(BaseCell* cell, bool unregisterAll);
	void RegisterMiniStage(MiniStage* stage);
	void UnregisterMiniStage(MiniStage* stage, bool unregisterAll);
	virtual void DoUpdate(float deltatime);
	virtual void DoLateUpdate(float deltatime);

	virtual void Load();
	virtual void Unload();
	virtual void Reset();

	virtual void OnSceneLoad();
	virtual void OnSceneUnload();

	virtual void SetSceneActive(bool state);

	void SetNextScene(E_Scene nextScene);
	E_Scene GetNextScene();
	E_Scene GetSceneType();

	void SetDefaultPlayerPos(vector3s pos);
	vector3s GetDefaultPos();

	void SetFrontStage(MiniStage* _stage);

	/// <summary>
	/// 一番最後に生成されたブロックを保存して、敵をランダムに生成する
	/// </summary>
	/// <param name="g"></param>
	/// <param name="initEnemyType">敵の種類　（テスト用）</param>
	void SetGround(Ground* g,E_EnemyType initEnemyType);
};

class TestScene :public Scene {
public:
	TestScene();
	~TestScene();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};



class Scene_1 :public Scene {//movement
public:
	Scene_1();
	~Scene_1();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();

};

class Scene_2 :public Scene {//movement + laser
public:
	Scene_2();
	~Scene_2();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};

class Scene_3 :public Scene {//npc
public:
	Scene_3();
	~Scene_3();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};

class Scene_4 :public Scene {//npc
public:
	Scene_4();
	~Scene_4();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};

class Scene_5 :public Scene {//npc(movement
public:
	Scene_5();
	~Scene_5();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};


class Scene_6 :public Scene {//npc(movement
public:
	Scene_6();
	~Scene_6();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();
};

//==========
// 追加松尾
//==========
class BossScene :public Scene {
private:
	Obstacle* wall[13];
	Boss* enemyboss;
	BaseCell* m_onigiri;
	float setFrame = 0;
	float setFrame2 = 0;
	void BlockAppear();
	void OnigiriAppear();
public:
	BossScene();
	~BossScene();

	void Load();
	void Unload();
	void Reset();

	void DoUpdate(float deltatime);
	void DoLateUpdate(float deltatime);

	void OnSceneLoad();
	void OnSceneUnload();




};

#pragma endregion scene
