#include "GameLogic.h"
#include "Scene.h"
#include "Player.h"
#include "Camera.h"
#include "Enemy.h"
#include "MapGenerator.h"
#include "EventCenter.h"
#include "Effect.h"

#define BAR_SCALE_X  (3)


#pragma region input_manager

InputMgr::InputMgr() :curInputMode(E_InputMode::InputMode_None) {

}

InputMgr::~InputMgr() {

}

void InputMgr::DoUpdate(float deltatime) {
	if (inputState == true) {

		////�ǉ�����================================================================
		//if (curInputMode == E_InputMode::InputMode_Logo) {//�^�C�g���ł̓���
		//	if (GetKeyboardTrigger(DIK_SPACE) == true) {
		//		GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Title, true);
		//		
		//	}
		//}
		//=========================================================================
		if (curInputMode == E_InputMode::InputMode_Title) {//�^�C�g���ł̓���
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START)) {
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal,true);
			}
		}
		else if (curInputMode == E_InputMode::InputMode_Game) {//�Q�[������
			if (GameMgr::GetInstance()->sceneObjMgr->GetPlayer() != NULL) {
				GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->Input(deltatime);//�v���C���[�L�����N�^�[�̓��͏���
			}
			//MainCamera::GetInstance()->MoveTest();
			MainCamera::GetInstance()->ZoomTest();
		}
		else if (curInputMode == E_InputMode::InputMode_Game_Menu) {//�Q�[�����j���[�ł̓���

		}
		else if (curInputMode == E_InputMode::InputMode_Game_Lose_Menu) {
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START))
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal, true);
		}
		else if (curInputMode == E_InputMode::InputMode_Game_Win_Menu) {
			if (GetKeyboardTrigger(DIK_RETURN) == true || IsButtonPressed(0, BUTTON_START)) {
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Normal, true);
			}
		}
	}
}

/// <summary>
/// ���̓��[�h��؂�ւ���
/// </summary>
/// <param name="targetInputMode"></param>
void InputMgr::SwitchInputMode(E_InputMode targetInputMode) {
	curInputMode = targetInputMode;
}

/// <summary>
/// ���͏�Ԃ�ݒ肷��
/// </summary>
/// <param name="state"></param>
void InputMgr::SetInputState(bool state) {
	inputState = state;
}

/// <summary>
/// ���̓��̓��[�h���擾����
/// </summary>
/// <returns></returns>
E_InputMode InputMgr::GetCurInputMode()
{
	return curInputMode;
}

#pragma endregion input_manager

#pragma region SceneObjMgr_method

SceneObjMgr::SceneObjMgr() :cellIdCounter(10){

	m_objectPoolMgr = NULL;
	drawCellDic.clear();
	cellDic.clear();
	cellTypeDic.clear();

	m_player = NULL;

	tempCell = NULL;
}

SceneObjMgr::~SceneObjMgr()
{
	drawCellDic.clear();
	cellDic.clear();
	cellTypeDic.clear();
	drawCellDic.clear();
	m_player = NULL;

	if (tempCell != NULL) {
		delete tempCell;
		tempCell = NULL;
	}
	if (m_objectPoolMgr != NULL) {
		delete m_objectPoolMgr;
	}
}

void SceneObjMgr::DoInit() {

	cellIdCounter = 10;
	cellDic.clear();

	m_objectPoolMgr = new ObjectPoolMgr();
	m_objectPoolMgr->DoInit();

	//�v���C���[�L�����N�^�[���C���X�^���X��
	Player* player = new Player();
	player->SetID(this->GetID());//ID��ݒ肷��
	player->SetPos(vector3s(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.3f, 0.0f), true);//�ʒu��ݒ肷��
	//�`��f�[�^�����
	DrawData* drawData = new DrawData();
	drawData->shakeWithCamera = true;//�J�����h��̎��A�Q�[����ʂƈꏏ�ɗh���
	drawData->SetSize(const_size_player);//�`��T�C�Y
	drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\playerAniSheet.png"));//�e�N�X�`���\
	drawData->texIndex = vector2s(TEX_PLAYER_ANIM_IDLE_START_INDEX_X, TEX_PLAYER_ANIM_IDLE_START_INDEX_Y);//�ŏ��̃e�N�X�`���\UV���W
	drawData->tex_w_count = TEX_PLAYER_ANIM_CELL_W_COUNT;//�e�N�X�`���\�̉��̐}�Ă̐�
	drawData->tex_texel_size = character_cell_tex_size;//�e�N�X�`���\�̐}�ẴT�C�Y�@
	//drawData->usePreComputeUV = false;
	//drawData->preComputeUV = vector2s(0.0f, 0.0f);
	//drawData->preComputeUVSize = vector2s(1.0f,1.0f);
	//drawData->tex_texel_size = vector2s(1.0f,1.0f);//�e�N�X�`���\�̐}�ẴT�C�Y�@
	player->SetDrawData(drawData);//�`��f�[�^���v���C���[�Q�[���I�u�W�F�N�g�ɓn��

	//�v���C���[�A�j���[�V�����R���|�[�l���g�����
	Animation* playerAnimComponent = new Animation();//�C���X�^���X��
	if (player->RegisterComponent(E_Component::Component_Animation, playerAnimComponent) == false) {//�A�j���[�V�����R���|�[�l���g��ǉ�����
		delete playerAnimComponent;
		playerAnimComponent = NULL;
	}
	else {
		
		//�A�C�h���A�j���[�V���������
		AnimClip* playerIdleAnim = new AnimClip();
		int startTexIndex = TEX_PLAYER_ANIM_IDLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_IDLE_START_INDEX_X;//�A�j���[�V�����̃X�^�[�gUV���W
		int endTexIndex = startTexIndex + TEX_PLAYER_ANIM_IDLE_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerIdleAnim->SetAnimParam(E_Anim::Anim_Idle, vector2s(startTexIndex, endTexIndex), 0.5f, true, true);//�A�j���[�V�����̃p�����[�^��ݒ肷��
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Idle, playerIdleAnim, true);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����

		//�ړ��A�j���[�V���������
		AnimClip* playerMoveAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_MOVE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_MOVE_START_INDEX_X; // �A�j���[�V�����̃X�^�[�gUV���W
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_MOVE_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerMoveAnim->SetAnimParam(E_Anim::Anim_Move, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Move, playerMoveAnim, false);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����
		
		//����A�j���[�V���������
		AnimClip* playerSlideAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_SLIDE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_SLIDE_START_INDEX_X; // �A�j���[�V�����̃X�^�[�gUV���W
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_SLIDE_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerSlideAnim->SetAnimParam(E_Anim::Anim_Slide, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Slide, playerSlideAnim, false);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����

		//�]�ԃA�j���[�V���������
		AnimClip* playerTumbleAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_TUMBLE_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_TUMBLE_START_INDEX_X; // �A�j���[�V�����̃X�^�[�gUV���W
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_TUMBLE_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerTumbleAnim->SetAnimParam(E_Anim::Anim_Tumble, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Tumble, playerTumbleAnim, false);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����
	
		//�U���󂯃A�j���[�V���������
		AnimClip* playerHurtAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_HURT_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_HURT_START_INDEX_X; // �A�j���[�V�����̃X�^�[�gUV���W
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_HURT_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerHurtAnim->SetAnimParam(E_Anim::Anim_Hurt, vector2s(startTexIndex, endTexIndex), 0.79f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Hurt, playerHurtAnim, false);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����
	
		//�����A�j���[�V���������
		AnimClip* playerWinAnim = new AnimClip();
		startTexIndex = TEX_PLAYER_ANIM_WIN_START_INDEX_Y * TEX_PLAYER_ANIM_CELL_W_COUNT + TEX_PLAYER_ANIM_WIN_START_INDEX_X; // �A�j���[�V�����̃X�^�[�gUV���W
		endTexIndex = startTexIndex + TEX_PLAYER_ANIM_WIN_FRAME_COUNT;//�A�j���[�V�����̃G���hUV���W
		//								���				UV���W							�������ԁA�����A�J��Ԃ��t���O
		playerWinAnim->SetAnimParam(E_Anim::Anim_Win, vector2s(startTexIndex, endTexIndex), 1.5f, true, true);
		playerAnimComponent->RegisterAnim(E_Anim::Anim_Win, playerWinAnim, false);//�A�j���[�V�����R���|�[�l���g�ɐV�����A�j���[�V������ǉ�����
		playerWinAnim->animEnd_Callback = new Anim_Player_Win_CB();

	}
	vector2s size = player->GetSize(true);
	//size.x *= 0.92f;
	//size.y *= 1.0f;
	//�v���C���[�R���C�_�[�R���|�[�l���g�����
	Collider* playerColliderComponent = new Collider();//�C���X�^���X��
	player->RegisterComponent(E_Component::Component_Collider, playerColliderComponent);//�R���C�_�[�R���|�[�l���g��ǉ�����
		//�R���C�_�[�f�[�^�����
	ColliderData* playerCollider_1 = new ColliderData(1, playerColliderComponent, true,
		E_ColliderType::Collider_Circle, size, vector3s(0, 0, 0));

	playerColliderComponent->AddColliderData(playerCollider_1);//�R���C�_�[�R���|�[�l���g�ɃR���C�_�[�f�[�^��ǉ�����

	RegisterCell(player,true);//�V�����Q�[���I�u�W�F�N�g��SceneObjMgr�ɕۑ����Ă��炤
	m_player = player;

	player->DoInit();
	player->SetState(false);

	//�ꎞ�I�ȃQ�[���I�u�W�F�N�g�����i�����蔻��p�Ȃ̂ŁA�R���C�_�[�R���|�[�l���g���������Ă��Ȃ��j
	//�`���A�b�v�f�[�g�����Ȃǂ̕K�v���Ȃ��̂ŁA���X�g�ɒǉ����邱�Ƃ͕s�v
	tempCell = new BaseCell();
	Collider* collider = new Collider();
	ColliderData* collider_data = new ColliderData(0, collider, false, E_ColliderType::Collider_Rect, vector2s(0, 0), vector3s(0, 0, 0));
	collider->AddColliderData(collider_data);
	tempCell->RegisterComponent(E_Component::Component_Collider, collider);

	m_nezumi = new Nezumi();
	RegisterCell(m_nezumi, true);//�V�����Q�[���I�u�W�F�N�g��SceneObjMgr�ɕۑ����Ă��炤
	m_nezumi->DoInit();
	//m_nezumi->SetState(false);


}

void SceneObjMgr::UnInit(bool deleteCell) {
	//���ׂẴQ�[���I�u�W�F�N�g���폜����
	if (deleteCell) {
		map<int, BaseCell*>::iterator iter;
		for (iter = cellDic.begin(); iter != cellDic.end(); iter++) {
			if (iter->second == NULL)continue;
			BaseCell* temp = iter->second;
			iter->second = NULL;
			delete temp;
		}

		cellDic.clear();
		cellIdCounter = 10;

		drawCellDic.clear();
	}
}

/// <summary>
/// ���ׂẴQ�[���I�u�W�F�N�g������������i�Q�[���V�[�������[�h����Ƃ��ɌĂяo�����
/// </summary>
void SceneObjMgr::DoInitCell() {
	for (auto cell : cellDic) {
		if (cell.second == NULL || cell.second->CheckState() == false) {
			continue;
		}

		cell.second->DoInit();
	}
}

/// <summary>
/// ���ׂẴQ�[���I�u�W�F�N�g�̃A�b�v�f�[�g����
/// </summary>
/// <param name="deltatime"></param>
void SceneObjMgr::DoUpdate(float deltatime) {
	int size = cellDic.size();
	for (auto cell : cellDic) {
		if (cell.second == NULL || cell.second->CheckState() == false) {
			continue;
		}

		cell.second->DoUpdate(deltatime);

	}
	//23.1
	DeleteCell();
}

void SceneObjMgr::DoLateUpdate(float deltatime) {

}

/// <summary>
/// ���ׂẴQ�[���I�u�W�F�N�g�̕`�揈��
/// </summary>
void SceneObjMgr::DoDraw()
{
	//�`�揇���ɂ���ĕ`�悷��
	//bottom	�]�@��Ԍ��
	//scene_3
	//scene_2
	//scene_1
	//ui
	//top		�]	��ԑO
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Bottom) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Bottom];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_3) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_3];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_2) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_2];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Scene_1) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Scene_1];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_UI) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_UI];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
	if (drawCellDic.find(E_DrawLayer::DrawLayer_Top) != drawCellDic.end()) {
		map<int, BaseCell*>& tempDic = drawCellDic[E_DrawLayer::DrawLayer_Top];
		for (auto& tempCell : tempDic) {
			if (tempCell.second == NULL || tempCell.second->CheckState() == false) {
				continue;
			}

			DrawData* tempDrawData = tempCell.second->GetDrawData();
			if (tempDrawData != NULL) {
				DrawCell(tempCell.second->GetTransform(), tempCell.second->GetDrawData());
			}
		}
	}
}

