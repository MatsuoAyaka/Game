/*==============================================================================

   レンダリング管理[renderer.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once
#include "main.h"

//*********************************************************
// マクロ定義
//*********************************************************
enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)

	CULL_MODE_NUM
};

enum ADDRESS_MODE
{
	ADDRESS_MODE_WRAP,
	ADDRESS_MODE_MIRROR,
	ADDRESS_MODE_CLAMP,
	ADDRESS_MODE_BORDER,

	ADDRESS_MODE_NUM
};

enum FILTER_MODE
{
	FILTER_MODE_POINT,
	FILTER_MODE_LINEAR,
	FILTER_MODE_ANISOTROPIC,

	FILTER_MODE_NUM
};

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    D3DXCOLOR   Diffuse;
    D3DXVECTOR2 TexCoord;
};


// マテリアル構造体
struct MATERIAL
{
	D3DXCOLOR	Ambient;
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Specular;
	D3DXCOLOR	Emission;
	float		Shininess;
	float		Dummy[3];//16byte境界用
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( bool Enable );
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetSamplerState(FILTER_MODE fm, ADDRESS_MODE am);
void SetSamplerBorderColor(D3DXCOLOR col);

void SetWorldViewProjection2D( void );
void SetWorldMatrix( D3DXMATRIX *WorldMatrix );
void SetViewMatrix( D3DXMATRIX *ViewMatrix );
void SetProjectionMatrix( D3DXMATRIX *ProjectionMatrix );


#pragma region self

enum E_BatchingType {
	BatchingType_None,
};

class BaseShaderParam {

};


#pragma region draw_data

enum E_DrawDataType {
	DrawData_Normal,
	DrawData_Tile,
};

enum E_TileType {
	TileType_Repeat,
	TileType_Stretch,
};

struct TileData {

	E_TileType borderTileType;
	E_TileType centerTileType;

	vector2s cornerScale_1;
	vector2s cornerScale_2;
	vector2s cornerSize_1;
	vector2s cornerSize_2;

	vector2s texRawSize;

	//border
	unsigned int topBorderTex;
	vector2s topBorderTexSize;
	unsigned int bottomBorderTex;
	vector2s bottomBorderTexSize;
	unsigned int leftBorderTex;
	vector2s leftBorderTexSize;
	unsigned int rightBorderTex;
	vector2s rightBorderTexSize;
	unsigned int centerTex;
	vector2s centerTexSize;

	vector2s lr_repeatCount;
	vector2s tb_repeatCount;
	vector2s lr_texScale;
	vector2s tb_texScale;
	vector2s center_texScale;
	TileData() :borderTileType(E_TileType::TileType_Stretch), centerTileType(E_TileType::TileType_Stretch), cornerScale_1(vector2s(1, 1)), cornerScale_2(vector2s(1, 1)),
		cornerSize_1(vector2s()), cornerSize_2(vector2s()), texRawSize(vector2s()),
		topBorderTex(-1), bottomBorderTex(-1), leftBorderTex(-1), rightBorderTex(-1), centerTex(-1),
		topBorderTexSize(vector2s(1, 1)), bottomBorderTexSize(vector2s(1, 1)), leftBorderTexSize(vector2s(1, 1)), rightBorderTexSize(vector2s(1, 1)), centerTexSize(vector2s(1, 1)),
		lr_repeatCount(vector2s(1, 1)), tb_repeatCount(vector2s(1, 1)),
		lr_texScale(vector2s(1, 1)), tb_texScale(vector2s(1, 1)), center_texScale(vector2s(1.0f,1.0f))
	{

	}

	TileData(const TileData& src) {
		this->borderTileType = src.borderTileType;
		this->centerTileType = src.centerTileType;
		this->cornerScale_1 = src.cornerScale_1;
		this->cornerScale_2 = src.cornerScale_2;
		this->cornerSize_1 = src.cornerSize_1;
		this->cornerSize_2 = src.cornerSize_2;
		this->texRawSize = src.texRawSize;

		this->topBorderTex = src.topBorderTex;
		this->bottomBorderTex = src.bottomBorderTex;
		this->leftBorderTex = src.leftBorderTex;
		this->rightBorderTex = src.rightBorderTex;
		this->centerTex = src.centerTex;
		this->topBorderTexSize = src.topBorderTexSize;
		this->bottomBorderTexSize = src.bottomBorderTexSize;
		this->leftBorderTexSize = src.leftBorderTexSize;
		this->rightBorderTexSize = src.rightBorderTexSize;
		this->centerTexSize = src.centerTexSize;

		this->lr_repeatCount = src.lr_repeatCount;
		this->tb_repeatCount = src.tb_repeatCount;

		this->lr_texScale = src.lr_texScale;
		this->tb_texScale = src.tb_texScale;
	}

	vector2s GetCenterTexSize() {
		float x = texRawSize.x - cornerSize_1.x - cornerSize_2.x;
		float y = texRawSize.y - cornerSize_1.y - cornerSize_2.y;
		if (x < 0 || y < 0) {
			return vector2s(0, 0);
		}
		return vector2s(x, y);
	}

	vector2s GetTexSize_Inverse() {
		if (texRawSize.x <= 0.000001f || texRawSize.y <= 0.000001f) {
			return vector2s(1.0f, 1.0f);
		}

		return vector2s(1.0f / texRawSize.x, 1.0f / texRawSize.y);
	}

	~TileData() {

	}
};

struct DrawVertexData {
	VERTEX_3D* vertices;

	bool useDefaultTexture;
	unsigned int curTexNo;
	vector2s tl;
	vector2s tr;
	vector2s bl;
	vector2s br;
	vector2s tl_uv;
	vector2s tr_uv;
	vector2s bl_uv;
	vector2s br_uv;

	float z;

	DrawVertexData() :useDefaultTexture(true), curTexNo(-1), tl(vector2s()), tr(vector2s()), bl(vector2s()), br(vector2s()), tl_uv(vector2s()), tr_uv(vector2s()), bl_uv(vector2s()), br_uv(vector2s()), z(0.0f) {
		vertices = NULL;
	}

	void AdjustTexCoord( ) {
		tl_uv.y = F_Clamp(1.0f - tl_uv.y, 0.0f, 1.0f);
		tr_uv.y = F_Clamp(1.0f - tr_uv.y, 0.0f, 1.0f);
		bl_uv.y = F_Clamp(1.0f - bl_uv.y, 0.0f, 1.0f);
		br_uv.y = F_Clamp(1.0f - br_uv.y, 0.0f, 1.0f);
		//tl_uv.y =- tl_uv.y;
		//tr_uv.y =- tr_uv.y;
		//bl_uv.y =- bl_uv.y;
		//br_uv.y =- br_uv.y;
	}

	~DrawVertexData() {
		vertices = NULL;
	}
};
#pragma region draw_data


class DrawData {
protected:


	E_DrawDataType drawDataType;
	TileData* tileData;

	BaseShaderParam* shaderParam;//シェーダーデータ（未完成）

	vector2s size;//描画サイズ
	D3DXCOLOR color;//カラー
	int textureNo;//テクスチャ―

	int drawVertexIndex;//バッチ処理変数（未完成）

public:
	//テクスチャ―マッピングパラメータ
	vector2s texIndex;//UV座標
	vector2s tex_texel_size;//テクスチャ―図案のサイズ　(1.0f/テクスチャ―の横のサイズ　, 1.0f/テクスチャ―の縦のサイズ）
	int tex_w_count;//テクスチャ―の横の図案の数
	bool usePreComputeUV;//あらかじめ決められたUV座標を使うかどうか（座標の値が固定され、アニメーションが使えない）
	vector2s preComputeUV;//図案のUV座標の始点
	vector2s preComputeUVSize;//図案のサイズ　‐　終点=(preComputeUV.x + preComputeUVSize.x, preComponentUV.y + preComponentUVSize.y)

public:
	E_BatchingType batchingType;//バッチ処理タイプ（未完成）
	bool useBatching;//バッチ処理を使うかどうか（未完成）
	bool isUseEbo;
	bool useDefaultVertices;//TODO
public:
	bool shakeWithCamera;//カメラ揺れの時、ゲーム画面と一緒に揺れるかどうか
public:

	DrawData();

	DrawData(const DrawData& src);

	~DrawData();

	void SetDrawVertexIndex(int index);
	int GetDrawVertexIndex();


	void SetSize(vector2s s) {
		size = s;
	}

	vector2s GetSize() {
		return size;
	}

	//shader praram
	void SetShaderParam(BaseShaderParam* param);

	BaseShaderParam* GetShaderParam() {
		return shaderParam;
	}

	void SetColor(D3DXCOLOR c) {
		color = c;
	}

	D3DXCOLOR GetColor() {
		return color;
	}

	void SetTextureNo(int texNo) {
		textureNo = texNo;
	}
	int GetTextureNo() {
		return textureNo;
	}

	void SetShaderIntParam(int index, int intValue) {
		if (index == 1) {
			//tex x index;
			texIndex.x = intValue;
		}
		else if (index == 2) {
			//tex y index
			texIndex.y = intValue;
		}
		else if (index == 3) {
			//texCountIndex;
			texIndex.x = intValue % tex_w_count;
			texIndex.y = intValue / tex_w_count;
		}
	}

	void SetShaderFloatParam(int index, float floatValue) {
		if (shaderParam == NULL)return;
		//shaderParam->SetFloatParam(index, floatValue);
	}

	void SetShaderVecTParam(int index, vector3s vecTParam) {
		if (shaderParam == NULL)return;
		//shaderParam->SetVecTParam(index, vecTParam);
	}

	void SetShaderVecDParam(int index, vector3s vecDParam) {
		if (shaderParam == NULL)return;
		//shaderParam->SetVecDParam(index, vecDParam);
	}



	//draw tile type
	void SetDrawDataType(E_DrawDataType dataType);
	E_DrawDataType GetDrawDataType();
	void SetTileData(TileData* data);
	TileData* GetTileData();
};

#pragma endregion self