#pragma once
#include "Title.h"
#include "main.h"
#include "Texture.h"


static	ID3D11ShaderResourceView* g_TitleTexture = NULL;	//�e�N�X�`�����@�摜1���ɂ��ϐ�1�K�v
static	char* g_TitleTextureName = (char*)"data\\texture\\snow_template1.jpg";//�e�N�X�`���t�@�C���p�X

typedef struct {
	D3DXVECTOR3 Position;//�\�����W
	D3DXVECTOR2 Size;//�T�C�Y
	D3DXCOLOR Color;//�F
	float Rotate;//�p�x

}TITLE;

TITLE titleObject;

int TitleTextureNo;

void InitTitle() {
	TitleTextureNo = LoadTexture(g_TitleTextureName);
	if (TitleTextureNo == -1) {
		exit(999);
	}

	titleObject.Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	titleObject.Rotate = 0.0f;
	titleObject.Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	titleObject.Size = D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT);


}

void UninitTitle() {

}

void UpdateTitle() {

}

void DrawTitle() {

	SetWorldViewProjection2D();

	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TitleTextureNo));
	static float patern = 0;//�e�X�g�p
	DrawSpriteColorRotate(
		titleObject.Position.x,
		titleObject.Position.y,
		titleObject.Size.x,
		titleObject.Size.y,
		titleObject.Rotate,
		titleObject.Color,
		patern, 1.0f, 1.0f, 1
	);
}