/// <summary>
/// �V�����Q�[���I�u�W�F�N�g��z��̒��ɕۑ�����
/// </summary>
/// <param name="id">�@�Q�[���I�u�W�F�N�g��ID</param>
/// <param name="cell">�@�Q�[���I�u�W�F�N�g�̃C���X�^���X</param>
/// <param name="recordDefaultState">�Q�[���I�u�W�F�N�g�̏����̏�Ԃ�ۑ����邩�ǂ���</param>
void SceneObjMgr::RegisterCell(int id, BaseCell* cell, bool recordDefaultState) {
	if (cell == NULL)return;

	if (recordDefaultState == true) {
		cell->RecordCurState();//�����̏�Ԃ�ۑ�����
	}
	//�Q�[���I�u�W�F�N�g��ID��Y�����Ƃ��ă��X�g�ɒǉ�����
	if (cellDic.find(id) == cellDic.end()) {
		cellDic[id] = cell;
	}
	else {
		//error
	}

	E_CellType cellType = cell->GetCellType();
	//�Q�[���I�u�W�F�N�g����ނɂ���ĕ��ނ��Ă��烊�X�g�ɒǉ�����
	if (cellTypeDic.find(cellType) == cellTypeDic.end()) {
		cellTypeDic[cellType].clear();//init
	}
	if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
		return; //error
	}
	cellTypeDic[cellType][id] = cell;

	//�Q�[���I�u�W�F�N�g��`�揇���ɂ���ĕ��ނ��Ă��烊�X�g�ɒǉ�����
	E_DrawLayer drawLayer = cell->GetDrawLayer();
	int drawLayerIndex = (int)drawLayer;
	if (drawCellDic.find(drawLayerIndex) == drawCellDic.end()) {
		drawCellDic[drawLayerIndex].clear();
	}
	if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
		//error
	}
	else {
		drawCellDic[drawLayerIndex][id] = cell;
	}
}

void SceneObjMgr::RegisterCell(BaseCell* cell, bool recordDefaultState) {
	if (cell == NULL) {
		return;
	}

	if (recordDefaultState == true) {
		cell->RecordCurState();//�����̏�Ԃ�ۑ�����
	}

	int id = cell->GetID();
	//�Q�[���I�u�W�F�N�g��ID��Y�����Ƃ��ă��X�g�ɒǉ�����
	if (cellDic.find(id) == cellDic.end()) {
		cellDic[id] = cell;
	}
	else {
		//error
	}

	E_CellType cellType = cell->GetCellType();
	//�Q�[���I�u�W�F�N�g����ނɂ���ĕ��ނ��Ă��烊�X�g�ɒǉ�����
	if (cellTypeDic.find(cellType) == cellTypeDic.end()) {
		cellTypeDic[cellType].clear();//init
	}
	if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
		//error
	}
	else {
		cellTypeDic[cellType][id] = cell;
	}

	//�Q�[���I�u�W�F�N�g��`�揇���ɂ���ĕ��ނ��Ă��烊�X�g�ɒǉ�����
	E_DrawLayer drawLayer = cell->GetDrawLayer();
	int drawLayerIndex = (int)drawLayer;
	if (drawCellDic.find(drawLayerIndex) == drawCellDic.end()) {
		drawCellDic[drawLayerIndex].clear();
	}
	if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
		//error
	}
	else {
		drawCellDic[drawLayerIndex][id] = cell;
	}

}

void SceneObjMgr::InitCornetState(vector3s defaultPos)
{
	if (m_nezumi != nullptr) {
		m_nezumi->SetPos(defaultPos, true);
		m_nezumi->SetState(true);
		m_nezumi->SetStatus(E_EnemyStatus::ENEMY_STS_NONE);
	}
}

void SceneObjMgr::UninitCornetState()
{
	if (m_nezumi != nullptr) {
		m_nezumi->SetState(false);
	}
}

/// <summary>
/// �Q�[���I�u�W�F�N�g���폜����
/// </summary>
/// <param name="id"></param>
void SceneObjMgr::UnRegisterCell(int id) {
	if (cellDic.find(id) == cellDic.end()) {
		return;
	}
	BaseCell* cell = cellDic[id];
	cellDic.erase(id);

	if (cell != NULL) {

		E_CellType cellType = cell->GetCellType();
		if (cellTypeDic.find(cellType) != cellTypeDic.end()) {
			if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
				cellTypeDic[cellType].erase(id);
			}
		}

		//draw cell dic
		E_DrawLayer drawLayer = cell->GetDrawLayer();
		int drawLayerIndex = (int)drawLayer;
		if (drawCellDic.find(drawLayerIndex) != drawCellDic.end()) {
			if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
				drawCellDic[drawLayerIndex].erase(id);
			}
		}


		delete cell;
	}
}

/// <summary>
/// �Q�[���I�u�W�F�N�g���폜����
/// </summary>
/// <param name="cell"></param>
void SceneObjMgr::UnRegisterCell(BaseCell* cell) {
	if (cell == NULL)return;
	int id = cell->GetID();
	if (cellDic.find(id) == cellDic.end()) {
		return;
	}

	cellDic.erase(id);

	if (cell != NULL) {

		E_CellType cellType = cell->GetCellType();
		if (cellTypeDic.find(cellType) != cellTypeDic.end()) {
			if (cellTypeDic[cellType].find(id) != cellTypeDic[cellType].end()) {
				cellTypeDic[cellType].erase(id);
			}
		}

		//draw cell dic
		E_DrawLayer drawLayer = cell->GetDrawLayer();
		int drawLayerIndex = (int)drawLayer;
		if (drawCellDic.find(drawLayerIndex) != drawCellDic.end()) {
			if (drawCellDic[drawLayerIndex].find(id) != drawCellDic[drawLayerIndex].end()) {
				drawCellDic[drawLayerIndex].erase(id);
			}
		}

		delete cell;
	}
}

/// <summary>
/// ID�ɂ���ăQ�[���I�u�W�F�N�g�̃C���X�^���X���擾����
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
BaseCell* SceneObjMgr::GetCell(int id) {
	map<int, BaseCell*>::iterator iter = cellDic.find(id);
	if (iter != cellDic.end()) {
		return iter->second;
	}

	return NULL;
}

/// <summary>
/// �v���C���[�Q�[���I�u�W�F�N�g���擾����
/// </summary>
/// <returns></returns>
Player* SceneObjMgr::GetPlayer()
{
	return m_player;
}

Nezumi* SceneObjMgr::GetNezumi()
{
	return m_nezumi;
}

/// <summary>
/// �v���C���[�̓����蔻��
/// </summary>
/// <param name="player"></param>
//void SceneObjMgr::CheckPlayerCollision(Player* player) {
//	if (player == NULL)return;
//
//	//��Q���Ƃ̓����蔻��
//	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true,0), false);
//				player->SetCurGounrd(iter->second);
//			}
//		}
//
//	}
//	//�A�C�X�Ƃ̓����蔻��
//	if (cellTypeDic.find(E_CellType::CellType_Ice) != cellTypeDic.end()) {
//
//		bool iceCollision = false;
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Ice];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				player->SetOnIce(true);
//				iceCollision = true;
//				break;
//			}
//		}
//		if (iceCollision == false) {
//			player->SetOnIce(false);
//		}
//	}
//	//�S�[���Ƃ̓����蔻��
//	if (cellTypeDic.find(E_CellType::CellType_TargetPoint) != cellTypeDic.end()) {
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_TargetPoint];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				//clear
//				//���̃X�e�[�W�i�V�[���j�ɐ؂�ւ�
//				GameMgr::GetInstance()->sceneMgr->SwitchNextScene();
//				break;
//			}
//		}
//	}
//	/*
//	//�G�Ƃ̓����蔻��
//	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {
//
//		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Enemy];
//		map<int, BaseCell*>::iterator iter;
//		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
//			if (iter->second == NULL || iter->second->CheckState() == false) {
//				continue;
//			}
//			if (iter->second->GetCellType() == E_CellType::CellType_Player)continue;
//
//			if (Collider::CollisionDetect(player, iter->second)) {
//				Transform::AdjustPosition(player, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
//			}
//		}
//
//	}
//	*/
//}

bool SceneObjMgr::CheckTempCollision()
{
	if (tempCell == NULL) {
		return false;
	}
	bool returnSign = false;
	//��Q���Ƃ̓����蔻��
	//�������̂��̂Ɠ����蔻�肵�����ꍇ��
	//E_CellType::CellType_Obstacle�𑼂̒l�ɕς���΂����ł��@�iE_CellType::CellType_Enemy�Ƃ�
	if (cellTypeDic.find(E_CellType::CellType_Obstacle) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_Obstacle];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//�����蔻��
				//adjust pos
				//�ړ���Ɉړ��ł��Ȃ����Ƃ��Ă��A�Ԃ���O�̈ʒu�Ɉړ��ł���̂ŁA���̈ʒu�Ɉړ�������
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true,0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true,0) , iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}
	if (cellTypeDic.find(E_CellType::CellType_MoveGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_MoveGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//�����蔻��
				//adjust pos
				//�ړ���Ɉړ��ł��Ȃ����Ƃ��Ă��A�Ԃ���O�̈ʒu�Ɉړ��ł���̂ŁA���̈ʒu�Ɉړ�������
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true,0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	if (cellTypeDic.find(E_CellType::CellType_AppearGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_AppearGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//�����蔻��
				//adjust pos
				//�ړ���Ɉړ��ł��Ȃ����Ƃ��Ă��A�Ԃ���O�̈ʒu�Ɉړ��ł���̂ŁA���̈ʒu�Ɉړ�������
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	if (cellTypeDic.find(E_CellType::CellType_SpringGround) != cellTypeDic.end()) {

		map<int, BaseCell*>& tempDic = cellTypeDic[E_CellType::CellType_SpringGround];
		map<int, BaseCell*>::iterator iter;
		for (iter = tempDic.begin(); iter != tempDic.end(); iter++) {
			if (iter->second == NULL || iter->second->CheckState() == false) {
				continue;
			}

			if (Collider::CollisionDetect(tempCell, iter->second)) {//�����蔻��
				//adjust pos
				//�ړ���Ɉړ��ł��Ȃ����Ƃ��Ă��A�Ԃ���O�̈ʒu�Ɉړ��ł���̂ŁA���̈ʒu�Ɉړ�������
				//Transform::AdjustPosition(tempCell, iter->second->GetTransform()->GetPos(), iter->second->GetColliderSize(true, 0), false);
				Transform::AdjustPosition(tempCell, iter->second->GetColliderPos(true, 0), iter->second->GetColliderSize(true, 0), false);
				//return true;
				returnSign = true;
				break;
			}
		}

	}

	return returnSign;
}


void SceneObjMgr::InitPlayerObjState(vector3s defaultPos)
{
	if (m_player != NULL) {
		m_player->Reset();
		m_player->SetinAir(false);
		m_player->SetvelocityY(0.0f);
		m_player->SetPos(defaultPos, true);
		m_player->SetState(true);
		m_player->SetCurGround(NULL);
		m_player->inputLock = false;
		//m_player->m_winSign = false;
		m_player->SetWinSign(false);
	}
}

void SceneObjMgr::UninitPlayerObjState()
{
	if (m_player != NULL) {
		m_player->Reset();
		m_player->SetPos(vector3s(SCREEN_WIDTH*0.3f,SCREEN_HEIGHT*0.5f,1.0f), true);
		m_player->SetState(false);
		m_player->SetCurGround(NULL);
		m_player->inputLock = true;
	}

}



BaseCell* SceneObjMgr::GetTempCell()
{
	return tempCell;
}

void SceneObjMgr::SetTempCellParam(vector3s pos, vector3s rot, vector3s scale, vector2s colliderSize,int colliderDataIndex)
{
	if (tempCell == NULL)return;
	tempCell->SetPos(pos,true);
	tempCell->SetRot(rot);
	tempCell->SetScale(scale);

	Collider* collider = (Collider*) tempCell->GetComponent(E_Component::Component_Collider);
	if (collider != NULL) {
		collider->SetColliderSize(colliderSize, colliderDataIndex);
	}

}


//�������v�[��
//��ނɂ���ăI�u�W�F�N�g���擾
BaseCell* SceneObjMgr::CreateCell(E_CellType _cellType, int _count, ...)//[�ϒ�����]�@CreateCell�iE_CellType , ���ɂ��Ă�������̐��A���̈���...)
{
	E_ObjCellType objCellType = E_ObjCellType::ObjCellType_None;

	//if (_cellType == E_CellType::CellType_Boss) {
	//	objCellType = E_ObjCellType::ObjCellType_Boss;
	//	BaseCell* cell = m_objectPoolMgr->GetCellFromPool(objCellType);//�������v�[������I�u�W�F�N�g�����o��
	//	if (cell == NULL)return NULL;

	//	return (BaseCell*)cell;
	//}

	va_list args;

	if (_cellType == E_CellType::CellType_Obstacle) {
		va_start(args, _count);

		E_Obstacle obstacleType = va_arg(args, E_Obstacle);
		switch (obstacleType) {
		case E_Obstacle::Obstacle_Ground:
			objCellType = E_ObjCellType::ObjCellType_Ground;
			break;
		case E_Obstacle::Obstacle_Ice:
			objCellType = E_ObjCellType::ObjCellType_Ice;
			break;
		case E_Obstacle::Obstacle_Stone:
			objCellType = E_ObjCellType::ObjCellType_Stone;
			break;
		default:
			break;
		}
		va_end(args);

	}
	else if (_cellType == E_CellType::CellType_Enemy_Dumpling) {

		objCellType = E_ObjCellType::ObjCellType_Enemy_Hiyoko;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Ittanmomen) {

		objCellType = E_ObjCellType::ObjCellType_Enemy_Karasu;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Pasta) {
		objCellType = E_ObjCellType::ObjCellType_Enemy_Monkey;
	}
	else if (_cellType == E_CellType::CellType_Enemy_Cornet) {
		objCellType = E_ObjCellType::ObjCellType_Enemy_Nezumi;
	}
	else if (_cellType == E_CellType::CellType_Boss) {
		objCellType = E_ObjCellType::ObjCellType_Boss;
	}
	else if (_cellType == E_CellType::CellType_Ice)
	{
		objCellType = E_ObjCellType::ObjCellType_Ice;
	}
	else if (_cellType == E_CellType::CellType_Bullet)
	{
		objCellType = E_ObjCellType::ObjCellType_Bullet;
	}
	else if (_cellType == E_CellType::CellType_Bullet_Fireball) {
		objCellType = E_ObjCellType::ObjCellType_Bullet_Fireball;
	}

	else if (_cellType == E_CellType::CellType_BeltConveyor)
	{
		objCellType = E_ObjCellType::ObjCellType_BeltConveyor;
	}
	else if (_cellType == E_CellType::CellType_Wind)
	{
		objCellType = E_ObjCellType::ObjCellType_Wind;
	}
	else if (_cellType == E_CellType::CellType_MoveGround)
	{
		objCellType = E_ObjCellType::ObjCellType_MoveGround;
	}
	else if (_cellType == E_CellType::CellType_AppearGround)
	{
		objCellType = E_ObjCellType::ObjCellType_AppearGround;
	}
	else if (_cellType == E_CellType::CellType_SpringGround) 
	{
		objCellType = E_ObjCellType::ObjCellType_SpringGround;
	}
	else if (_cellType == E_CellType::CellType_Onigiri) {
		objCellType = E_ObjCellType::ObjCellType_Onigiri;
	}
	else if (_cellType == E_CellType::CellType_Sugar) {
		objCellType = E_ObjCellType::ObjCellType_Sugar;
	}
	else if (_cellType == E_CellType::CellType_Pepper) {
		objCellType = E_ObjCellType::ObjCellType_Pepper;
	}
	else if (_cellType == E_CellType::CellType_Pickled_Plums) {
		objCellType = E_ObjCellType::ObjCellType_Pickled_Plums;
	}
	else if (_cellType == E_CellType::CellType_Miso_Soup) {
		objCellType = E_ObjCellType::ObjCellType_Miso_Soup;
	}
	else if (_cellType == E_CellType::CellType_Effect_Hit) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Hit;
	}
	else if (_cellType == E_CellType::CellType_Effect_Jump) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Jump;
	}
	else if (_cellType == E_CellType::CellType_Effect_Rush) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Rush;
	}
	else if (_cellType == E_CellType::CellType_Effect_Recover) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Recover;
	}
	else if (_cellType == E_CellType::CellType_Effect_Button) {
		objCellType = E_ObjCellType::ObjCellType_Effect_Button;
	}


	BaseCell* cell = m_objectPoolMgr->GetCellFromPool(objCellType);//�������v�[������I�u�W�F�N�g�����o��
	if (cell == NULL)return NULL;

	return (BaseCell*)cell;
}

