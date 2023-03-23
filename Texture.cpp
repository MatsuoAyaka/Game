#include "main.h"
#include "Texture.h"

//�}�N����`
#define MAX_TEXTURE_NUM 100	//���[�h���E��

//�O���[�o���ϐ�
//�e�N�X�`�����̃|�C���^�̔z��
static ID3D11ShaderResourceView* g_pTexture[MAX_TEXTURE_NUM];

//���[�h�ς݂̃e�N�X�`���̐�
static UINT g_TextureCount;

//�e�N�X�`���t�@�C�����̃o�b�t�@ �t�@�C����255�����܂�
static char g_TextureName[MAX_TEXTURE_NUM][256];

//�w��̃e�N�X�`�������擾
//IN �e�N�X�`���n���h��
//OUT �e�N�X�`�����̃|�C���^�̓������|�C���^�ϐ��̃|�C���^
ID3D11ShaderResourceView** GetTexture(int index) {
	//�G���[�`�F�b�N
	if (index < 0) {
		return NULL;//0����
	}

	if (index >= g_TextureCount) {
		return NULL;//���[�h���I�[�o�[
	}


	//�C���f�b�N�X�������ꏊ�̔z��̃|�C���^��Ԃ�
	return &g_pTexture[index];


}

//�e�N�X�`���̏I������
//���[�h�ς݂̃e�N�X�`�������ׂĉ��
void UninitTexture() {

	for (int index = 0; index < g_TextureCount; index++) {
		if (g_pTexture[index] != NULL) {//�����f�[�^�������Ă�����
			g_pTexture[index]->Release();//�������
			g_pTexture[index] = NULL;//���������Ă���
		}
	}

}

int LoadTexture(char* filename) {
	//���łɓǂݍ��܂ꂽ�e�N�X�`�����𒲂ׂē����̃e�N�X�`���������݂����ꍇ�́A�e�N�X�`������
	//�i�[���ꂽ�z��̔ԍ���Ԃ�
	for (int index = 0; index < g_TextureCount; index++) {
		//�e�N�X�`�������r
		if (strcmp(g_TextureName[index], filename) == 0) {
			// if(strcmp(&g_TextureName[index][0] , filename) ==0)
			return index;
		}
	}

	//�ǂݍ��ݐ������E�𒴂��Ă��Ȃ���
	if (g_TextureCount ==  MAX_TEXTURE_NUM) {
		return -1;//�ǂݍ��߂Ȃ��̂ŃG���[�R�[�h��Ԃ�
	}

	//�e�N�X�`���̃��[�h
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		GetDevice(),
		filename,
		NULL,
		NULL,
		&g_pTexture[g_TextureCount],
		NULL
	);

	if (hr != S_OK) {
		return -1;//�ǂݍ��ݎ��s
	}
	//���[�h�����e�N�X�`�����̓o�^
	strcpy_s(g_TextureName[g_TextureCount],256,filename);


	int result = g_TextureCount;//���݂̐����ꎞ�ޔ�
	g_TextureCount++;//���݂̐��𑝂₵�Ă���

	return result;//�ޔ����Ă����l��Ԃ�
}