/*==============================================================================

   DirectX�̏����� [main.cpp]
                                                         Author : 
                                                         Date   : 
--------------------------------------------------------------------------------

==============================================================================*/
#include "main.h"
#include "Player.h"
//*****************************************************************************
// ���C�u�����̃����N0.
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME			"GameWindow"				// �E�C���h�E�̃N���X��
#define WINDOW_CAPTION		"�u����I�ꍇ�N�I�v"			// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************

#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_CAPTION;	// �f�o�b�O�����\���p

#endif

vector2s preMousePos = vector2s(0.0f, 0.0f);
vector2s curMousePos = vector2s(0.0f, 0.0f);
vector2s curMouseMovement = vector2s(0.0f, 0.0f);
HWND hwndInstance;
											//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);

	RegisterClass(&wc);
	
	// �E�B���h�E�̍쐬
	HWND hWnd = CreateWindow(CLASS_NAME,
						WINDOW_CAPTION,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// �E�B���h�E�̍����W
						CW_USEDEFAULT,																		// �E�B���h�E�̏���W
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// �E�B���h�E����
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
						NULL,
						NULL,
						hInstance,
						NULL);
	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}


	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;
	
	// �E�C���h�E�̕\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG			msg;

	hwndInstance = hWnd;

	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// �V�X�e���������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_CAPTION);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

				dwFrameCount++;		// �����񐔂̃J�E���g�����Z
			}
		}
	}
	
	timeEndPeriod(1);				// ����\��߂�

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł����H", "�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	};

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
RECT clipRect;

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);
	//���͏����̏�����
	InitInput(hInstance, hWnd);
	InitSound(hWnd);

	InitSprite();

	InitGame();
	//InitLineCircle();
	////�Q�[���֘A������
	//InitSprite();
	//InitPolygon();		//�|���S���̏����������s
	//InitPlayer();
	//InitEnemy();
	//InitBullet();
	//InitEnemyBullet();
	//InitExplosion();
	GetWindowRect(hWnd, &clipRect);
	ShowCursor(true);

	POINT mouseM = POINT();
	mouseM.x = SCREEN_WIDTH * 0.5f;
	mouseM.y = SCREEN_HEIGHT * 0.5f;
	ClientToScreen(hWnd,&mouseM);
	SetCursorPos(mouseM.x, mouseM.y);

	//POINT mouseM = POINT();
	//GetCursorPos(&mouseM);
	//ScreenToClient(GetHWnd(), &mouseM);

	preMousePos.x = GetMouseX();
	preMousePos.y = GetMouseY();
	curMousePos = preMousePos;


	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	UninitPolygon();	//�|���S���̏I������

	//���͏����̏I��
	UninitInput();
	UninitSound();
	UninitSprite();
	UninitTexture();
	//UninitLineCircle();
	UninitGame();
	//UninitPlayer();
	//UninitEnemy();
	//UninitSprite();
	//UninitBullet();
	//UninitExplosion();
	//// �����_�����O�̏I������
	//UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	//���͏����̍X�V
	UpdateInput();
	if (GameMgr::GetInstance()->sceneObjMgr->GetPlayer()->inputLock == false) {
		//GetWindowRect(hwndInstance, &clipRect);
		//ClipCursor(&clipRect);
	}

	preMousePos = curMousePos;
	curMousePos.x = GetMouseX()* MOUSE_SENSITY;
	curMousePos.y = GetMouseY()* MOUSE_SENSITY;
	UpdateGame();

	//UpdateLineCircle();
	//UpdatePolygon();	//�|���S���̍X�V����
	//UpdatePlayer();
	//UpdateEnemy();
	//UpdateBullet();
	//UpdateExplosion();
	//UpdateCollision();
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(true);
	SetBlendState(BLEND_MODE::BLEND_MODE_ALPHABLEND);
	//DrawLineCircle();
	DrawGame();
	//DrawPolygon();			//�|���S���̕\��
	//DrawPlayer();
	//DrawEnemy();
	//DrawBullet();
	//DrawExplosion();
	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


vector2s GetMouseMovement() {
	//return curMousePos - preMousePos;
	return curMousePos;
}

HWND& GetHWnd()
{
	return hwndInstance;
}

bool AdjustScreenPoint(vector3s& pos, vector2s size)
{
	/*	if (pos.x + size.x * 0.5f > SCREEN_WIDTH) {
			pos.x = SCREEN_WIDTH- size.x * 0.5f;
		}
		else*/ if (pos.x - size.x * 0.5f < 0) {
			pos.x = size.x * 0.5f;
		}

		if (pos.y + size.y * 0.5f > SCREEN_HEIGHT) {
			pos.y = SCREEN_HEIGHT - size.y * 0.5f;
			return true;
		}
		else if (pos.y - size.y * 0.5f < 0) {
			//pos.y = size.y * 0.5f;
		}

		return false;
}

//