void SceneObjMgr::RecycleCell(BaseCell* _cell)
{
	if (m_objectPoolMgr != NULL) {
		m_objectPoolMgr->RecycleCell(_cell);
	}
}

void SceneObjMgr::DeleteCell(BaseCell* _cell)
{
	if (_cell == NULL)return;
	if (m_deleteCellDic.find(_cell->GetID()) == m_deleteCellDic.end()) {
		m_deleteCellDic[_cell->GetID()] = _cell;
	}
}

void SceneObjMgr::DeleteCell()
{
	for (auto& temp : m_deleteCellDic) {
		if (temp.second == NULL)continue;
		this->UnRegisterCell(temp.second);
	}

	m_deleteCellDic.clear();
}


#pragma endregion SceneObjMgr_method

#pragma region effect_manager

EffectMgr::EffectMgr()
{
	cameraShakeFactor = 0.0f;
	cameraShakeTime = 0.0f;
	cameraShakeTimePass = 0.0f;
	shakeTargetPos = vector3s(0, 0, 0);

	cameraShakeSwitch = false;
	shakePosOffset = vector3s(0.0f, 0.0f, 0.0f);

	sceneTransformBg = NULL;
	sceneTransformBg_2 = NULL;

	hitEffect_end_cb = new Anim_Effect_Normal_CB();

}

EffectMgr::~EffectMgr()
{
	sceneTransformBg = NULL;
	sceneTransformBg_2 = NULL;

	if (hitEffect_end_cb != NULL) {
		delete hitEffect_end_cb;
	}
	hitEffect_end_cb = NULL;
}

void EffectMgr::DoInit()
{
	////create effect data�@�FTODO
	//scene transform bg
	sceneTransformBg = new BaseCell();
	sceneTransformBg->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	sceneTransformBg->SetCellType(E_CellType::CellType_BG);
	sceneTransformBg->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	sceneTransformBg->SetPos(vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), true);

	DrawData* sceneTransformBg_drawData = new DrawData();
	sceneTransformBg_drawData->SetSize(sceneTransformBGSize);
	sceneTransformBg_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\SceneTransformBG.png"));
	sceneTransformBg_drawData->usePreComputeUV = true;
	sceneTransformBg_drawData->preComputeUV = vector2s(0, 0);
	sceneTransformBg_drawData->preComputeUVSize = vector2s(1, 1);
	sceneTransformBg_drawData->tex_texel_size = vector2s(1, 1);
	sceneTransformBg->SetDrawData(sceneTransformBg_drawData);

	Movement_Axis* sceneTransformBg_movement = new Movement_Axis(sceneTransformBg, false, true, false, false, 0.8f,
		vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(-sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.5f, -1.0f));
	sceneTransformBg->AddMovement(sceneTransformBg_movement);

	sceneTransformBg->SetState(false);

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sceneTransformBg, true);

	sceneTransformBg_2 = new BaseCell();
	sceneTransformBg_2->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	sceneTransformBg_2->SetCellType(E_CellType::CellType_BG);
	sceneTransformBg_2->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());
	sceneTransformBg_2->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), true);
	sceneTransformBg_2->SetRot(vector3s(180.0f, 0.0f, 0.0f));

	DrawData* sceneTransformBg_2_drawData = new DrawData();
	sceneTransformBg_2_drawData->SetSize(sceneTransformBGSize);
	sceneTransformBg_2_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\sceneTransformBg.png"));
	sceneTransformBg_2_drawData->usePreComputeUV = true;
	sceneTransformBg_2_drawData->preComputeUV = vector2s(0, 0);
	sceneTransformBg_2_drawData->preComputeUVSize = vector2s(1, 1);
	sceneTransformBg_2_drawData->tex_texel_size = vector2s(1, 1);
	sceneTransformBg_2->SetDrawData(sceneTransformBg_2_drawData);

	Movement_Axis* sceneTransformBg_2_movement = new Movement_Axis(sceneTransformBg_2, false, true, false, false, 0.8f,
		vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f), vector3s(SCREEN_WIDTH + sceneTransformBGSize.x * 0.5f, SCREEN_HEIGHT * 0.5f, -1.0f));
	sceneTransformBg_2->AddMovement(sceneTransformBg_2_movement);

	sceneTransformBg_2->SetState(false);

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sceneTransformBg_2, true);


	//20230119 �����Ꮙ����
	sakuraFubuki = new EffectSakura();
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(sakuraFubuki, true);//SceneObjMgr�ɕۑ����Ă��炤
}

void EffectMgr::UnInit()
{

}

void EffectMgr::DoUpdate(float deltatime)
{
	//�J�����h�ꏈ��
	if (cameraShakeSwitch == true) {

		cameraShakeFactor -= deltatime * CAMERA_SHAKE_REDUCE_FACTOR;//���������񂾂񌸂炵�Ă���
		//�����_�������ɗh���
		shakeTargetPos.x = (rand() % 200 - 100) * 0.01f;
		shakeTargetPos.y = (rand() % 200 - 100) * 0.01f;
		shakeTargetPos.z = 0.0f;
		shakeTargetPos = VecNormalize(shakeTargetPos);//�����x�N�g���𐳋K������
		shakeTargetPos = shakeTargetPos * cameraShakeFactor;//�����̒l��������
		shakeTargetPos = shakeTargetPos;//�ڕW�̈ʒu���A�b�v�f�[�g����

		shakePosOffset = VecLerp(shakePosOffset, shakeTargetPos, deltatime * cameraShakeFactor);//��Ԋ֐��ŃJ�����̎��̈ʒu���擾����

		cameraShakeTimePass += deltatime;
		if (cameraShakeTimePass >= cameraShakeTime) {//�J�����h�ꂪ�I���
			cameraShakeSwitch = false;//�X�C�b�`�I�t
			cameraShakeFactor = 0.0f;
			cameraShakeTime = 0.0f;
		}
	}
}

void EffectMgr::SetCameraShake(float shakeTime, float shakeFactor)
{
	if (cameraShakeSwitch == false) {//�J�����h�ꂪ�I�t�̏ꍇ�A�܂�����������
		shakePosOffset = vector3s(0.0f, 0.0f, 0.0f);
	}
	else {//�J�����h�ꂪ�I���̏ꍇ�i�쓮���j
		if (shakeFactor < cameraShakeFactor) { //�����J�����h��̋��������̋������ア�ł���΁A�㏑���ł��Ȃ����Ƃɂ���
			return;
		}
	}

	cameraShakeSwitch = true;//�X�C�b�`�I��
	cameraShakeTime = shakeTime;//��������
	cameraShakeFactor = shakeFactor;//����
	cameraShakeTimePass = 0.0f;//�o�ߎ��Ԃ�����������
}


/// <summary>
/// �V�[���̐؂�ւ��̃A�j���[�V�������g��
/// </summary>
/// <param name="show">�@�g�����ǂ���</param>
/// <param name="swapIn">�@true : �t�F�C�g�C���@false : �t�F�C�g�A�E�g</param>
void EffectMgr::ShowSceneTransformBG(bool show, bool swapIn)
{
	if (swapIn == true) {
		if (sceneTransformBg == NULL)return;
		if (show == true) {
			sceneTransformBg->Reset();
			sceneTransformBg->StartMovement(E_MovementType::MovementType_None, true,true);
			sceneTransformBg->SetState(true);
		}
		else {
			sceneTransformBg->SetState(false);
		}
	}
	else {
		if (sceneTransformBg != NULL) {
			sceneTransformBg->SetState(false);
		}

		if (sceneTransformBg_2 == NULL)return;
		if (show == true) {
			sceneTransformBg_2->Reset();
			sceneTransformBg_2->StartMovement(E_MovementType::MovementType_None, true,true);
			sceneTransformBg_2->SetState(true);
		}
		else {
			sceneTransformBg_2->SetState(false);
		}
	}
}

/// <summary>
/// �؂�ւ��̃A�j���[�V�����̏�Ԃ��`�F�b�N
/// </summary>
/// <returns></returns>
bool EffectMgr::CheckSceneTransformBGSwapInAnimDone()
{
	if (sceneTransformBg == NULL || sceneTransformBg->CheckState() == false)return true;

	return sceneTransformBg->CheckCurMovementDoneState(E_MovementType::MovementType_Axis);
}

void EffectMgr::ShowSakura()
{
	sakuraFubuki->SetState(true);
}

void EffectMgr::HideSakura()
{
	sakuraFubuki->SetState(false);
}


/// <summary>
/// �؂�ւ��̃A�j���[�V�����̏�Ԃ��`�F�b�N
/// </summary>
/// <returns></returns>
bool EffectMgr::CheckSceneTransformBGSwapOutAnimDone()
{
	if (sceneTransformBg_2 == NULL || sceneTransformBg_2->CheckState() == false)return true;

	return sceneTransformBg_2->CheckCurMovementDoneState(E_MovementType::MovementType_Axis);
}

Effect* EffectMgr::CreateEffect(E_Effect effectType, BaseCell* followObj, vector3s pos, vector3s dir)
{
	Effect* effect = NULL;
	switch (effectType) {
	case E_Effect::Effect_Hit:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Hit);
		break;
	case E_Effect::Effect_Jump:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Jump);
		break;
	case E_Effect::Effect_Rush:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Rush);
		break;
	case E_Effect::Effect_Recover:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Recover);
		break;
	case E_Effect::Effect_Button:
		effect = (Effect*)MapGenerator::GetInstance()->CreateItem(E_CellType::CellType_Effect_Button);
		break;
	default:
		break;
	}
	if (effect == NULL)return NULL;

	//calc rot
	//default dir: right
	vector3s directoin = VecNormalize(dir);

	float angle = (atan2(rightVector.x, rightVector.y) - atan2(dir.x, dir.y)) * RAD2DEG;
	effect->GetTransform()->SetRot(vector3s(0, 0, angle));

	pos.z = -10;
	effect->SetPos(pos, true);
	effect->StartEffect();
	effect->SetState(true);

	return effect;
}


#pragma endregion effect_manager


#pragma region scene_manager_method

SceneMgr::SceneMgr() : defaultSceneType(E_Scene::Scene_None), curSceneType(E_Scene::Scene_None), curScene(NULL),targetScene(E_Scene::Scene_None),reloadTargetScene(false)
{
	switchSceneSign = switchSceneSign_1 = switchSceneSign_2 = false;
	stageMgr = new StageManager();
	sceneDic.clear();

	char filename_GameClear[] = "data\\BGM\\GameClear.wav";
	m_sound_GameClear = LoadSound(filename_GameClear);

	char filename_GameOver[] = "data\\BGM\\GameOver.wav";
	m_sound_GameOver = LoadSound(filename_GameOver);

	char filename_GamePlay[] = "data\\BGM\\GamePlay.wav";
	m_sound_GamePlay = LoadSound(filename_GamePlay);

	char filename_title[] = "data\\BGM\\Shall_we_meet�H.wav";
	m_sound_title = LoadSound(filename_title);
}

