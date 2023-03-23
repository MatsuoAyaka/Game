
#include "Camera.h"
#include "Player.h"

MainCamera* MainCamera::instance = NULL;
MainCamera::CameraDeletor cameraDeletor;

void MainCamera::DoUpdate(float deltatime)
{
	if (m_scrollSign == true) {
		Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
		if (player != NULL) {
			vector3s pos = player->GetTransform()->GetPos();
			if (pos.x > SCREEN_WIDTH * 0.5f) {
				camPos.x = pos.x - SCREEN_WIDTH * 0.5f;
			}
		}
	}
}

void MainCamera::MoveTest()
{
	if (GetKeyboardPress(DIK_LEFT))
	{
		camPos.x -= 5.0f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		camPos.x += 5.0f;
	}
}

void MainCamera::ZoomTest()
{
	vector2s mousePos = GetMouseMovement();
	vector2s camOffset = vector2s(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	if (GetKeyboardPress(DIK_1))
	{
		mousePos = GetMouseMovement();
	}

	//ズーム関連
	if (GetKeyboardPress(DIK_NUMPADPLUS))
	{
		camScale += 0.02f;
		camScale = F_Clamp(camScale, 0.2f, 2.0f);
	}
	if (GetKeyboardPress(DIK_NUMPADMINUS))
	{
		camScale -= 0.02f;
		camScale = F_Clamp(camScale, 0.2f, 2.0f);
	}

	//マウス操作
	if (IsMouseLeftTriggered())
	{
		leftMousePosOld = leftMousePos = mousePos;
	}

	if (IsMouseLeftPressed())
	{
		vector2s tempMove = mousePos - leftMousePos;
		camPos += tempMove;

		leftMousePos = mousePos;
	}
}

void MainCamera::ResetCamPos()
{
	camPos = camPosDefault;
}
