/*==============================================================================

   DirectXの初期化 [main.cpp]
                                                         Author : 
                                                         Date   : 
--------------------------------------------------------------------------------

==============================================================================*/
#include "main.h"
#include "Player.h"
//*****************************************************************************
// ライブラリのリンク0.
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
// マクロ定義
//*****************************************************************************
#define CLASS_NAME			"GameWindow"				// ウインドウのクラス名
#define WINDOW_CAPTION		"「走れ！一合君！」"			// ウインドウのキャプション名

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_CAPTION;	// デバッグ文字表示用

#endif

vector2s preMousePos = vector2s(0.0f, 0.0f);
vector2s curMousePos = vector2s(0.0f, 0.0f);
vector2s curMouseMovement = vector2s(0.0f, 0.0f);
HWND hwndInstance;
											//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
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
	
	// ウィンドウの作成
	HWND hWnd = CreateWindow(CLASS_NAME,
						WINDOW_CAPTION,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// ウィンドウの左座標
						CW_USEDEFAULT,																		// ウィンドウの上座標
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// ウィンドウ横幅
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
						NULL,
						NULL,
						hInstance,
						NULL);
	// DirectXの初期化(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}


	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;
	
	// ウインドウの表示(Init()の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG			msg;

	hwndInstance = hWnd;

	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
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
			dwCurrentTime = timeGetTime();					// システム時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_CAPTION);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
				SetWindowText(hWnd, g_DebugStr);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

				dwFrameCount++;		// 処理回数のカウントを加算
			}
		}
	}
	
	timeEndPeriod(1);				// 分解能を戻す

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
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
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
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
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダリング処理の初期化
	InitRenderer(hInstance, hWnd, bWindow);
	//入力処理の初期化
	InitInput(hInstance, hWnd);
	InitSound(hWnd);

	InitSprite();

	InitGame();
	//InitLineCircle();
	////ゲーム関連初期化
	//InitSprite();
	//InitPolygon();		//ポリゴンの初期化を実行
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
// 終了処理
//=============================================================================
void Uninit(void)
{
	UninitPolygon();	//ポリゴンの終了処理

	//入力処理の終了
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
	//// レンダリングの終了処理
	//UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	//入力処理の更新
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
	//UpdatePolygon();	//ポリゴンの更新処理
	//UpdatePlayer();
	//UpdateEnemy();
	//UpdateBullet();
	//UpdateExplosion();
	//UpdateCollision();
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// 2D描画なので深度無効
	SetDepthEnable(true);
	SetBlendState(BLEND_MODE::BLEND_MODE_ALPHABLEND);
	//DrawLineCircle();
	DrawGame();
	//DrawPolygon();			//ポリゴンの表示
	//DrawPlayer();
	//DrawEnemy();
	//DrawBullet();
	//DrawExplosion();
	// バックバッファ、フロントバッファ入れ替え
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