SceneMgr::~SceneMgr()
{
	if (stageMgr != NULL) {
		delete stageMgr;
		stageMgr = NULL;
	}
	curScene = NULL;
}

void SceneMgr::DoInit()
{
	Scene_1* scene_1 = new Scene_1();
	scene_1->SetNextScene(E_Scene::Scene_Stage2);
	scene_1->SetDefaultPlayerPos(player_stage_1_defaultPos);

	Scene_2* scene_2 = new Scene_2();
	scene_2->SetNextScene(E_Scene::Scene_Stage3);
	scene_2->SetDefaultPlayerPos(player_stage_1_defaultPos);

	Scene_3* scene_3 = new Scene_3();
	scene_3->SetNextScene(E_Scene::Scene_Stage4);
	scene_3->SetDefaultPlayerPos(player_stage_3_defaultPos);

	Scene_4* scene_4 = new Scene_4();
	scene_4->SetNextScene(E_Scene::Scene_Stage5);
	scene_4->SetDefaultPlayerPos(player_stage_4_defaultPos);

	Scene_5* scene_5 = new Scene_5();
	scene_5->SetNextScene(E_Scene::Scene_End);
	scene_5->SetDefaultPlayerPos(player_stage_5_defaultPos);

	TestScene* testScene = new TestScene();
	testScene->SetNextScene(E_Scene::Scene_End);
	testScene->SetDefaultPlayerPos(player_stage_5_defaultPos);

	//==============
	// �ǉ�����
	//==============
	BossScene* bossScene = new BossScene();
	bossScene->SetNextScene(E_Scene::Scene_End);
	bossScene->SetDefaultPlayerPos(player_stage_5_defaultPos);

	RegisterScene(scene_1, true);
	RegisterScene(scene_2, false);
	RegisterScene(scene_3, false);
	RegisterScene(scene_4, false);
	RegisterScene(scene_5, false);

	RegisterScene(testScene, false);

	//==============
	// �ǉ�����
	//==============
	RegisterScene(bossScene, false);


	switchSceneSign = switchSceneSign_1 = switchSceneSign_2 = switchSceneSign_3 = false;
	targetScene = E_Scene::Scene_None;
	reloadTargetScene = false;
	//load default scene
	//SwitchScene(defaultSceneType,true);
}

void SceneMgr::UnInit()
{
	ClearSceneDic();
}

void SceneMgr::DoUpdate(float deltatime)
{
	if (curScene != NULL) {
		curScene->DoUpdate(deltatime);
	}

	//�V�[���̐؂�ւ��̃A�j���[�V��������
	if (switchSceneSign == true) {
		if (switchSceneSign_1 == true) {
			if (GameMgr::GetInstance()->effectMgr->CheckSceneTransformBGSwapInAnimDone() == true) {
				if ((targetScene == E_Scene::Scene_End || targetScene == E_Scene::Scene_None)) {
					if (GameMgr::GetInstance()->GetCurGameState() != E_GameState::GameState_Game_Lose) {
						SwitchScene_2(E_Scene::Scene_None, reloadTargetScene);
						GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_End, true);
					}
					else {
						switchSceneSign = false;
						switchSceneSign_1 = false;
						switchSceneSign_2 = false;
						switchSceneSign_3 = false;

						GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, true);
						GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, false);
						return;
					}
				}
				else {
					SwitchScene_2(targetScene, reloadTargetScene);//���ۂɃV�[����؂�ւ���
				}
				switchSceneSign_1 = false;
				switchSceneSign_2 = true;
			}
		}
		else if (switchSceneSign_2 == true) {
			GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, true);
			GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(true, false);
			//GameMgr::GetInstance()->inputMgr->SetInputState(true);
			switchSceneSign_2 = false;
			switchSceneSign_3 = true;

		}
		else if (switchSceneSign_3 == true) {
			if (GameMgr::GetInstance()->effectMgr->CheckSceneTransformBGSwapOutAnimDone() == true) {
				switchSceneSign_3 = false;
				switchSceneSign = false;
				GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(false, false);
			}
		}
	}

}

void SceneMgr::DoLateUpdate(float deltatime)
{
}

/// <summary>
/// �V�[����ǉ�����
/// </summary>
/// <param name="s">�@�V�[���̃C���X�^���X</param>
/// <param name="setDefaultScene"></param>
void SceneMgr::RegisterScene(Scene* s, bool setDefaultScene)
{
	if (s == NULL)return;
	E_Scene sceneType = s->GetSceneType();
	if (sceneDic.size() == 0 || sceneDic.find(sceneType) == sceneDic.end()) {
		sceneDic[sceneType] = s;//�V�[����z��̒��ɒǉ�����

		if (setDefaultScene == true) {
			defaultSceneType = sceneType;
		}
	}
	else {
		//error
	}
}

void SceneMgr::UnregisterScene(Scene* s)
{
	if (s == NULL || sceneDic.size() == 0)return;

	map<E_Scene, Scene*>::iterator iter = sceneDic.find(s->GetSceneType());
	if (iter != sceneDic.end()) {
		sceneDic.erase(iter);
	}
}

/// <summary>
/// �w�肳�ꂽ�V�[���ɐ؂�ւ���
/// </summary>
/// <param name="sceneType">�@���̃V�[��</param>
/// <param name="reload">�@���̃V�[�������������邩�ǂ���</param>
/// <param name="activeTransformAnim">�@�؂�ւ��̃A�j���[�V�������g�����ǂ���</param>
void SceneMgr::SwitchScene(E_Scene sceneType, bool reload,bool activeTransformAnim) {
	if (activeTransformAnim == false) {
		SwitchScene_2(sceneType, reload);
	}
	else {

		if (switchSceneSign == true) {
			return;
		}

		targetScene = sceneType;//���̃V�[������U�ۑ�����
		reloadTargetScene = reload;
		switchSceneSign = true;
		switchSceneSign_1 = true;
		switchSceneSign_2 = false;
		switchSceneSign_3 = false;

		GameMgr::GetInstance()->effectMgr->ShowSceneTransformBG(true,true);
		//GameMgr::GetInstance()->inputMgr->SetInputState(false);

		//GameMgr::GetInstance()->effectMgr->ShowSceneTransformLogo(true,true);
		//GameMgr::GetInstance()->inputMgr->SetInputState(false);
	}
}

/// <summary>
/// ���ۂɃV�[����؂�ւ��鏈��
/// </summary>
/// <param name="sceneType">�@���̃V�[��</param>
/// <param name="reload">�@���̃V�[�������������邩�ǂ���</param>
void SceneMgr::SwitchScene_2(E_Scene sceneType, bool reload)
{
	//GameMgr::GetInstance()->uiMgr->ShowTitle(false);
	//GameMgr::GetInstance()->uiMgr->ShowEndMenu(false);
	//GameMgr::GetInstance()->effectMgr->HideSakura();

	if (sceneType == curSceneType && sceneType != E_Scene::Scene_None && reload == false) {//���̃V�[�������̃V�[���Ɠ����ł����
		ResetCurScene();
		return;
	}

	if (sceneDic.find(sceneType) == sceneDic.end() && sceneDic[sceneType] != NULL) {//�V�[����������Ȃ��ł���΁A���^�[��
		return;
	}

	if (curScene != NULL) {
		curScene->Unload();//���̃V�[���̏I������
		curScene = NULL;
	}

	curScene = sceneDic[sceneType];
	if (curScene != NULL) {
		curScene->Load();//���̃V�[�������[�h����
		curSceneType = sceneType;
	}
}

/// <summary>
/// ���炩���ߐݒ肳�ꂽ���̃V�[���ɐ؂�ւ���
/// </summary>
void SceneMgr::SwitchNextScene()
{
	if (curScene != NULL) {
		E_Scene nextScene = curScene->GetNextScene();//���̃V�[�����擾����
		SwitchScene(nextScene, true, true);
	}
}

/// <summary>
/// ���̃V�[����������x����������
/// </summary>
void SceneMgr::ResetCurScene()
{
	SwitchScene(curSceneType, true, true);
}

void SceneMgr::ClearSceneDic()
{
	map<E_Scene, Scene*>::iterator iter;
	for (iter = sceneDic.begin(); iter != sceneDic.end(); iter++) {
		if (iter->second != NULL) {
			delete iter->second;
		}
	}
	sceneDic.clear();
}

/// <summary>
/// ���̃V�[�����擾����
/// </summary>
/// <returns></returns>
E_Scene SceneMgr::GetCurSceneType()
{
	return curSceneType;
}

vector3s SceneMgr::GetSceneCameraPos()
{
	//if (curScene != NULL) {
	//	return curScene->GetSceneCameraPos();
	//}
	return vector3s(0, 0, 0);
}

Scene* SceneMgr::GetCurScene()
{
	return curScene;
}


#pragma endregion scene_manager_method

#pragma region ui_manager

UIMgr::UIMgr()
{
	logoMenu = NULL;
	titleMenu = NULL;
	endMenu = NULL;
	m_title = NULL;
	//����ǉ�(��)=======
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = NULL;
	}
	//===============
}

UIMgr::~UIMgr() {
	logoMenu = NULL;
	titleMenu = NULL;
	endMenu = NULL;
	m_title = NULL;

	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = NULL;
	}
	//===============
}

