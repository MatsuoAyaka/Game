#pragma once
#include "main.h"
#include "renderer.h"

#define EXPLOSION_MAX 5//爆発最大数
#define EXPLOSION_SPEED 10 //移動速度
#define EXPLOSION_SIZE_W 50//爆発横サイズ
#define EXPLOSION_SIZE_H 50//爆発縦サイズ
#define EXPLOSION_ANIME_MAX 7//アニメーション最大数
#define EXPLOSION_ANIME_SPEED 1.0f
struct EXPLOSION {
	bool use;//使用中フラグ
	float w, h;//サイズ
	D3DXVECTOR2 pos;//表示座標
	float rot;//回転角度
	D3DXVECTOR2 move;//移動スピード
	int texNo;//テクスチャ番号
	float AnimePattern;//アニメーションのパータン番号
	float AnimeSpeed;//アニメーションスピード
};

HRESULT InitExplosion();
void UninitExplosion();
void UpdateExplosion();
void DrawExplosion();

EXPLOSION* GetExplosion();//構造体の先頭のポインタを取得
void SetExplosion(D3DXVECTOR2 pos,float animSpeed);//爆発の発生