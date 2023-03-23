/*==============================================================================

   頂点管理 [polygon.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include	"polygon.h"
#include	"Line.h"
#include	"LineCircle.h"
#include "Sprite.h"
#include "Texture.h"

#include "input.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_VERTEX 4
#define	NUM_TRIANGLE_VERTEX	(4)	//三角形用頂点数

#define	BOX_W	(700)	//四角形の横のサイズ
#define	BOX_H	(700)	//四角形の縦のサイズ

#define RUNNING_MAN_W 700
#define RUNNING_MAN_H 400
#define RUNNING_MAN_W_COUNT 5
#define RUNNING_MAN_H_COUNT 2
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//void SetVertex(D3DXVECTOR3, D3DXVECTOR3, D3DXCOLOR);	//直線を表示する
//void SetVertexTringle(float, float, int, int, float, D3DXCOLOR);	//三角形用頂点格納処理

//*****************************************************************************
// グローバル変数
//*****************************************************************************
//static	ID3D11Buffer		*g_VertexBuffer = NULL;		// 頂点情報
static	ID3D11Buffer		*g_VertexBufferTriangle = NULL;

static	ID3D11ShaderResourceView	*g_Texture = NULL;		//テクスチャ情報　画像1枚につき変数1つ必要
static	ID3D11ShaderResourceView	*g_BGTexture = NULL;	//テクスチャ情報　画像1枚につき変数1つ必要
static	char	*g_TextureName = (char*)"data\\texture\\runningman003.png";//テクスチャファイルパス
static	char	*g_BGTextureName = (char*)"data\\texture\\snow_template1.jpg";//テクスチャファイルパス


float	PositionX = SCREEN_WIDTH / 2;//画面中央座標
float	PositionY = SCREEN_HEIGHT / 2;
float	Kakudo = 0;

D3DXVECTOR3		MoveVec;

//typedef struct {
//	D3DXVECTOR3 Position;//表示座標
//	D3DXVECTOR2 Size;//サイズ
//	D3DXCOLOR Color;//色
//	float Rotate;//角度
//
//}PLAYER;
//PLAYER mario;
typedef struct {
	D3DXVECTOR3 Position;//表示座標
	D3DXVECTOR2 Size;//サイズ
	D3DXCOLOR Color;//色
	float Rotate;//角度

}BG;

BG* sampleBG;

//テクスチャインデックス
int TextureNo = 0;
int TextureNoBG = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPolygon(void)
{

	sampleBG = new BG();
	sampleBG->Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	sampleBG->Size = D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT);
	sampleBG->Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//mario.Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	////mario.Size = D3DXVECTOR2(SCREEN_WIDTH/2, BOX_H/2);
	//mario.Size = D3DXVECTOR2(RUNNING_MAN_W / RUNNING_MAN_W_COUNT, RUNNING_MAN_H / RUNNING_MAN_H_COUNT);
	//mario.Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);



	//表示座標をランダムに決める
	PositionX = rand() % SCREEN_WIDTH;
	PositionY = rand() % SCREEN_HEIGHT;

	D3DXVECTOR3		StartPos = D3DXVECTOR3(PositionX, PositionY, 0);
	D3DXVECTOR3		EndPos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);

	MoveVec = EndPos - StartPos;				//GOALへのベクトル作成
	D3DXVec3Normalize(&MoveVec, &MoveVec);		//MoveVecを正規化する 長さを１にする		
	MoveVec *= 2.0f;							//移動の長さを乗算する


	ID3D11Device *pDevice = GetDevice();


	//{//三角形用の頂点初期化処理
	//	// 頂点バッファ生成
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.Usage = D3D11_USAGE_DYNAMIC;
	//	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_TRIANGLE_VERTEX;//頂点1個＊頂点数のサイズで作る
	//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBufferTriangle);
	//	//三角形用頂点格納処理
	//	D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//	SetVertexTringle(PositionX, PositionY, BOX_W, BOX_H, 0, col);
	//}

	
	////	テクスチャのロード
	//D3DX11CreateShaderResourceViewFromFile(
	//	pDevice,
	//	g_TextureName,	//テクスチャ画像のファイル名
	//	NULL,
	//	NULL,
	//	&g_Texture,		//読み込んだテクスチャの情報が入る変数のポインタ
	//	NULL
	//);

	////	BGテクスチャのロード
	//D3DX11CreateShaderResourceViewFromFile(
	//	pDevice,
	//	g_BGTextureName,	//テクスチャ画像のファイル名
	//	NULL,
	//	NULL,
	//	&g_BGTexture,		//読み込んだテクスチャの情報が入る変数のポインタ
	//	NULL
	//);

	//TextureNo = LoadTexture(g_TextureName);
	//if (TextureNo == -1) {
	//	exit(999);//強制終了
	//}
	//TextureNoBG = LoadTexture(g_BGTextureName);
	//if (TextureNoBG == -1) {
	//	exit(999);
	//}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPolygon(void)
{
	//// 頂点バッファの解放
	//if (g_VertexBuffer)
	//{
	//	g_VertexBuffer->Release();//使い終わったので解放する
	//	g_VertexBuffer = NULL;
	//}
	//UninitLine();	//直線の終了処理





	if (g_VertexBufferTriangle)
	{
		g_VertexBufferTriangle->Release();//使い終わったので解放する
		g_VertexBufferTriangle = NULL;
	}

	//テクスチャの解放
	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

	//BGテクスチャの解放
	if (g_BGTexture)
	{
		g_BGTexture->Release();
		g_BGTexture = NULL;
	}

	if (sampleBG != NULL) {
		delete sampleBG;
	}
	//UninitLineCircle();
	UninitTexture();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePolygon(void)
{

	//if (GetKeyboardPress(DIK_UP)) {
	//	mario.Position.y -= 5.0f;
	//}
	//else if (GetKeyboardPress(DIK_DOWN)) {
	//	mario.Position.y += 5.0f;
	//}

	//if (IsMouseLeftTriggered()) {
	//	mario.Position.x = GetMouseX();
	//	mario.Position.y = GetMouseY();
	//}

	//UpdateLine();	//直線の更新処理

	//UpdateLineCircle();

	//PositionX += MoveVec.x;
	//PositionY += MoveVec.y;
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPolygon(void)
{
	{
		//// 表示したいポリゴンの頂点バッファを設定
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// ２Ｄ表示をするためのマトリクスを設定
		SetWorldViewProjection2D();

		//// プリミティブトポロジ設定
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// マテリアル設定
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//SetMaterial(material);

	}
	return;
	{
		//// 表示したいポリゴンの頂点バッファを設定
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);
		//// プリミティブトポロジ設定
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//三角形の頂点データを格納
		
		//テクスチャのセット
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_BGTexture);
		//GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNoBG));
		//// ポリゴン描画
		//D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//DrawSpriteColorRotate(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0, col);
	}

	{
		//// 表示したいポリゴンの頂点バッファを設定
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);
		//// プリミティブトポロジ設定
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//三角形の頂点データを格納
		
		//テクスチャのセット
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
		//GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNo));
		//// ポリゴン描画
		//D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//DrawSpriteColorRotate(PositionX, PositionY, BOX_W / 2, BOX_H / 2, 35, col);
	}


	{
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_BGTexture);
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNoBG));

		DrawSpriteColorRotate(
			sampleBG->Position.x,
			sampleBG->Position.y,
			sampleBG->Size.x,
			sampleBG->Size.y,
			sampleBG->Rotate,
			sampleBG->Color,
			0,1.0f,1.0f,1
		);
	}
	return;
	{
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNo));
		static float patern = 0;//テスト用
		//DrawSpriteColorRotate(
		//	mario.Position.x,
		//	mario.Position.y,
		//	mario.Size.x,
		//	mario.Size.y,
		//	mario.Rotate,
		//	mario.Color,
		//	patern,1.0f/5,1.0f/2.0f,5
		//);

		patern += 0.05f;
		if (patern >= 10.0f) {
			patern -= 10.0f;
		}
	}
}




////=============================================================================
//// 頂点データ設定
////
////in
//// 開始座標, 終了座, カラー
////
////=============================================================================
//void SetVertex(D3DXVECTOR3 StartPos, D3DXVECTOR3 EndPos, D3DXCOLOR col)
//{
//	D3D11_MAPPED_SUBRESOURCE msr;
//	//頂点バッファへの書き込み許可をもらう
//	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//
//	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
//
//	//一本目の直線の頂点をセット
//	vertex[0].Position = StartPos;	// D3DXVECTOR3(100.0f, 100.0f, 0.0f);
//	vertex[0].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
//	vertex[1].Position = EndPos;	// D3DXVECTOR3(SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT - 100.0f, 0.0f);
//	vertex[1].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
//
//	////二本目の直線の頂点をセット
//	//vertex[2].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, 0.0f + 50.0f, 0.0f);
//	//vertex[2].Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
//	//vertex[2].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
//	//vertex[3].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50.0f, 0.0f);
//	//vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
//	//vertex[3].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
//
//	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
//}

//=================================================
//三角形用頂点格納処理
//
//in
//	表示座標X, 表示座標Y, 横サイズ, 縦サイズ, 回転角度, カラー
//=================================================
//void SetVertexTringle(float posx, float posy, int size_w, int size_h, float kakudo, D3DXCOLOR col)
//{
//	D3D11_MAPPED_SUBRESOURCE msr;
//	//頂点バッファへの書き込み許可をもらう
//	GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//	//頂点構造体の型にポインター型を変換をする
//	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
//
//	static	float ofset = 0.0f;
//	//頂点 V0をセット
//	vertex[0].Position = D3DXVECTOR3(-size_w/2, -size_h/2, 0.0f);
//	vertex[0].Diffuse = col;	// D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
//	vertex[0].TexCoord = D3DXVECTOR2(0.0f+ofset, 0.0f);
//	//頂点 V1をセット
//	vertex[1].Position = D3DXVECTOR3(+size_w/2, -size_h/2, 0.0f);
//	vertex[1].Diffuse = col;	// D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
//	vertex[1].TexCoord = D3DXVECTOR2(1.0f+ofset, 0.0f);
//	//頂点 V2をセット
//	vertex[2].Position = D3DXVECTOR3(-size_w/2, +size_h/2,0.0f);
//	vertex[2].Diffuse = col;	// D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
//	vertex[2].TexCoord = D3DXVECTOR2(0.0f+ofset, 1.0f);
//	//頂点 V3をセット
//	vertex[3].Position = D3DXVECTOR3(+size_w/2, +size_h/2, 0.0f);
//	vertex[3].Diffuse = col;	// D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
//	vertex[3].TexCoord = D3DXVECTOR2(1.0f+ofset, 1.0f);
//
//
//	//UVアニメーション処理ををここに作る
//
//	//本当は引数で受け取りたい情報
//	static	float	PaternNo = 0;	//最初に一回だけ0が入る、その後はグローバル変数と同じで値が保存される
//	float			uv_w = 0.5f;	//キャラクターの横サイズ(UV座標表現)
//	float			uv_h = 0.5f;	//キャラクターの縦サイズ(UV座標表現)
//	int				NumPatern = 2;	//キャラクターの横の枚数
//	//===========================
//	
//	PaternNo += 0.008f;	//パターン番号を少しずつ増やす
//	if (PaternNo >= 4)//パターン番号が4以上になったら
//	{	//
//		PaternNo -= 4.0f;//パターン番号を0へ戻す
//	}
//	int		no = PaternNo;//整数値へ変換（小数点以下が切り落とされる）
//
//	//左上（LEFT-TOP）のUV座標を計算
//	float	uv_left = (no % NumPatern) * uv_w;	//左上U座標
//	float	uv_top = (no / NumPatern) * uv_h;	//左上V座標
//
//	//テクスチャ座標を書き換える
//	vertex[0].TexCoord = D3DXVECTOR2(uv_left,        uv_top);	//LEFT-TOP
//	vertex[1].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top);	//RIGHT-TOP
//	vertex[2].TexCoord = D3DXVECTOR2(uv_left, uv_top + uv_h);	//LEFT-BOTTOM
//	vertex[3].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top + uv_h);	//RIGHT-BOTTOM
//
//	//度をラジアンへ変換
//	float	rad = D3DXToRadian(kakudo);
//	float	x, y;
//
//	for (int i = 0; i < 4; i++)
//	{
//		x = vertex[i].Position.x * cosf(rad) - vertex[i].Position.y * sinf(rad);
//		y = vertex[i].Position.y * cosf(rad) + vertex[i].Position.x * sinf(rad);
//
//		vertex[i].Position.x = x + posx;
//		vertex[i].Position.y = y + posy;
//	}
//
//	GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);//書き込み終了
//}