void UIMgr::DoInit()
{
	//=======================================================================
	//�ǉ�����
	//���S�\����ʂ����
	logoMenu = new BaseCell();
	logoMenu->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	logoMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	logoMenu->SetCellType(E_CellType::CellType_Menu);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	logoMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu

	DrawData* logoMenu_drawData = new DrawData();
	logoMenu_drawData->SetSize(vector2s(SCREEN_WIDTH, SCREEN_HEIGHT));//�`��T�C�Y
	logoMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));//�e�N�X�`���\
	logoMenu_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	logoMenu_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	logoMenu_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	logoMenu_drawData->tex_texel_size = vector2s(1, 1);
	logoMenu->SetDrawData(logoMenu_drawData);//�`��f�[�^��n��

	logoMenu->SetState(false);//��\���ɂ���

	//========================================================================
	//���S�\��
	teamLogo = new BaseCell();
	teamLogo->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//�`�揇����ݒ肷��
	teamLogo->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	teamLogo->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
	teamLogo->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
	teamLogo_drawData = new DrawData();
	teamLogo_drawData->SetSize(vector2s(842, 596));//�`��T�C�Y
	teamLogo_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\TeamLogo.png"));//�e�N�X�`���\
	teamLogo_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	teamLogo_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	teamLogo_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	teamLogo_drawData->tex_texel_size = vector2s(1, 1);
	teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
	teamLogo->SetDrawData(teamLogo_drawData);//�`��f�[�^��n��

	teamLogo->SetState(false);//��\���ɂ���
	//
	//=======================================================================

	//�^�C�g��UI�����
	titleMenu = new BaseCell();
	titleMenu->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	titleMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	titleMenu->SetCellType(E_CellType::CellType_Menu);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	titleMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* titleMenu_drawData = new DrawData();
	titleMenu_drawData->SetSize(vector2s(SCREEN_WIDTH, SCREEN_HEIGHT));//�`��T�C�Y
	titleMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\fade_white.png"));//�e�N�X�`���\
	titleMenu_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	titleMenu_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	titleMenu_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	titleMenu_drawData->tex_texel_size = vector2s(1, 1);
	titleMenu_drawData->SetColor(D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f));
	titleMenu->SetDrawData(titleMenu_drawData);//�`��f�[�^��n��

	titleMenu->SetState(false);//��\���ɂ���

	m_title = new BaseCell();
	m_title->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	m_title->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	m_title->SetCellType(E_CellType::CellType_Menu);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	m_title->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* title_drawData = new DrawData();
	//title_drawData->SetSize(vector2s(1784.0f, 1784*0.563f));//�`��T�C�Y
	title_drawData->SetSize(vector2s(1920.0f, 1080.0f));//�`��T�C�Y
	title_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\title.png"));//�e�N�X�`���\
	title_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	title_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	title_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	title_drawData->tex_texel_size = vector2s(1, 1);
	m_title->SetDrawData(title_drawData);//�`��f�[�^��n��

	m_title->SetState(false);//��\���ɂ���

	//�Q�[���I�[�o�[UI�����
	gameOver = new BaseCell();
	gameOver->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//�`�揇����ݒ肷��
	gameOver->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	gameOver->SetCellType(E_CellType::CellType_GameOver);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	gameOver->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
	//�`��f�[�^�����
	DrawData* gameOver_drawData = new DrawData();
	//gameOver_drawData->SetSize(vector2s(1784.0f, 1784*0.563f));//�`��T�C�Y
	gameOver_drawData->SetSize(vector2s(1920.0f, 1080.0f));//�`��T�C�Y
	gameOver_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\GameOver.png"));//�e�N�X�`���\
	gameOver_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	gameOver_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	gameOver_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	gameOver_drawData->tex_texel_size = vector2s(1, 1);
	gameOver->SetDrawData(gameOver_drawData);//�`��f�[�^��n��

	gameOver->SetState(false);//��\���ɂ���


	endMenu = new BaseCell();
	endMenu->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	endMenu->SetCellType(E_CellType::CellType_Menu);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	endMenu->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//�`�揇����ݒ肷��
	endMenu->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* endMenu_drawData = new DrawData();
	endMenu_drawData->SetSize(vector2s(lightTextEff_size.x * 0.8f, lightTextEff_size.y * 0.8f));
	endMenu_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\lightText.png"));
	//endMenu_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	//endMenu_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	//endMenu_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	endMenu_drawData->texIndex = vector2s(TEX_EFFECT_LIGHTTEXT_START_INDEX_X, TEX_EFFECT_LIGHTTEXT_START_INDEX_Y);
	endMenu_drawData->tex_w_count = TEX_EFFECT_LIGHTTEXT_CELL_W_COUNT;
	endMenu_drawData->tex_texel_size = lightText_cell_tex_size;
	endMenu->SetDrawData(endMenu_drawData);//�`��f�[�^��n��

	Animation* lightTextAni = new Animation();
	endMenu->RegisterComponent(E_Component::Component_Animation, lightTextAni, true);

	AnimClip* lightTextAniClip = new AnimClip();
	int startTexIndex = 0;
	int endTexIndex = startTexIndex + TEX_EFFECT_LIGHTTEXT_FRAME_COUNT;
	lightTextAniClip->SetAnimParam(E_Anim::Anim_LightTextEff, vector2s(startTexIndex, endTexIndex), LIGHTTEXT_SPEED, true, true);
	lightTextAni->RegisterAnim(E_Anim::Anim_LightTextEff, lightTextAniClip, true);

	endMenu->SetState(false);//��\���ɂ���

		//���U���g�G�t�F�N�g
	resultEff = new BaseCell();
	resultEff->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	resultEff->SetCellType(E_CellType::CellType_Effect_Result);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	resultEff->SetDrawLayer(E_DrawLayer::DrawLayer_Top);
	resultEff->SetPos(vector3s(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* resultEff_drawData = new DrawData();
	resultEff_drawData->SetSize(resultEff_size);
	resultEff_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\resultEffect.png"));
	//resultEff_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	//resultEff_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	//resultEff_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	resultEff_drawData->texIndex = vector2s(TEX_EFFECT_RESULT_START_INDEX_X, TEX_EFFECT_RESULT_START_INDEX_Y);
	resultEff_drawData->tex_w_count = TEX_EFFECT_RESULT_CELL_W_COUNT;
	resultEff_drawData->tex_texel_size = result_cell_tex_size;
	resultEff->SetDrawData(resultEff_drawData);//�`��f�[�^��n��

	Animation* resultEffAni = new Animation();
	resultEff->RegisterComponent(E_Component::Component_Animation, resultEffAni, true);

	AnimClip* resultEffAniClip = new AnimClip();
	startTexIndex = 0;
	endTexIndex = startTexIndex + TEX_EFFECT_RESULT_FRAME_COUNT;
	resultEffAniClip->SetAnimParam(E_Anim::Anim_ResultEff, vector2s(startTexIndex, endTexIndex), RESULTEFF_SPEED, true, true);
	resultEffAni->RegisterAnim(E_Anim::Anim_ResultEff, resultEffAniClip, true);

	resultEff->SetState(false);//��\���ɂ���

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(resultEff, true);

		//����ǉ�(��)==============================================
	//**********�v���C���[���C�t��UI�e�X�g�p*************
	campos = MainCamera::GetInstance()->GetCamPos(); //�J�����ʒu�擾

	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		uiLife[i] = new BaseCell();
		uiLife[i]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
		uiLife[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
		uiLife[i]->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
		uiLife[i]->SetPos(vector3s(120.0f + uiLife_gap * i, 120.0f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
		DrawData* uiLife_drawData = new DrawData();
		uiLife_drawData->SetSize(vector2s(120, 120));
		uiLife_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		uiLife_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
		uiLife_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
		uiLife_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
		uiLife_drawData->tex_texel_size = vector2s(1, 1);
		uiLife[i]->SetDrawData(uiLife_drawData);//�`��f�[�^��n��
		uiLife[i]->SetState(false);//��\���ɂ���
		Life_Drawflag[i] = true;
		PlyerLife_before = 100;
		HpDecrease = 0;

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(uiLife[i], true);//SceneObjMgr�ɕۑ����Ă��炤

		//uiLife_gap += 50; //���C�t�̕\���ʒu�����炷
	}

	//�e�\����UI
	for (int n = 0; n < UI_BULLET_NUM; n++)
	{
		uiBullet[n] = new BaseCell();
		uiBullet[n]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
		uiBullet[n]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
		uiBullet[n]->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
		uiBullet[n]->SetPos(vector3s(680.0f - uiBullet_gap, 160.0f, 0.0f), true);//�ʒu

		DrawData* uiBullet_drawData = new DrawData();
		uiBullet_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\okome.png"));
		uiBullet_drawData->SetSize(vector2s(40, 50));
		uiBullet_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
		uiBullet_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
		uiBullet_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
		uiBullet_drawData->tex_texel_size = vector2s(1, 1);
		uiBullet[n]->SetDrawData(uiBullet_drawData);//�`��f�[�^��n��
		uiBullet[n]->SetState(false);//��\���ɂ���
		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(uiBullet[n], true);//SceneObjMgr�ɕۑ����Ă��炤

		Bullet_Drawflag[n] = true;

		uiBullet_gap += 30;
	}
	bulletPos = 680.0f; // 01/23 ����ǉ�

	//======================================================

	//bar�\��                                      by���E
	//======================================================
	bar = new BaseCell();
	bar->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	bar->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	bar->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	bar->SetPos(vector3s(195, 100, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* bar_drawData = new DrawData();
	bar_drawData->SetSize(vector2s(300, 20));
	bar_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\bar.png"));
	bar_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	bar_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	bar_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	bar_drawData->tex_texel_size = vector2s(1, 1);
	bar->SetDrawData(bar_drawData);//�`��f�[�^��n��

	bar->SetState(false);//��\���ɂ���
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(bar, true);//SceneObjMgr�ɕۑ����Ă��炤

	//pIcon�\��
	pIcon = new BaseCell();
	pIcon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	pIcon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	pIcon->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	pIcon->SetPos(vector3s(bar->GetTransform()->GetPos().x + (bar_drawData->GetSize().x / 2), 100, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* pIcon_drawData = new DrawData();
	pIcon_drawData->SetSize(vector2s(60, 60));
	pIcon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\pIcon.png"));
	pIcon_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	pIcon_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	pIcon_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	pIcon_drawData->tex_texel_size = vector2s(1, 1);
	pIcon->SetDrawData(pIcon_drawData);//�`��f�[�^��n��

	pIcon->SetState(false);//��\���ɂ���
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(pIcon, true);//SceneObjMgr�ɕۑ����Ă��炤

	//eIcon�\��
	eIcon = new BaseCell();
	eIcon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	eIcon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	eIcon->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��Menu�ɐݒ肷��
	eIcon->SetPos(vector3s(bar->GetTransform()->GetPos().x - (bar_drawData->GetSize().x / 2), 100, 0.0f), true);//�ʒu
		//�`��f�[�^�����
	DrawData* eIcon_drawData = new DrawData();
	eIcon_drawData->SetSize(vector2s(60, 60));
	eIcon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\eIcon.png"));
	eIcon_drawData->usePreComputeUV = true;//�e�N�X�`���\�}�b�s���O�̃^�C�v��ݒ肷��
	eIcon_drawData->preComputeUV = vector2s(0, 0);//UV���W�̎n�_�i���_�j
	eIcon_drawData->preComputeUVSize = vector2s(1, 1);//�}�ẴT�C�Y�i�e�N�X�`���\�̃T�C�Y�Ɠ����̏ꍇ�ApreComputeUVSize��tex_texel_size��������vector2s(1,1)�ɐݒ肷���OK�ł�
	eIcon_drawData->tex_texel_size = vector2s(1, 1);
	eIcon->SetDrawData(eIcon_drawData);//�`��f�[�^��n��

	eIcon->SetState(false);//��\���ɂ���
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(eIcon, true);//SceneObjMgr�ɕۑ����Ă��炤
	//======================================================


	//====================23.1�@��
	campos = MainCamera::GetInstance()->GetCamPos(); //�J�����ʒu�擾

	for (int i = 0; i < UI_NUM_COUNT; i++)
	{
		m_numArray[i] = new BaseCell();
		m_numArray[i]->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
		m_numArray[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
		m_numArray[i]->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
		m_numArray[i]->SetPos(vector3s(120.0f + uiLife_gap * i, 280.0f, 0.0f), true);//�ʒu
		//�`��f�[�^�����
		DrawData* uiNum_drawData = new DrawData();
		uiNum_drawData->SetSize(vector2s(100.0f, 100.0f));
		uiNum_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\num.png"));
		uiNum_drawData->texIndex = vector2s(TEX_NUM_START_INDEX_X + rand()% TEX_NUM_COUNT_W, TEX_NUM_START_INDEX_Y);
		uiNum_drawData->tex_w_count = TEX_NUM_COUNT_W;
		uiNum_drawData->tex_texel_size = num_tex_size;
		m_numArray[i]->SetDrawData(uiNum_drawData);//�`��f�[�^��n��
		m_numArray[i]->SetState(false);//��\���ɂ���

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_numArray[i], true);//SceneObjMgr�ɕۑ����Ă��炤

		//uiLife_gap += 50; //���C�t�̕\���ʒu�����炷
	}

	m_icon = new BaseCell();
	m_icon->SetDrawLayer(E_DrawLayer::DrawLayer_UI);//�`�揇����ݒ肷��
	m_icon->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	m_icon->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
	m_icon->SetPos(vector3s(150.0f,SCREEN_HEIGHT* 0.4f,0.0f), true);//�ʒu

	//�`��f�[�^�����
	DrawData* icon_drawData = new DrawData();
	icon_drawData->SetSize(vector2s(100.0f, 100.0f));
	icon_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\cornet_icon.png"));
	icon_drawData->usePreComputeUV = true;
	icon_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	icon_drawData->preComputeUVSize = vector2s(1.0f, 1.0f);
	icon_drawData->tex_texel_size = vector2s(1.0f, 1.0f);
	m_icon->SetDrawData(icon_drawData);//�`��f�[�^��n��
	m_icon->SetState(false);//��\���ɂ���

		//�X�R�A�e�L�X�g������
	scoreText = new BaseCell();
	scoreText->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//�`�揇����ݒ肷��
	scoreText->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
	scoreText->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
	scoreText->SetPos(vector3s(200.0f, SCREEN_HEIGHT * 0.8f, 0.0f), true);//�ʒu

	//�`��f�[�^�����
	DrawData* scoreText_drawData = new DrawData();
	scoreText_drawData->SetSize(vector2s(500.0f, 100.0f));
	scoreText_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\Score.png"));
	scoreText_drawData->usePreComputeUV = true;
	scoreText_drawData->preComputeUV = vector2s(0.0f, 0.0f);
	scoreText_drawData->preComputeUVSize = vector2s(1.0f, 1.0f);
	scoreText_drawData->tex_texel_size = vector2s(1.0f, 1.0f);
	scoreText->SetDrawData(scoreText_drawData);//�`��f�[�^��n��
	scoreText->SetState(false);//��\���ɂ���


	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_icon, true);//SceneObjMgr�ɕۑ����Ă��炤


	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(titleMenu, true);//SceneObjMgr�ɕۑ����Ă��炤
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(endMenu, true);//SceneObjMgr�ɕۑ����Ă��炤
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_title, true);//SceneObjMgr�ɕۑ����Ă��炤

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(logoMenu, true);//SceneObjMgr�ɕۑ����Ă��炤
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(teamLogo, true);//SceneObjMgr�ɕۑ����Ă��炤

	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(resultEff, true);//SceneObjMgr�ɕۑ����Ă��炤
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(gameOver, true);//SceneObjMgr�ɕۑ����Ă��炤
	GameMgr::GetInstance()->sceneObjMgr->RegisterCell(scoreText, true);//SceneObjMgr�ɕۑ����Ă��炤
	//���C�t�̏����l��ۑ�
	playerLifeMax = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();

	//score===============================
	//campos = MainCamera::GetInstance()->GetCamPos(); //�J�����ʒu�擾

	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i] = new BaseCell();
		m_score[i]->SetDrawLayer(E_DrawLayer::DrawLayer_Top);//�`�揇����ݒ肷��
		m_score[i]->SetID(GameMgr::GetInstance()->sceneObjMgr->GetID());//ID��ݒ肷��
		m_score[i]->SetCellType(E_CellType::CellType_UI);//�Q�[���I�u�W�F�N�g�̃^�C�v��UI�ɐݒ肷��
		m_score[i]->SetPos(vector3s(800.0f + uiLife_gap * i, 50.0f, 0.0f), true);//�ʒu

		GameMgr::GetInstance()->sceneObjMgr->RegisterCell(m_score[i], true);//SceneObjMgr�ɕۑ����Ă��炤
	}

	playerMaxPos = 0;
	m_number = 0;
}

/// <summary>
/// �^�C�g����ʂ�\������
/// </summary>
/// <param name="show"></param>
void UIMgr::ShowTitle(bool show)
{
	if (titleMenu == NULL)return;
	titleMenu->SetState(show);

	if (m_title != NULL)
		m_title->SetState(show);
}

/// <summary>
/// �Q�[���I�[�o�[��ʂ�\������
/// </summary>
/// <param name="show"></param>
void UIMgr::ShowEndMenu(bool show)
{
	if (endMenu == NULL)return;
	endMenu->SetState(show);

	int playerHP;
	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerHP = player->GetPlayerLifeValue();
	m_number = playerHP * (int)(playerMaxPos / 100);

	//m_number *= playerHP;
	scoreText->SetPos(vector3s(700.0f, SCREEN_LIMIT_DOWN - 50.0f, 0.0f), true);
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i]->SetPos(vector3s(1050.0f + score_gap * i, SCREEN_LIMIT_DOWN - 50.0f, 0.0f), true);//�ʒu
	}
	scoreText->SetState(show);
	ShowScore();
	DrawScore(show);


	resultEff->SetState(show);
}

void UIMgr::ShowGameOver(bool show)
{
	if (gameOver == NULL)return;
	gameOver->SetState(show);
}

//========================================================
//�ǉ�����

void UIMgr::ShowLogoMenu(bool show)
{
	if (logoMenu == NULL)return;
	logoMenu->SetState(show);
}

void UIMgr::DrawTeamLogo(bool show)
{
	if (teamLogo == NULL)return;
	teamLogo->SetState(show);
}

void UIMgr::DoUpdate(float deltatime)
{
	alphateamLogo = teamLogo_drawData->GetColor().a;

	switch (fadeStatus)
	{
	case FadeIn:
		if (alphateamLogo < 1.0f)
		{
			alphateamLogo += 0.005f;
			if (alphateamLogo >= 1.0f)
			{
				alphateamLogo = 1.0f;
			}
			teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
			if (alphateamLogo == 1.0)
				SetFadeStatus(FadeOut);
		}
		break;
	case FadeOut:
		if (alphateamLogo > 0.0f)
		{
			alphateamLogo -= 0.005f;
			if (alphateamLogo <= 0.0f)
			{
				alphateamLogo = 0.0f;
			}
			teamLogo_drawData->SetColor(D3DXCOLOR(1.0, 1.0, 1.0, alphateamLogo));
			if (alphateamLogo == 0.0)
				GameMgr::GetInstance()->SwitchGameState(E_GameState::GameState_Game_Title, true);
		}

	default:
		break;
	}

}
//bar�̍X�V�ƕ`��     by	���E
//================================================
void UIMgr::UpdateBar()
{
	bar->SetPos(vector3s((campos->x + 195), (campos->y + bar_y_offset), 0.0f), true);//�ʒu
	pIcon->SetPos(vector3s((campos->x + 345), (campos->y + bar_y_offset), 0.0f), true);//�ʒu
	eIcon->SetPos(vector3s((campos->x + 45), (campos->y + bar_y_offset), 0.0f), true);//�ʒu

	float maxX = 300;
	float ePosx = GameMgr::GetInstance()->sceneObjMgr->GetNezumi()->GetTransform()->GetPos().x;
	//float ePosx = 0;
	float pPosx = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetTransform()->GetPos().x;
	float tempx = abs(ePosx - pPosx) / BAR_SCALE_X;
	if (tempx >= 300)
	{
		tempx = 300;
	}
	eIcon->GetTransform()->SetPos(vector3s(pIcon->GetTransform()->GetPos().x - tempx, eIcon->GetTransform()->GetPos().y, 0));

}
void UIMgr::DrawBar(bool show)
{
	if (bar == NULL || eIcon == NULL || pIcon == NULL)
		return;

	bar->SetState(show);
	pIcon->SetState(show);
	eIcon->SetState(show);

}
//=================================

//����ǉ�=========================
void UIMgr::DoUpdate()
{
	if (m_HUDState == true) {

		DrawLife();
		DrawUIBullet();

		UpdateBar();

		UpdateScore();

		//�v���C���[�̃��C�t�̎擾
		playerLife = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();

		//uiLife_gap = 0.0f;
		for (int i = 0; i < UI_LIFE_NUM; i++)
		{
			uiLife[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 80.0f), 0.0f), true);//�ʒu
			//uiLife_gap += 50; //���C�t�̕\���ʒu�����炷
			Life_Drawflag[i] = true;
			DrawData* UiLife = uiLife[i]->GetDrawData();

			int LifePerUI = PLAYER_HP / UI_LIFE_NUM; // ���ɂ���1�������HP

			if (playerLife <= LifePerUI * (i + 1))
			{
				int LifeExcess = playerLife - (i * LifePerUI); //�����Ă��邨�ɂ���̕����̃��C�t��

				if (LifeExcess > (LifePerUI / 4) * 3) // 0126�C��
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
				}
				if (LifeExcess <= (LifePerUI / 4) * 3)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP2.png"));
				}
				if (LifeExcess <= (LifePerUI / 4) * 2)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP3.png"));
				}
				if (LifeExcess <= (LifePerUI / 4))
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP4.png"));
				}
				if (LifeExcess <= 0)
				{
					UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP5.png"));
				}

				uiLife[i]->SetDrawData(UiLife);
			}
		}
		//�e��UI
		uiBullet_gap = 0;

		// 01/23 ����ǉ�****************************
		if (playerLife <= PLAYER_HP) { //01/24�C��
			bulletPos = 680;
		}
		if (playerLife <= PLAYER_HP - 20) {
			bulletPos = 680 - uiLife_gap;
		}
		if (playerLife <= PLAYER_HP - (20 * 2)) {
			bulletPos = 680 - uiLife_gap * 2;
		}
		if (playerLife <= PLAYER_HP - (20 * 3)) {
			bulletPos = 680 - uiLife_gap * 3;
		}
		if (playerLife <= PLAYER_HP - (20 * 4)) {
			bulletPos = 680 - uiLife_gap * 4;
		}
		//*********************************************

		for (int n = 0; n < UI_BULLET_NUM; n++)
		{
			uiBullet[n]->SetPos(vector3s((campos->x + bulletPos) - uiBullet_gap, (campos->y + 160.0f), 0.0f), true);//�ʒu
			uiBullet_gap += 20;

			//�e��UI�̕\�������炷
			if (playerLife < PlyerLife_before)
			{
				if (Bullet_Drawflag[HpDecrease] == true)
				{
					HpDecrease++;
					Bullet_Drawflag[HpDecrease - 1] = false;
				}
			}
			//�e��UI�̕\����߂�
			if (HpDecrease >= UI_BULLET_NUM)
			{
				Bullet_Drawflag[n] = true;
				if (Bullet_Drawflag[UI_BULLET_NUM - 1] == true)
				{
					HpDecrease = 0;
				}
			}
			//��
			if (playerLife > PlyerLife_before)
			{
				// 0126 ����C��******************************
				int life = playerLife % 5;

				if (life == 0) {
					HpDecrease = 0;
				}
				else if (life == 1) {
					HpDecrease = 4;
				}
				else if (life == 2) {
					HpDecrease = 3;
				}
				else if (life == 3) {
					HpDecrease = 2;
				}
				else if (life == 4) {
					HpDecrease = 1;
				}

				for (int m = 0; m < UI_BULLET_NUM; m++) {
					if (life >= UI_BULLET_NUM - m) {
						Bullet_Drawflag[m] = true;
					}
				}
				//***************************************************
			}

			PlyerLife_before = playerLife;
		}
		//uiLife_gap = 0.0f;
		//for (int i = 0; i < UI_LIFE_NUM; i++)
		//{
		//	uiLife[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 120.0f), 0.0f), true);//�ʒu
		//	//uiLife_gap += 50; //���C�t�̕\���ʒu�����炷

		//	//HP�ɉ����Ă��ɂ����\��
		//	//HP100�`91->10�R
		//	//HP90�`81 ->9�R
		//	if ((i * 10) >= playerLife)
		//	{
		//		Life_Drawflag[i] = false;
		//	}
		//	else
		//	{
		//		Life_Drawflag[i] = true;
		//	}
		//}

		for (int i = 0; i < UI_NUM_COUNT; i++)
		{
			m_numArray[i]->SetPos(vector3s((campos->x + 120.0f) + uiLife_gap * i, (campos->y + 280.0f), 0.0f), true);//�ʒu
		}

		m_icon->SetPos(vector3s(campos->x + 150.0f, SCREEN_HEIGHT * 0.4f, 0.0f), true);//�ʒu
	}
	else {

		DrawScore(false);
		DrawBar(false);

		for (int i = 0; i < UI_BULLET_NUM; i++)
		{
			if (uiBullet[i] == NULL)return;
			uiBullet[i]->SetState(false);
		}
	}

}
//=================================

