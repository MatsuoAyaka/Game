#pragma once
#include "Game.h"
#include "main.h"
#include "renderer.h"
#include "Sprite.h"

#include "Explosion.h"
#include "Texture.h"
#include "polygon.h"

GameMgr* GameMgr::instance = NULL;//�V���O���g���ϐ�������������
GameMgr::Deletor deletor;

void InitGame() {
	//�Q�[���֘A������
	InitSprite();
	InitPolygon();		//�|���S���̏����������s

	InitExplosion();

	srand(timeGetTime());	//����������

	GameMgr::GetInstance()->DoInit();//�Q�[��������������

}

void UninitGame() {
	UninitSprite();
	UninitExplosion();
	// �����_�����O�̏I������
	UninitRenderer();

	GameMgr::GetInstance()->DoUninit();//�Q�[���̏I������
}

void UpdateGame() {
	UpdatePolygon();	//�|���S���̍X�V����
	UpdateExplosion();

	GameMgr::GetInstance()->DoUpdate(0.016f);//�Q�[���̃A�b�v�f�[�g�i1�t���C���̎������Ԃ͂Ƃ肠����0.016�b�ɐݒ肵�Ă��i��b���Ƃ�60�t���C���j

}

void DrawGame() {
	DrawPolygon();			//�|���S���̕\��
	//DrawExplosion();

	GameMgr::GetInstance()->sceneObjMgr->DoDraw();//�Q�[���̕`�揈��
}


