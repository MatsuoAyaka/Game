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
/// �~�j�X�e�[�W�̃I�u�W�F�N�g�̃f�[�^
/// </summary>
struct StageItemData {
	int stageId;//�~�j�X�e�[�WID
	int id;//�I�u�W�F�N�g��ID
	int	type;//�I�u�W�F�N�g�̎��
	vector3s pos;//�ʒu
	vector3s posRandomRange;//�ʒu�̃����_���͈̔�
	vector3s rot;//��]
	vector3s rotRandomRange;//��]�̃����_���͈̔�
	vector3s scale;
	vector3s scaleRandomRange;//�X�P�[���̃����_���͈̔�
	vector2s size;
	vector2s sizeRandomRange;//�T�C�Y�̃����_���͈̔�

	int bindID;//���̃I�u�W�F�N�g�ƈꏏ�ɏ�������ꍇ�A���̃I�u�W�F�N�g��ID���擾����K�v������i�Ⴆ�΁@�o�l�n�ʂƂ����ɂ���G
	int randomType;//�����_�������̎��
	float randomProbability;//�����_���̊m��

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
/// �~�j�X�e�[�W�̃f�[�^
/// </summary>
struct StageData {
	int stageID;//�X�e�[�WID
	vector<StageItemData*> itemDataList;//�~�j�X�e�[�W�̂��ׂẴI�u�W�F�N�g�̃f�[�^�iCSV�t�@�C������ǂݍ��܂ꂽ�f�[�^
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
	/// �f�[�^�����X�g�ɒǉ�
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
	//�~�j�X�e�[�WID	�I�u�W�F�N�g�̃f�[�^�̃��X�g
	map<int, vector<StageItemData*>> m_stageItemDic;
	//�~�j�X�e�[�WID	�~�j�X�e�[�W�̃|�C���^
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

	static vector<string> Split(string& input, char delimiter);//������𕪊�����
	static vector4s& GetVec4(string& str);//��������x�N�g���ɓ]������
	static vector3s& GetVec3(string& str);
	static vector2s& GetVec2(string& str);
};