//========================================================

//����ǉ�=========================
//HPUI�̕\��
void UIMgr::DrawLife()
{
	if (m_HUDState == false)return;
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		if (uiLife[i] == NULL)return;
		uiLife[i]->SetState(Life_Drawflag[i]);
	}

}

//�eUI
void UIMgr::DrawUIBullet()
{
	for (int i = 0; i < UI_BULLET_NUM; i++)
	{
		if (uiBullet[i] == NULL)return;
		uiBullet[i]->SetState(Bullet_Drawflag[i]);
	}
}


//=================================

void UIMgr::ShowHUD(bool _show)
{
	m_HUDState = _show;
	for (int i = 0; i < UI_LIFE_NUM; i++)
	{
		if (uiLife[i] != NULL) {
			uiLife[i]->SetState(_show);
		}
	}
	if (_show == false) {
		for (int i = 0; i < UI_BULLET_NUM; i++)
		{
			if (uiBullet[i] == NULL)return;
			uiBullet[i]->SetState(false);
		}
	}
	//for (int i = 0; i < UI_NUM_COUNT; i++)
	//{
	//	if (m_numArray[i] != NULL) {
	//		m_numArray[i]->SetState(_show);
	//	}
	//}
	//if (m_icon != NULL) {
	//	m_icon->SetState(_show);
	//}
}

//score===============================

void UIMgr::UpdateScore()
{
	vector3s playerPos;

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	playerPos = player->GetTransform()->GetPos();

	if ((int)playerPos.x > playerMaxPos)
	{
		playerMaxPos = (int)playerPos.x;
		m_number = playerMaxPos / 100;
	}
	else
	{
		m_number = playerMaxPos / 100;
	}



	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		m_score[i]->SetPos(vector3s((campos->x + 1300 + score_gap * i), (campos->y + 80.0f), 0.0f), true);//�ʒu

	}

	ShowScore();

}

void UIMgr::DrawScore(bool show)
{
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		if (m_score[i] != NULL) {
			m_score[i]->SetState(show);
		}
	}
}

void UIMgr::ShowScore()
{
	for (int i = 0; i < UI_SCORE_COUNT; i++)
	{
		//�`��f�[�^�����
		DrawData* uiScore_drawData = new DrawData();
		uiScore_drawData->SetSize(vector2s(100.0f, 100.0f));
		uiScore_drawData->SetTextureNo(LoadTexture((char*)"data\\texture\\num.png"));
		uiScore_drawData->tex_w_count = TEX_NUM_COUNT_W;
		uiScore_drawData->tex_texel_size = num_tex_size;

		if (i == 0) {
			uiScore_drawData->SetShaderIntParam(3, m_number / 10000);
		}
		else if (i == 1) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 10000 / 1000);
		}
		else if (i == 2) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 1000 / 100);
		}
		else if (i == 3) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 100 / 10);
		}
		else if (i == 4) {
			uiScore_drawData->SetShaderIntParam(3, m_number % 10);
		}

		m_score[i]->SetDrawData(uiScore_drawData);//�`��f�[�^��n��
		//m_score[i]->SetState(false);//��\���ɂ���
	}
}

void UIMgr::ResetLifeUI()
{
	PlyerLife_before = 100;
	HpDecrease = 0;
	//�v���C���[�̃��C�t�̎擾
	//playerLife = GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->GetPlayerLifeValue();
	for (int i = 0; i < UI_LIFE_NUM; i++) {
		DrawData* UiLife = uiLife[i]->GetDrawData();
		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		uiLife[i]->SetDrawData(UiLife);
		//Life_Drawflag[i] = true;
		//DrawData* UiLife = uiLife[i]->GetDrawData();

		//int LifePerUI = PLAYER_HP / UI_LIFE_NUM; // ���ɂ���1�������HP

		//if (playerLife < LifePerUI * (i + 1))
		//{
		//	int LifeExcess = playerLife - (i * LifePerUI); //�����Ă��邨�ɂ���̕����̃��C�t��

		//	if (LifeExcess >= LifePerUI)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP1.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4) * 3)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP2.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4) * 2)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP3.png"));
		//	}
		//	if (LifeExcess <= (LifePerUI / 4))
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP4.png"));
		//	}
		//	if (LifeExcess <= 0)
		//	{
		//		UiLife->SetTextureNo(LoadTexture((char*)"data\\texture\\HP5.png"));
		//	}

		//	uiLife[i]->SetDrawData(UiLife);
		//}
	}
}

#pragma endregion ui_manager



#pragma region game_state

GameState::GameState() :gameStateType(E_GameState::GameState_None)
{

}

GameState::GameState(E_GameState state) : gameStateType(state) {

}

GameState::~GameState() {

}



void GameState::DoUpdate(float deltatime) {

}

//================================================================================
//�ǉ�����
#pragma region game_state_logo

GameState_Logo::GameState_Logo() :GameState(E_GameState::GameState_Game_Logo)
{

}

GameState_Logo::~GameState_Logo() {

}

void GameState_Logo::DoUpdate(float deltatime)
{
	GameMgr::GetInstance()->uiMgr->DoUpdate(deltatime);

}

void GameState_Logo::DoInit()
{

	GameMgr::GetInstance()->uiMgr->DrawTeamLogo(true);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Logo);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);
	GameMgr::GetInstance()->uiMgr->ShowLogoMenu(true);
	GameMgr::GetInstance()->effectMgr->HideSakura();

}

void GameState_Logo::Uninit()
{
	GameMgr::GetInstance()->uiMgr->teamLogo->SetState(false);
	GameMgr::GetInstance()->uiMgr->DrawTeamLogo(false);
	GameMgr::GetInstance()->uiMgr->ShowLogoMenu(false);
}

#pragma endregion game_state_logo

//================================================================================



#pragma region game_state_title

