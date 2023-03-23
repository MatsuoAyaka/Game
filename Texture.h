#pragma once
#include "renderer.h"

//テクスチャのロード
//IN テクスチャファイル名の仙頭ポンタ
//
int LoadTexture(char* filename);

//テクスチャの終了処理
//ロード済みのテクスチャをすべて解放
void UninitTexture();


//指定のテクスチャ情報を取得
//IN テクスチャハンドル
ID3D11ShaderResourceView** GetTexture(int index);
