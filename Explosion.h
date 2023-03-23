#pragma once
#include "main.h"
#include "renderer.h"

#define EXPLOSION_MAX 5//�����ő吔
#define EXPLOSION_SPEED 10 //�ړ����x
#define EXPLOSION_SIZE_W 50//�������T�C�Y
#define EXPLOSION_SIZE_H 50//�����c�T�C�Y
#define EXPLOSION_ANIME_MAX 7//�A�j���[�V�����ő吔
#define EXPLOSION_ANIME_SPEED 1.0f
struct EXPLOSION {
	bool use;//�g�p���t���O
	float w, h;//�T�C�Y
	D3DXVECTOR2 pos;//�\�����W
	float rot;//��]�p�x
	D3DXVECTOR2 move;//�ړ��X�s�[�h
	int texNo;//�e�N�X�`���ԍ�
	float AnimePattern;//�A�j���[�V�����̃p�[�^���ԍ�
	float AnimeSpeed;//�A�j���[�V�����X�s�[�h
};

HRESULT InitExplosion();
void UninitExplosion();
void UpdateExplosion();
void DrawExplosion();

EXPLOSION* GetExplosion();//�\���̂̐擪�̃|�C���^���擾
void SetExplosion(D3DXVECTOR2 pos,float animSpeed);//�����̔���