GameState_Title::GameState_Title() :GameState(E_GameState::GameState_Game_Title)
{

}

GameState_Title::~GameState_Title() {

}

void GameState_Title::DoUpdate(float deltatime)
{
	//BaseCell* tempCell_1 = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
	//Ground* ground_1 = (Ground*)tempCell_1;

	//BaseCell* tempCell_2 = GameMgr::GetInstance()->sceneObjMgr->CreateCell(E_CellType::CellType_Obstacle, 1, E_Obstacle::Obstacle_Ground);
	//Ground* ground_2 = (Ground*)tempCell_2;

	//tempCell_1->DeleteCell();
	//tempCell_2->DeleteCell();
}

void GameState_Title::DoInit()
{
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Title);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);
	GameMgr::GetInstance()->uiMgr->ShowTitle(true);
	GameMgr::GetInstance()->effectMgr->ShowSakura();
	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_title, -1);
}

void GameState_Title::Uninit()
{
	GameMgr::GetInstance()->uiMgr->ShowTitle(false);
	GameMgr::GetInstance()->effectMgr->HideSakura();

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_title);
}

#pragma endregion game_state_title

#pragma region game_state_game

GameState_Game::GameState_Game() {

}

GameState_Game::~GameState_Game() {

}

void GameState_Game::DoUpdate(float deltatime) {
	////=============����ǉ�===================
	GameMgr::GetInstance()->uiMgr->DoUpdate();
	//GameMgr::GetInstance()->uiMgr->DrawLife();
	//GameMgr::GetInstance()->uiMgr->DrawUIBullet();
	////========================================

	//	//bar�����@�@by���E
	////===========================================
	//GameMgr::GetInstance()->uiMgr->UpdateBar();
	//GameMgr::GetInstance()->uiMgr->DrawBar(true);
	////===========================================

	//	//score===========
	////===========================================
	//GameMgr::GetInstance()->uiMgr->UpdateScore();
	//GameMgr::GetInstance()->uiMgr->DrawScore(true);
	////===========================================
}

void GameState_Game::DoInit() {
	//load game scene
	//active spawner
	//switch input state
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(true);
	GameMgr::GetInstance()->uiMgr->SetEnemyCounter(0);

	GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_TestScene, true,true);//�V�[����؂�ւ���

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);//���̓��[�h��؂�ւ���
	GameMgr::GetInstance()->inputMgr->SetInputState(true);//���̓X�C�b�`���I���ɂ���r

	//23.1
	GameMgr::GetInstance()->uiMgr->ShowHUD(true);
	GameMgr::GetInstance()->uiMgr->SetScoreNum(0);

	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_GamePlay, -1);

}

void GameState_Game::Uninit() {
	//unload all unit(unload scene
	//inactive spawner
	//MainCamera::GetInstance()->ResetCamPos();
	//MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_None, true,false);//�V�[���̏I������
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_None);//���̓��[�h��؂�ւ���
	
	//23.1
	GameMgr::GetInstance()->uiMgr->ShowHUD(false);

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GamePlay);

}

#pragma endregion game_state_game

#pragma region game_state_menu


GameState_Menu::GameState_Menu() {

}

GameState_Menu::~GameState_Menu() {

}

void GameState_Menu::DoUpdate(float deltatime) {

}

void GameState_Menu::DoInit() {

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Menu);//���̓��[�h��؂�ւ���
	GameMgr::GetInstance()->inputMgr->SetInputState(true);//���̓X�C�b�`���I���ɂ���r

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);//�Q�[�����ꎞ��~����r
}

void GameState_Menu::Uninit() {

	//return to game input
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game);//���̓��[�h��؂�ւ���

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);//�Q�[�����ĊJ����
}

#pragma endregion game_state_menu

#pragma region game_state_lose

GameState_Lose::GameState_Lose() {

}

GameState_Lose::~GameState_Lose() {

}

void GameState_Lose::DoUpdate(float deltatime) {

}

void GameState_Lose::DoInit() {
	//set timer
	//load lose panel
	//set input state
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);

	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Lose_Menu);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);

	GameMgr::GetInstance()->uiMgr->ShowGameOver(true);
	GameMgr::GetInstance()->sceneObjMgr->UninitPlayerObjState();

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);

	PlaySound(GameMgr::GetInstance()->sceneMgr->m_sound_GameOver, -1);
}

void GameState_Lose::Uninit() {
	GameMgr::GetInstance()->sceneMgr->ResetCurScene();
	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);

	GameMgr::GetInstance()->uiMgr->ShowGameOver(false);

	//GameMgr::GetInstance()->sceneMgr->SwitchScene(E_Scene::Scene_TestScene, true, true);//�V�[����؂�ւ���
	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GameOver);
}

#pragma endregion game_state_lose

#pragma region game_state_end

GameState_End::GameState_End() {

}

GameState_End::~GameState_End() {

}

void GameState_End::DoUpdate(float deltatime) {

}

void GameState_End::DoInit() {
	MainCamera::GetInstance()->ResetCamPos();
	MainCamera::GetInstance()->SetScrollState(false);
	GameMgr::GetInstance()->inputMgr->SwitchInputMode(E_InputMode::InputMode_Game_Win_Menu);
	GameMgr::GetInstance()->inputMgr->SetInputState(true);

	GameMgr::GetInstance()->uiMgr->ShowHUD(false);

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 0.0f);
	GameMgr::GetInstance()->uiMgr->ShowEndMenu(true);

}

void GameState_End::Uninit() {

	GameMgr::GetInstance()->timer->SetTimer(E_Timer::Timer_Game_1, 1.0f);
	GameMgr::GetInstance()->uiMgr->ShowEndMenu(false);

	StopSound(GameMgr::GetInstance()->sceneMgr->m_sound_GameClear);
}

#pragma endregion game_state_end

#pragma endregion game_state

#pragma region game_manager

GameMgr::GameMgr() :curGameStateType(E_GameState::GameState_None), defaultGameStateType(E_GameState::GameState_None),
defaultSubGameStateType(E_GameState::GameState_None), curSubGameStateType(E_GameState::GameState_None),
curGameState(NULL), curSubGameState(NULL) {
	//�C���X�^���X��
	sceneMgr = new SceneMgr();
	inputMgr = new InputMgr();
	sceneObjMgr = new SceneObjMgr();
	effectMgr = new EffectMgr();
	uiMgr = new UIMgr();
	timer = new Timer();
	dataMgr = new DataManager();

	gameStateDic.clear();
	subGameStateDic.clear();
}

GameMgr::~GameMgr() {
	if (sceneMgr != NULL) {
		delete sceneMgr;
	}
	if (inputMgr != NULL) {
		delete inputMgr;
	}
	if (sceneObjMgr != NULL) {
		delete sceneObjMgr;
	}
	if (effectMgr != NULL) {
		delete effectMgr;
	}
	if (timer != NULL) {
		delete timer;
	}
	if (dataMgr != NULL) {
		delete dataMgr;
	}
	sceneMgr = NULL;
	inputMgr = NULL;
	sceneObjMgr = NULL;
	effectMgr = NULL;
	timer = NULL;
	dataMgr = NULL;
	//�Q�[���X�e�[�g���폜����
	map<E_GameState, GameState*>::iterator iter;
	for (iter = gameStateDic.begin(); iter != gameStateDic.end(); iter++) {
		if (iter->second == NULL)continue;
		delete iter->second;
	}
	for (iter = subGameStateDic.begin(); iter != subGameStateDic.end(); iter++) {
		if (iter->second == NULL)continue;
		delete iter->second;
	}

	gameStateDic.clear();
	subGameStateDic.clear();
}

void GameMgr::DoInit() {

	//�Q�[���X�e�[�g�����X�g�ɒǉ�����

	//===============================
	//�ǉ�����
	RegisterGameState(E_GameState::GameState_Game_Logo, new GameState_Logo(), true);
	//
	//==============
	RegisterGameState(E_GameState::GameState_Game_Title, new GameState_Title(), false);
	RegisterGameState(E_GameState::GameState_Game_Normal, new GameState_Game(), false);
	RegisterGameState(E_GameState::GameState_Game_End, new GameState_End(), false);

	RegisterSubGameState(E_GameState::GameState_Game_Menu, new GameState_Menu(), false);
	//RegisterSubGameState(E_GameState::GameState_Game_Lose, new GameState_Lose(), false);
	RegisterGameState(E_GameState::GameState_Game_Lose, new GameState_Lose(), false);
	//RegisterSubGameState(E_GameState::GameState_Game_End, new GameState_End(), false);

	//init all manager
	timer->Reset();

	sceneObjMgr->DoInit();//��񂵂����������Ȃ�

	effectMgr->DoInit();//��񂵂����������Ȃ�

	sceneMgr->DoInit();//��񂵂����������Ȃ�

	uiMgr->DoInit();//��񂵂����������Ȃ�

	dataMgr->DoInit();

	//�f�t�H���g�Q�[���X�e�[�g�@�^�f�t�H���g�T�u�Q�[���X�e�[�g�@�ɐ؂�ւ���
	SwitchGameState(defaultGameStateType, true);
	SwitchSubGameState(defaultSubGameStateType, true);
}

void GameMgr::DoUninit() {
	sceneMgr->UnInit();
	effectMgr->UnInit();
	sceneObjMgr->UnInit(true);
}

/// <summary>
/// �S�̂̃A�b�v�f�[�g�����̏o���_
/// </summary>
/// <param name="deltatime"></param>
void GameMgr::DoUpdate(float deltatime) {
	inputMgr->DoUpdate(deltatime);//���̓A�b�v�f�[�g
	effectMgr->DoUpdate(deltatime * timer->gameTimer_1);//�G�t�F�N�g�A�b�v�f�[�g
	sceneObjMgr->DoUpdate(deltatime);//�Q�[���I�u�W�F�N�g�A�b�v�f�[�g
	//sceneObjMgr->DoLateUpdate(deltatime * timer->uiTimer);
	MainCamera::GetInstance()->DoUpdate(deltatime * timer->gameTimer_1);
	sceneMgr->DoUpdate(deltatime * timer->gameTimer_1);//�V�[���A�b�v�f�[�g
	timer->DoUpdate(deltatime);

	if (curGameState != NULL) {
		curGameState->DoUpdate(deltatime);//�Q�[���X�e�[�g�̃A�b�v�f�[�g
	}
	if (curSubGameState != NULL) {
		curSubGameState->DoUpdate(deltatime);//�T�u�Q�[���X�e�[�g�̃A�b�v�f�[�g
	}
}

void GameMgr::DoLateUpdate(float deltatime) {

}

/// <summary>
/// �Q�[���X�e�[�g��؂�ւ���
/// </summary>
/// <param name="gameState">�@���̃X�e�[�g</param>
/// <param name="reset">�@�����X�e�[�g�̏ꍇ�A������x���������邩�ǂ���</param>
void GameMgr::SwitchGameState(E_GameState gameState, bool reset)
{
	if (gameState == E_GameState::GameState_None) {
		if (curGameState != NULL) {
			curGameState->Uninit();
			curGameState = NULL;
		}
		curGameStateType = E_GameState::GameState_None;
		return;
	}

	if (gameState == curGameStateType) {//���̃X�e�[�g�����̃X�e�[�g�Ɠ����ł����
		if (reset == false) {
			return;
		}
		else {
			if (curGameState != NULL) {
				curGameState->Uninit();
				curGameState->DoInit();
			}
			return;
		}
	}

	if (curGameState != NULL) {
		curGameState->Uninit();//���̃X�e�[�g�̏I������
		curGameState = NULL;
	}

	if (gameStateDic.find(gameState) == gameStateDic.end()) {//���̃X�e�[�g��������Ȃ��ꍇ
		return;
	}

	curGameState = gameStateDic[gameState];
	if (curGameState != NULL) {
		curGameState->DoInit();//���̃X�e�[�g������������
		curGameStateType = gameState;
	}
}

/// <summary>
/// �T�u�Q�[���X�e�[�g��؂�ւ���
/// </summary>
/// <param name="subGameState">�@���̃X�e�[�g</param>
/// <param name="reset">�@�����X�e�[�g�̏ꍇ�A������x���������邩�ǂ���</param>
void GameMgr::SwitchSubGameState(E_GameState subGameState, bool reset)
{
	if (subGameState == E_GameState::GameState_None) {
		if (curSubGameState != NULL) {
			curSubGameState->Uninit();
			curSubGameState = NULL;
		}
		curSubGameStateType = E_GameState::GameState_None;
		return;
	}

	if (subGameState == curSubGameStateType) {//���̃X�e�[�g�����̃X�e�[�g�Ɠ����ł����
		if (reset == false) {
			return;
		}
		else {
			if (curSubGameState != NULL) {
				curSubGameState->Uninit();
				curSubGameState->DoInit();
			}
			return;
		}
	}

	if (curSubGameState != NULL) {
		curSubGameState->Uninit();//���̃X�e�[�g�̏I������
		curSubGameState = NULL;
	}

	if (subGameStateDic.find(subGameState) == subGameStateDic.end()) {//���̃X�e�[�g��������Ȃ��ꍇ
		return;
	}

	curSubGameState = subGameStateDic[subGameState];
	if (curSubGameState != NULL) {
		curSubGameState->DoInit();//���̃X�e�[�g������������
		curSubGameStateType = subGameState;
	}
}

