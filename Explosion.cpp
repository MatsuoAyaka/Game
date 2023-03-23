#include "main.h"
#include "Explosion.h"
#include "texture.h"
#include "Sprite.h"


EXPLOSION g_Explosion[EXPLOSION_MAX];//爆発構造体配列



HRESULT InitExplosion() {

	int texno = LoadTexture((char*)"data\\texture\\explosion000.png");
	//爆発構造体の初期化
	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		cur.use = false;//
		cur.w = EXPLOSION_SIZE_W;
		cur.h = EXPLOSION_SIZE_H;
		cur.pos = D3DXVECTOR2(0.0f, 0.0f);//表示座標
		cur.rot = 0.0f;//回転角度
		cur.texNo = texno;//テクスチャ番号
		cur.move = D3DXVECTOR2(0.0f, -EXPLOSION_SPEED);//移動スピード
		cur.AnimePattern = 0.0f;
		cur.AnimeSpeed = EXPLOSION_ANIME_SPEED;
	}
	return S_OK;
}

void UninitExplosion() {
}

void UpdateExplosion() {
	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		if (cur.use == false) {
			continue;
		}

		//cur.pos += cur.move;

		//画面外チェック
		//if (cur.pos.y + cur.h * 0.5f < SCREEN_LIMIT_UP || cur.pos.y - cur.h * 0.5f > SCREEN_LIMIT_DOWN) {
		//	cur.use = false;//爆発を消滅
		//}

		cur.AnimePattern += cur.AnimeSpeed;
		if (cur.AnimePattern > EXPLOSION_ANIME_MAX) {
			cur.use = false;
		}
	}
}

void DrawExplosion() {

	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		if (cur.use == true) {//構造体が使用中なら処理
			GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(cur.texNo));//テクスチャのセット
			//描画する
			DrawSpriteColorRotate(
				cur.pos.x,
				cur.pos.y,
				cur.w,
				cur.h,
				cur.rot,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				(int)cur.AnimePattern,
				1.0f / 8,
				1.0f / 1,
				8
			);
		}
	}
}


EXPLOSION* GetExplosion() {
	return &g_Explosion[0];
}

void SetExplosion(D3DXVECTOR2 pos,float animSpeed) {
	//構造体を全て調べる
	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		if (cur.use == false) {//未使用の構造体を見つける
			cur.pos = pos;//発生座標
			cur.AnimeSpeed = animSpeed;
			cur.AnimePattern = 0.0f;
			cur.use = true;//使用中
			return;//一つ作ったら終了
		}
	}
}