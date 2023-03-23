
#include	"Line.h"

//プロトタイプ宣言
void SetVertex(D3DXVECTOR3, D3DXVECTOR3, D3DXCOLOR);	//直線を表示する

//グローバル変数
static	ID3D11Buffer		*g_VertexBuffer = NULL;		// 頂点情報

//================================
//
//=================================
void	InitLine()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 2;//頂点1個＊頂点数のサイズで作る
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);
	// 頂点バッファ更新
	D3DXVECTOR3	st = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3	ed = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	D3DXCOLOR	col = D3DXCOLOR(1, 1, 1, 1);
	SetVertex(st, ed, col);
}

//================================
//
//=================================
void	UninitLine()
{
	// 頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();//使い終わったので解放する
		g_VertexBuffer = NULL;
	}

}

//================================
//
//=================================
void	DrawLine()
{
	// 表示したいポリゴンの頂点バッファを設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//表示処理
	static	float startposX = 0;
	static	float startposY = 0;
	D3DXVECTOR3		st = D3DXVECTOR3(startposX, startposY, 0.0f);
	D3DXVECTOR3		ed = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	D3DXCOLOR	col = D3DXCOLOR(1,1,1,1);
	SetVertex(st, ed, col);

	//startposX = cosf(D3DXToRadian(Kakudo)) * 100.0f;//開始座標を適当に変更する
	//startposY = sinf(D3DXToRadian(Kakudo)) * 100.0f;

	// ポリゴン描画
	GetDeviceContext()->Draw(2, 0);

}

//================================
//
//=================================
void	UpdateLine()
{

}


//=============================================================================
// 頂点データ設定
//
//in
// 開始座標, 終了座, カラー
//
//=============================================================================
void SetVertex(D3DXVECTOR3 StartPos, D3DXVECTOR3 EndPos, D3DXCOLOR col)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	//頂点バッファへの書き込み許可をもらう
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	//一本目の直線の頂点をセット
	vertex[0].Position = StartPos;	// D3DXVECTOR3(100.0f, 100.0f, 0.0f);
	vertex[0].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	vertex[1].Position = EndPos;	// D3DXVECTOR3(SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT - 100.0f, 0.0f);
	vertex[1].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	////二本目の直線の頂点をセット
	//vertex[2].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, 0.0f + 50.0f, 0.0f);
	//vertex[2].Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
	//vertex[2].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	//vertex[3].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50.0f, 0.0f);
	//vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	//vertex[3].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}