
#include	"LineCircle.h"

//プロトタイプ宣言
void SetVertexCircle(D3DXVECTOR3, float, int, float, D3DXCOLOR);	//多角形を表示する

//グローバル変数
static	ID3D11Buffer		*g_VertexBuffer = NULL;		// 頂点情報

//================================
//
//=================================
void	InitLineCircle()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 65;//頂点1個＊頂点数のサイズで作る
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);
	// 頂点バッファ更新
	D3DXVECTOR3	position = D3DXVECTOR3(100.0f, 200.0f, 0.0f);
	D3DXCOLOR	col = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
	SetVertexCircle(position, 100.0f,3, 90.0f, col);

}

//================================
//
//=================================
void	UninitLineCircle()
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
void	DrawLineCircle()
{
	// 表示したいポリゴンの頂点バッファを設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	static	float	rot = 0;

	//表示処理
	D3DXVECTOR3	position = D3DXVECTOR3(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f);



	D3DXCOLOR	col = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
	SetVertexCircle(position, 200.0f, 3, 90.0f + (rot*1.5), col);
	// ポリゴン描画
	GetDeviceContext()->Draw(3 + 1, 0);

	col = D3DXCOLOR(0.0f, 1.0f, 0.4f, 1.0f);
	SetVertexCircle(position, 200.0f, 3, 90.0f+180.0f+(rot*1.5), col);
	// ポリゴン描画
	GetDeviceContext()->Draw(3 + 1, 0);

	col = D3DXCOLOR(1.0f, 1.0f, 0.6f, 1.0f);
	SetVertexCircle(position, 200.0f, 10, 0.0f - rot, col);
	// ポリゴン描画
	GetDeviceContext()->Draw(10 + 1, 0);

	col = D3DXCOLOR(0.0f, 0.7f, 1.0f, 1.0f);
	SetVertexCircle(position, 200.0f, 10, 10.0f - rot, col);
	// ポリゴン描画
	GetDeviceContext()->Draw(10 + 1, 0);

	col = D3DXCOLOR(1.0f, 1.0f, 0.2f, 1.0f);
	SetVertexCircle(position, 200.0f, 10, 5.0f - rot, col);
	// ポリゴン描画
	GetDeviceContext()->Draw(10 + 1, 0);

	col = D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f);
	SetVertexCircle(position, 200.0f + 10.0f, 32, 0.0f + rot, col);
	// ポリゴン描画
	GetDeviceContext()->Draw(32 + 1, 0);

	col = D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f);
	SetVertexCircle(position, 200.0f + 15.0f, 32, 0.0f + rot, col);
	// ポリゴン描画
	GetDeviceContext()->Draw(32 + 1, 0);

	rot += 1.0f;

}

//================================
//
//=================================
void	UpdateLineCircle()
{

}


//=============================================================================
// 頂点データ設定
//
//in
// 開始座標, 半径, 頂点数,	回転角度, 色
//
//=============================================================================
void SetVertexCircle(D3DXVECTOR3 Pos, float  Rad, int NumVertex, float rot, D3DXCOLOR col)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	//頂点バッファへの書き込み許可をもらう
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	float	kakudo = 0;
	for (int i = 0; i < NumVertex; i++)
	{
		//一本目の直線の頂点をセット
		vertex[i].Position.x = cosf(D3DXToRadian(kakudo)) * ( Rad * 2.0f) ;
		vertex[i].Position.y = sinf(D3DXToRadian(kakudo)) * Rad;
		vertex[i].Position.z = 0;
		vertex[i].Diffuse = col;//D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
		vertex[i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		kakudo += (360.0f / NumVertex);
	}
	//度をラジアンへ変換
	float	rad = D3DXToRadian(rot);
	float	x, y;
	for (int i = 0; i < NumVertex; i++)
	{
		x = vertex[i].Position.x * cosf(rad) - vertex[i].Position.y * sinf(rad);
		y = vertex[i].Position.y * cosf(rad) + vertex[i].Position.x * sinf(rad);
		vertex[i].Position.x = x + Pos.x;
		vertex[i].Position.y = y + Pos.y;
	}

	//最後の頂点は先頭の頂点のコピーになる
	vertex[NumVertex] = vertex[0];

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

void DrawCircle(D3DXVECTOR3 pos, float radius, D3DXCOLOR col)
{
	SetWorldViewProjection2D();
	//GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(1));
	D3D11_MAPPED_SUBRESOURCE msr;
	//頂点バッファへの書き込み許可をもらう
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	//頂点構造体の型にポインター型を変換をする
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	for (int i = 0; i < NUM_CIRCLEVERTEX; i++)
	{
		vertex[i].Position = D3DXVECTOR3(pos.x + radius * -sinf(i * 360.0f / NUM_CIRCLEVERTEX), pos.y + radius * -cosf(i * 360.0f / NUM_CIRCLEVERTEX), pos.z + 0.0f);
		vertex[i].Diffuse = col;
		vertex[i].TexCoord = D3DXVECTOR2((i % 2 + 1) * 1.0f, ((i + 1) % 2 + 1) * 1.0f);//(i % 2 + 1) * 1.0f, (i % 2 + 1) * 1.0f
	}
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);//書き込み終了
	// 表示したいポリゴンの頂点バッファを設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	// ポリゴン描画
	GetDeviceContext()->Draw(NUM_CIRCLEVERTEX, 0);
}







