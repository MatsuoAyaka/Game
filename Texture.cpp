#include "main.h"
#include "Texture.h"

//マクロ定義
#define MAX_TEXTURE_NUM 100	//ロード限界数

//グローバル変数
//テクスチャ情報のポインタの配列
static ID3D11ShaderResourceView* g_pTexture[MAX_TEXTURE_NUM];

//ロード済みのテクスチャの数
static UINT g_TextureCount;

//テクスチャファイル名のバッファ ファイル名255文字まで
static char g_TextureName[MAX_TEXTURE_NUM][256];

//指定のテクスチャ情報を取得
//IN テクスチャハンドル
//OUT テクスチャ情報のポインタの入ったポインタ変数のポインタ
ID3D11ShaderResourceView** GetTexture(int index) {
	//エラーチェック
	if (index < 0) {
		return NULL;//0未満
	}

	if (index >= g_TextureCount) {
		return NULL;//ロード数オーバー
	}


	//インデックスが示す場所の配列のポインタを返す
	return &g_pTexture[index];


}

//テクスチャの終了処理
//ロード済みのテクスチャをすべて解放
void UninitTexture() {

	for (int index = 0; index < g_TextureCount; index++) {
		if (g_pTexture[index] != NULL) {//何かデータが入っていたら
			g_pTexture[index]->Release();//解放する
			g_pTexture[index] = NULL;//初期化しておく
		}
	}

}

int LoadTexture(char* filename) {
	//すでに読み込まれたテクスチャ名を調べて同名のテクスチャ名が存在した場合は、テクスチャ名の
	//格納された配列の番号を返す
	for (int index = 0; index < g_TextureCount; index++) {
		//テクスチャ名を比較
		if (strcmp(g_TextureName[index], filename) == 0) {
			// if(strcmp(&g_TextureName[index][0] , filename) ==0)
			return index;
		}
	}

	//読み込み数が限界を超えていないか
	if (g_TextureCount ==  MAX_TEXTURE_NUM) {
		return -1;//読み込めないのでエラーコードを返す
	}

	//テクスチャのロード
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		GetDevice(),
		filename,
		NULL,
		NULL,
		&g_pTexture[g_TextureCount],
		NULL
	);

	if (hr != S_OK) {
		return -1;//読み込み失敗
	}
	//ロードしたテクスチャ名の登録
	strcpy_s(g_TextureName[g_TextureCount],256,filename);


	int result = g_TextureCount;//現在の数を一時退避
	g_TextureCount++;//現在の数を増やしておく

	return result;//退避していた値を返す
}