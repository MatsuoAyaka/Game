//=============================
//
//スプライト描画処理
//=============================


#pragma once
#include "main.h"

//========================
//マクロ定義
//========================



//========================
//プロトタイプ宣言
//========================
	//static	float	PaternNo = 0;	//最初に一回だけ0が入る、その後はグローバル変数と同じで値が保存される
	//float			uv_w = 0.5f;	//キャラクターの横サイズ(UV座標表現)
	//float			uv_h = 0.5f;	//キャラクターの縦サイズ(UV座標表現)
	//int				NumPatern = 2;	//キャラクターの横の枚数

class Transform;
class DrawData;
class DrawVertexData;

void DrawSpriteColorRotate(float, float, int, int, float, D3DXCOLOR,float,float,float,int);
void DrawCell(Transform* transform, DrawData* data);

void DrawTile(Transform* transform, DrawData* data);
void DrawTilePart(DrawVertexData& drawData, DrawData* data);

void InitSprite();
void UninitSprite();