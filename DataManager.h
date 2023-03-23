#pragma once
#include "main.h"
#pragma region data

const string stage_data_file_path = "data\\table\\DataTest.csv";

enum E_StageItemDataType {
	StageItemDataType_Stage_ID = 0,
	StageItemDataType_ID = 1,
	StageItemDataType_Type = 2,
	StageItemDataType_Pos = 3,
	StageItemDataType_Pos_Random_Range = 4,
	StageItemDataType_Rot = 5,
	StageItemDataType_Rot_Random_Range = 6,
	StageItemDataType_Scale  = 7,
	StageItemDataType_Scale_Random_Range = 8,
	StageItemDataType_Size = 9,
	StageItemDataType_Size_Random_Range = 10,
	StageItemDataType_Bind_ID = 11,
	StageItemDataType_Random_Type = 12,
	StageItemDataType_Random_Probability = 13,
};

/// <summary>
/// ミニステージのオブジェクトのデータ
/// </summary>
struct StageItemData {
	int stageId;//ミニステージID
	int id;//オブジェクトのID
	int	type;//オブジェクトの種類
	vector3s pos;//位置
	vector3s posRandomRange;//位置のランダムの範囲
	vector3s rot;//回転
	vector3s rotRandomRange;//回転のランダムの範囲
	vector3s scale;
	vector3s scaleRandomRange;//スケールのランダムの範囲
	vector2s size;
	vector2s sizeRandomRange;//サイズのランダムの範囲

	int bindID;//他のオブジェクトと一緒に処理する場合、そのオブジェクトのIDを取得する必要がある（例えば　バネ地面とそこにいる敵
	int randomType;//ランダム処理の種類
	float randomProbability;//ランダムの確率

	StageItemData() :
		stageId(-1), id(-1), type(-1),
		pos(0, 0, 0), posRandomRange(0, 0, 0),
		rot(0, 0, 0), rotRandomRange(0, 0, 0),
		scale(1, 1, 1), scaleRandomRange(0, 0, 0),
		size(0, 0), sizeRandomRange(0, 0),
		bindID(-1), randomType(-1), randomProbability(1) {

	}

	static StageItemData* GetStageItemData(vector<string>& dataVec);
};

/// <summary>
/// ミニステージのデータ
/// </summary>
struct StageData {
	int stageID;//ステージID
	vector<StageItemData*> itemDataList;//ミニステージのすべてのオブジェクトのデータ（CSVファイルから読み込まれたデータ
	StageData():stageID(-1) {
		itemDataList.clear();
	}
	~StageData() {
		for (auto temp : itemDataList) {
			if (temp != NULL) {
				delete temp;
			}
		}
		itemDataList.clear();
	}

	/// <summary>
	/// データをリストに追加
	/// </summary>
	/// <param name="data"></param>
	void AddItemData(StageItemData* data) {
		if (data == NULL)return;
		itemDataList.push_back(data);
	}
};


#pragma endregion data

class DataManager {
//private:
//	vector<string> m_lineList;
//	vector<string> m_lineDataList;
private:
	//ミニステージID	オブジェクトのデータのリスト
	map<int, vector<StageItemData*>> m_stageItemDic;
	//ミニステージID	ミニステージのポインタ
	map<int, StageData*> m_stageDataDic;
private:
	void ReadData();
	void FillData(vector<string>& dataVec);
public:
	DataManager();
	~DataManager();

	void DoInit();
	void UnInit();

	void ReloadData();

	map<int, StageData*>& GetStageDataDic();

	static vector<string> Split(string& input, char delimiter);//文字列を分割する
	static vector4s& GetVec4(string& str);//文字列をベクトルに転換する
	static vector3s& GetVec3(string& str);
	static vector2s& GetVec2(string& str);
};


