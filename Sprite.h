//=============================
//
//�X�v���C�g�`�揈��
//=============================


#pragma once
#include "main.h"

//========================
//�}�N����`
//========================



//========================
//�v���g�^�C�v�錾
//========================
	//static	float	PaternNo = 0;	//�ŏ��Ɉ�񂾂�0������A���̌�̓O���[�o���ϐ��Ɠ����Œl���ۑ������
	//float			uv_w = 0.5f;	//�L�����N�^�[�̉��T�C�Y(UV���W�\��)
	//float			uv_h = 0.5f;	//�L�����N�^�[�̏c�T�C�Y(UV���W�\��)
	//int				NumPatern = 2;	//�L�����N�^�[�̉��̖���

class Transform;
class DrawData;
class DrawVertexData;

void DrawSpriteColorRotate(float, float, int, int, float, D3DXCOLOR,float,float,float,int);
void DrawCell(Transform* transform, DrawData* data);

void DrawTile(Transform* transform, DrawData* data);
void DrawTilePart(DrawVertexData& drawData, DrawData* data);

void InitSprite();
void UninitSprite();