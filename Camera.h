#pragma	once

#include "main.h"
#include "GameLogic.h"


class MainCamera 
{
private:
	bool m_scrollSign;
	vector2s camPos;
	vector2s camPosDefault;
	float camScale;

	static MainCamera* instance;
	vector2s leftMousePos;
	vector2s leftMousePosOld;
	MainCamera() :camPos(vector2s(0, 0)), camPosDefault(vector2s(0, 0)), m_scrollSign(false)
	{

	}
	MainCamera(const MainCamera&) = delete;
	MainCamera& operator = (MainCamera&) = delete;

public:
	static MainCamera* GetInstance()
	{
		if (instance == NULL)
		{
			instance = new MainCamera();
		}
		return instance;
	}

	void DoUpdate(float deltatime);

	//�J�����ړ�(�e�X�g)
	void MoveTest();
	//�J�����Y�[��(�e�X�g)
	void ZoomTest();

	void SetScrollState(bool _state) {
		m_scrollSign = _state;
	}
	void ResetCamPos();
	vector2s* GetCamPos()  { return &camPos; }
	vector2s GetCamPosDefault() { return camPosDefault; }

public:
	//2022.11.10
	//�V���O���g���������I�ɍ폜����邽�߂Ɏg���N���X
	class CameraDeletor {
	public:
		~CameraDeletor() {
			if (MainCamera::instance != NULL) {
				delete MainCamera::instance;
			}
		}
	};
	static CameraDeletor deletor;
};
