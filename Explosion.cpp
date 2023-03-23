#include "main.h"
#include "Explosion.h"
#include "texture.h"
#include "Sprite.h"


EXPLOSION g_Explosion[EXPLOSION_MAX];//�����\���̔z��



HRESULT InitExplosion() {

	int texno = LoadTexture((char*)"data\\texture\\explosion000.png");
	//�����\���̂̏�����
	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		cur.use = false;//
		cur.w = EXPLOSION_SIZE_W;
		cur.h = EXPLOSION_SIZE_H;
		cur.pos = D3DXVECTOR2(0.0f, 0.0f);//�\�����W
		cur.rot = 0.0f;//��]�p�x
		cur.texNo = texno;//�e�N�X�`���ԍ�
		cur.move = D3DXVECTOR2(0.0f, -EXPLOSION_SPEED);//�ړ��X�s�[�h
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

		//��ʊO�`�F�b�N
		//if (cur.pos.y + cur.h * 0.5f < SCREEN_LIMIT_UP || cur.pos.y - cur.h * 0.5f > SCREEN_LIMIT_DOWN) {
		//	cur.use = false;//����������
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
		if (cur.use == true) {//�\���̂��g�p���Ȃ珈��
			GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(cur.texNo));//�e�N�X�`���̃Z�b�g
			//�`�悷��
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
	//�\���̂�S�Ē��ׂ�
	for (int index = 0; index < EXPLOSION_MAX; index++) {
		EXPLOSION& cur = g_Explosion[index];
		if (cur.use == false) {//���g�p�̍\���̂�������
			cur.pos = pos;//�������W
			cur.AnimeSpeed = animSpeed;
			cur.AnimePattern = 0.0f;
			cur.use = true;//�g�p��
			return;//��������I��
		}
	}
}