#pragma once
#include "renderer.h"

//�e�N�X�`���̃��[�h
//IN �e�N�X�`���t�@�C�����̐哪�|���^
//
int LoadTexture(char* filename);

//�e�N�X�`���̏I������
//���[�h�ς݂̃e�N�X�`�������ׂĉ��
void UninitTexture();


//�w��̃e�N�X�`�������擾
//IN �e�N�X�`���n���h��
ID3D11ShaderResourceView** GetTexture(int index);