/// <summary>
/// �V�����X�e�[�g��ǉ�����
/// </summary>
/// <param name="gameStateType"></param>
/// <param name="gameState"></param>
/// <param name="setDefault"></param>
void GameMgr::RegisterGameState(E_GameState gameStateType, GameState* gameState, bool setDefault)
{
	if (gameState == NULL)return;
	if (gameStateDic.find(gameStateType) == gameStateDic.end()) {
		gameStateDic[gameStateType] = gameState;
		if (setDefault) {
			defaultGameStateType = gameStateType;
		}
	}
	else {
		//error
	}
}

/// <summary>
/// �V�����T�u�X�e�[�g��ǉ�����
/// </summary>
/// <param name="gameStateType"></param>
/// <param name="gameState"></param>
/// <param name="setDefault"></param>
void GameMgr::RegisterSubGameState(E_GameState gameStateType, GameState* gameState, bool setDefault)
{
	if (gameState == NULL)return;
	if (subGameStateDic.find(gameStateType) == subGameStateDic.end()) {
		subGameStateDic[gameStateType] = gameState;
		if (setDefault) {
			defaultSubGameStateType = gameStateType;
		}
	}
	else {
		//error
	}
}

/// <summary>
/// ���̃X�e�[�g���擾����
/// </summary>
/// <returns></returns>
E_GameState GameMgr::GetCurGameState()
{
	return curGameStateType;
}

/// <summary>
/// ���̃T�u�X�e�[�g���擾����
/// </summary>
/// <returns></returns>
E_GameState GameMgr::GetCurSubGameState() {
	return curSubGameStateType;
}

#pragma endregion game_manager

#pragma region timer_method

Timer::Timer() {
	Reset();
}

Timer::~Timer() {

}

void Timer::SetTimer(E_Timer timerType, bool limitForward) {
	switch (timerType) {
	case E_Timer::Timer_Game_1:
		gameTimer_1 = limitForward ? 1.0f : 0.0f;
		break;
	case E_Timer::Timer_Game_2:
		gameTimer_2 = limitForward ? 1.0f : 0.0f;
		break;
	case E_Timer::Timer_UI:
		uiTimer = limitForward ? 1.0f : 0.0f;
		break;
	default:
		break;
	}
}

void Timer::SetTimer(E_Timer timerType, float value)
{
	switch (timerType) {
	case E_Timer::Timer_Game_1:
		gameTimer_1 = value;
		break;
	case E_Timer::Timer_Game_2:
		gameTimer_2 = value;
		break;
	case E_Timer::Timer_UI:
		uiTimer = value;
		break;
	default:
		break;
	}
}

void Timer::Reset()
{
	gameTimer_1 = 1.0f;
	gameTimer_2 = 1.0f;
	uiTimer = 1.0f;
}

void Timer::DoUpdate(float deltatime)
{
}

#pragma endregion timer_method

//AI�L���I�[�g�}�g��
#pragma region fsm

FSMSystem::FSMSystem() :curState(NULL), parent(NULL),curStateType(E_FSMState::FSMState_Idle),defaultStateType(E_FSMState::FSMState_Idle) {

}

FSMSystem::FSMSystem(BaseCell* p) : parent(p), curState(NULL), curStateType(E_FSMState::FSMState_Idle), defaultStateType(E_FSMState::FSMState_Idle) {

}

FSMSystem::~FSMSystem() {
	if (stateDic.size() != 0) {
		map<E_FSMState, FSMState*>::iterator iter;
		for (iter = stateDic.begin(); iter != stateDic.end(); iter++) {
			if (iter->second != NULL) {
				delete iter->second;
			}
		}
	}
	stateDic.clear();

	curState = NULL;
}


void FSMSystem::DoInit() {
	//register state
	SwitchState(defaultStateType, true);
}

void FSMSystem::RegisterState(E_FSMState stateType, FSMState* state, bool setDefaultState) {
	if (state == NULL)return;

	if (stateDic.find(stateType) != stateDic.end()) {
		//error
		return;
	}

	stateDic[stateType] = state;

	if (setDefaultState) {
		defaultStateType = stateType;
	}
}

void FSMSystem::DoUpdate(float deltatime) {

	if (curState != NULL) {
		curState->DoUpdate(deltatime);
	}

}

void FSMSystem::SwitchState(E_FSMState targetStateType, bool resetState) {
	if (curStateType == targetStateType && resetState == false) {
		return;
	}

	if (stateDic.find(targetStateType) == stateDic.end()) {
		return;
	}

	if (curState != NULL) {
		curState->EndState();
		curState = NULL;
	}

	curState = stateDic[targetStateType];

	if (curState != NULL) {
		curState->DoInit();
		curState->StartState();
	}
}

void FSMSystem::SetParent(BaseCell* p)
{
	parent = p;
}

BaseCell* FSMSystem::GetParent() {
	return parent;
}


//state
FSMState::FSMState() :parentSystem(NULL), action(NULL), enable(false),stateType(E_FSMState::FSMState_Idle) {
	triggerDic.clear();
}

void FSMState::CheckStateTrigger(float deltatime) {
	if (triggerDic.size() == 0) {
		return;
	}

	map<E_FSMState, FSMTriggerGroup*>::iterator iter;
	for (iter = triggerDic.begin(); iter != triggerDic.end(); iter++) {
		if (iter->second == NULL)continue;
		if (iter->second->CheckTrigger(deltatime) == true) {
			//switch state
			if (parentSystem != NULL) {
				this->parentSystem->SwitchState(iter->first, true);
				return;
			}
		}
	}
}

FSMState::FSMState(FSMSystem* parent) :action(NULL), parentSystem(parent), enable(false), stateType(E_FSMState::FSMState_Idle) {

}

FSMState::~FSMState() {
	action = NULL;
}

void FSMState::DoInit() {
	map<E_FSMState, FSMTriggerGroup*>::iterator iter;
	for (iter = triggerDic.begin(); iter != triggerDic.end(); iter++) {
		if (iter->second == NULL)continue;
		iter->second->ResetTriggerState();
	}

	if (action != NULL) {
		(*action).Reset();
	}
}

void FSMState::DoUpdate(float deltatime) {
	if (enable == true) {
		DoAction(deltatime);

		//check state trigger
		CheckStateTrigger(deltatime);
	}
}

void FSMState::SetAction(FSMStateAction* a)
{
	if (a == NULL)return;
	action = a;
}

void FSMState::DoAction(float deltatime) {
	if (action != NULL) {
		(*action)(parentSystem->GetParent(), deltatime);
	}
}


void FSMState::StartState() {
	//start cb

	enable = true;
}

void FSMState::EndState() {
	//end cb

	enable = false;
}

void FSMState::PauseState(bool state)
{
	enable = state;
}

BaseCell* FSMState::GetParentCell() {
	if (parentSystem == NULL) {
		return NULL;
	}

	return parentSystem->GetParent();
}

void FSMState::RegisterTrigger(E_FSMState targetState, FSMTriggerGroup* triggerGroup)
{
	if (triggerGroup == NULL)return;

	if (triggerDic.find(targetState) == triggerDic.end()) {
		triggerDic[targetState] = triggerGroup;
	}
}


//trigger group
FSMTriggerGroup::FSMTriggerGroup() :checkTriggerType(E_CheckTriggerType::CheckTriggerType_AllTrue), parentSystem(NULL) {

}

FSMTriggerGroup::FSMTriggerGroup(E_CheckTriggerType checkType, FSMSystem* parent) : checkTriggerType(checkType), parentSystem(parent) {

}

FSMTriggerGroup::~FSMTriggerGroup()
{
	if (triggerArray.size() != 0) {
		vector<FSMStateTrigger*>::iterator iter;
		for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
			if ((*iter) != NULL) {
				delete (*iter);
			}
		}
	}
	triggerArray.clear();
}

E_CheckTriggerType FSMTriggerGroup::GetCheckType() {
	return checkTriggerType;
}

vector<FSMStateTrigger*>& FSMTriggerGroup::GetTriggerArray() {
	return triggerArray;
}

void FSMTriggerGroup::AddTrigger(FSMStateTrigger* trigger) {
	if (trigger == NULL)return;
	triggerArray.push_back(trigger);
}

bool FSMTriggerGroup::CheckTrigger(float deltatime) {
	if (triggerArray.size() == 0) {
		return false;
	}
	int falseSign = 0;
	int trueSign = 0;
	int allFalseSign = 1;
	int allTrueSign = 1;
	bool tempSign = 0;
	int tempSignInt = 0;
	vector<FSMStateTrigger*>::iterator iter;

	for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		tempSign = (*iter)->Check(parentSystem, deltatime);
		tempSignInt = tempSign == true ? 1 : 0;

		trueSign |= tempSignInt;
		allTrueSign &= tempSignInt;

		falseSign |= (~tempSignInt) & 1;
		allFalseSign &= (~tempSignInt) & 1;
	}

	switch (checkTriggerType) {
	case E_CheckTriggerType::CheckTriggerType_OneTrue:
		return trueSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_OneFalse:
		return falseSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AllTrue:
		return allTrueSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AllFalse:
		return allFalseSign ? true : false;
	case E_CheckTriggerType::CheckTriggerType_AlwaysTrue:
		return true;
	case E_CheckTriggerType::CheckTriggerType_AlwaysFalse:
		return false;
	default:
		return false;
	}
	return true;
}

void FSMTriggerGroup::ResetTriggerState()
{
	vector<FSMStateTrigger*>::iterator iter;
	for (iter = triggerArray.begin(); iter != triggerArray.end(); iter++) {
		if ((*iter) == NULL)continue;
		(*iter)->ResetTriggerState();
	}
}


//trigger

FSMStateTrigger::FSMStateTrigger() :parentState(NULL), triggerAction(NULL)
{

}

FSMStateTrigger::~FSMStateTrigger()
{
	parentState = NULL;
	if (triggerAction != NULL) {
		delete triggerAction;
	}
	triggerAction = NULL;
}

void FSMStateTrigger::SetTriggerParam(FSMState* parent, FSMStateTriggerAction* action) {
	if (parent == NULL || action == NULL)return;

	parentState = parent;
	triggerAction = action;
}

bool FSMStateTrigger::Check(FSMSystem* parentSystem, float deltatime) {
	if (parentSystem == NULL)return false;

	//judge
	if (triggerAction == NULL || parentState == NULL)return false;

	return (*triggerAction)(parentState->GetParentCell(), deltatime);
}

void FSMStateTrigger::ResetTriggerState()
{
	if (triggerAction != NULL) {
		triggerAction->Reset();
	}
}

#pragma endregion fsm

#pragma region ai

#pragma region state_action

FSMStateAction::FSMStateAction() :init(false), setAnim(E_Anim::Anim_None),actionDone(false)
{

}

void FSMStateAction::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL) {
		return;
	}

	//do action
}

void FSMStateAction::Reset()
{
	init = false;
}
#pragma region enemy_base_action

void FSMState_Base_Action_Idle::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		parentCell->SetMovementState(E_MovementType::MovementType_None, false,true);
		init = true;
		actionDone = false;
	}
}

void FSMState_Base_Action_Move2Player::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		init = true;
		actionDone = false;
	}

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player == NULL)return;

	vector3s moveDir = player->GetTransform()->GetPos() - parentCell->GetTransform()->GetPos();
	moveDir.z = 0.0f;

	moveDir = VecNormalize(moveDir);
	moveDir = moveDir * (moveSpeed * deltatime);
	vector3s newPos = parentCell->GetTransform()->GetPos() + moveDir;
	parentCell->SetPos(newPos, true);

}

void FSMState_Base_Action_Dmg::operator()(BaseCell* parentCell, float delattime) {
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		init = true;
		actionDone = false;
	}
}

void FSMState_Base_Action_Partrol::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return;
	if (init == false) {
		if (setAnim != E_Anim::Anim_None) {
			Animation* cellAnimator = (Animation*)parentCell->GetComponent(E_Component::Component_Animation);
			if (cellAnimator != NULL) {
				cellAnimator->SwitchAnim(setAnim, true, true);
			}
		}
		parentCell->StartMovement(E_MovementType::MovementType_None,false,true);
		init = true;
		actionDone = false;
	}

}


#pragma endregion enemy_base_action

#pragma endregion state_action

#pragma region trigger_action

bool FSMStateTriggerAction::operator()(BaseCell* parentCell, float deltatime) {
	if (parentCell == NULL)return false;

	return false;
}

void FSMStateTriggerAction::Reset()
{
	init = false;
}

//idle time
bool FSMStateTrigger_Base_Trigger_IdleTime::operator()(BaseCell* parentCell, float deltatime)
{
	if (init == false) {
		timePass = 0.0f;
		init = true;
	}

	timePass += deltatime;
	if (timePass > idleTime) {
		return true;
	}

	return false;
}

//dist 2 player
bool FSMStateTrigger_Base_Trigger_Dist2Player::operator()(BaseCell* parentCell, float deltatime)
{
	if (parentCell == NULL)return false;
	if (init == false) {
		init = true;
	}

	Player* player = GameMgr::GetInstance()->sceneObjMgr->GetPlayer();
	if (player == NULL)return false;

	vector3s cell2Player = player->GetTransform()->GetPos() - parentCell->GetTransform()->GetPos();
	cell2Player.z = 0.0f;

	float curDist = VecLengthS(cell2Player);
	if (curDist < dist * dist) {
		return true;
	}

	return false;
}

//dmg time
bool FSMStateTrigger_Base_Trigger_TakeDmg::operator()(BaseCell* parentCell, float deltatime)
{
	if (init == false) {
		init = true;
	}

	if (parentCell == NULL) {
		return true;
	}

	//if (parentCell->CheckCanMove()) {
	//	return false;
	//}
	//else {
	//	return true;
	//}

	return true;
}


#pragma endregion trigger_action


#pragma endregion ai

