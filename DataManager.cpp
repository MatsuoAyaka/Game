#include "DataManager.h"
#include <fstream>
#include <sstream>

#pragma region data_manager
void DataManager::ReadData()
{
	if (stage_data_file_path.empty() == true) {
		return;
	}

	ifstream ifs(stage_data_file_path);//�t�@�C�����J��

	string line;
	getline(ifs, line);//�擪�s��؂�̂�

	while (getline(ifs, line)) {//��s���ǂݍ��ށ@�i1�s���̕�����@���@1�I�u�W�F�N�g�̃f�[�^�@
		vector<string> strVec = Split(line, ',');//��������J���}�ŕ�����
		FillData(strVec);//��������f�[�^�ɕϊ�����
	}

	//�I�u�W�F�N�g�̃f�[�^���@�Y������~�j�X�e�[�W�̃f�[�^�@�ɕϊ�����
	for (auto itemTempList : m_stageItemDic) {
		if (itemTempList.second.empty() == true)continue;

		if (m_stageDataDic.find(itemTempList.first) == m_stageDataDic.end() || m_stageDataDic[itemTempList.first] == NULL) {//�Y������~�j�X�e�[�W�����݂��ĂȂ��ꍇ�A�V�����̂����
			StageData* newStageData = new StageData();//�V�����~�j�X�e�[�W�̃f�[�^
			newStageData->stageID = itemTempList.first;
			m_stageDataDic[itemTempList.first] = newStageData;
		}

		for (auto item : itemTempList.second) {
			if (item == NULL)continue;
			m_stageDataDic[itemTempList.first]->AddItemData(item);//�I�u�W�F�N�g�̃f�[�^�����X�g�ɒǉ�
		}
	}
}
void DataManager::FillData(vector<string>& dataVec)
{
	StageItemData* data = StageItemData::GetStageItemData(dataVec);//��������f�[�^�ɕϊ�����
	if (data == NULL)return;

	if (m_stageItemDic.find(data->stageId) == m_stageItemDic.end()) {
		m_stageItemDic[data->stageId].clear();
	}

	m_stageItemDic[data->stageId].push_back(data);//���X�g�ɕۑ�����
}

vector<string> DataManager::Split(string& input, char delimiter)
{
	vector<string> result;
	if (input.empty() == true) {
		return result;
	}
	istringstream stream(input);
	string field;
	while (getline(stream, field, delimiter)) {//������𕪊�
		result.push_back(field);
	}

	return result;
}

DataManager::DataManager()
{
	m_stageDataDic.clear();
	m_stageItemDic.clear();
}

DataManager::~DataManager()
{
	for (auto temp : m_stageDataDic) {
		if (temp.second != NULL) {
			delete temp.second;
		}
	}
	m_stageDataDic.clear();
	m_stageItemDic.clear();
}

void DataManager::DoInit()
{
	ReadData();
}

void DataManager::UnInit()
{

}

void DataManager::ReloadData()
{
	//TODO
}

map<int, StageData*>& DataManager::GetStageDataDic()
{
	return m_stageDataDic;
}

vector4s& DataManager::GetVec4(string& str)
{
	vector4s result = vector4s(0, 0, 0, 0);
	if (str.empty() == false) {
		vector<string> tempVec = Split(str, ';');
		if (tempVec.size() >= 4) {
			if (tempVec[0].empty() == false) {
				result.x = stof(tempVec[0]);
			}
			if (tempVec[1].empty() == false) {
				result.y = stof(tempVec[1]);
			}
			if (tempVec[2].empty() == false) {
				result.z = stof(tempVec[2]);
			}
			if (tempVec[3].empty() == false) {
				result.w = stof(tempVec[3]);
			}
		}
	}
	return result;
}

vector3s& DataManager::GetVec3(string& str)
{
	vector3s result = vector3s(0, 0, 0);
	if (str.empty() == false) {
		vector<string> tempVec = Split(str, ';');
		if (tempVec.size() >= 3) {
			if (tempVec[0].empty() == false) {
				result.x = stof(tempVec[0]);
			}
			if (tempVec[1].empty() == false) {
				result.y = stof(tempVec[1]);
			}
			if (tempVec[2].empty() == false) {
				result.z = stof(tempVec[2]);
			}
		}
	}
	return result;
}

vector2s& DataManager::GetVec2(string& str)
{
	vector2s result = vector2s(0, 0);
	if (str.empty() == false) {
		vector<string> tempVec = Split(str, ';');
		if (tempVec.size() >= 2) {
			if (tempVec[0].empty() == false) {
				result.x = stof(tempVec[0]);
			}
			if (tempVec[1].empty() == false) {
				result.y = stof(tempVec[1]);
			}
		}
	}
	return result;
}

#pragma endregion data_manager

#pragma region data

StageItemData* StageItemData::GetStageItemData(vector<string>& dataVec)
{
	StageItemData* resultData = new StageItemData();
	for (int index = 0; index < dataVec.size(); index++) {
		if (dataVec[index].empty() == true || dataVec[index] == "/") {
			if (index == 0) {
				//data error
				delete resultData;
				return NULL;
			}
			continue;
		}
		try {
			switch ((E_StageItemDataType)index) {
			case E_StageItemDataType::StageItemDataType_Stage_ID:
				resultData->stageId = stoi(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_ID:
				resultData->id = stoi(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Type:
				resultData->type = stoi(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Pos:
				resultData->pos = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Pos_Random_Range:
				resultData->posRandomRange = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Rot:
				resultData->rot = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Rot_Random_Range:
				resultData->rotRandomRange = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Scale:
				resultData->scale = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Scale_Random_Range:
				resultData->scaleRandomRange = DataManager::GetVec3(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Size:
				resultData->size = DataManager::GetVec2(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Size_Random_Range:
				resultData->sizeRandomRange = DataManager::GetVec2(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Bind_ID:
				resultData->bindID = stoi(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Random_Type:
				resultData->randomType = stoi(dataVec[index]);
				break;
			case E_StageItemDataType::StageItemDataType_Random_Probability:
				resultData->randomProbability = stof(dataVec[index]);
				break;
			default:
				break;
			}
		}
		catch (exception e) {
			//output error data (row,line)
		}
	}
	return resultData;
}

#pragma endregion data
