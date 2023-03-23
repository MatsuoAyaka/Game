/*==============================================================================

   ���_�Ǘ� [polygon.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include	"polygon.h"
#include	"Line.h"
#include	"LineCircle.h"
#include "Sprite.h"
#include "Texture.h"

#include "input.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define NUM_VERTEX 4
#define	NUM_TRIANGLE_VERTEX	(4)	//�O�p�`�p���_��

#define	BOX_W	(700)	//�l�p�`�̉��̃T�C�Y
#define	BOX_H	(700)	//�l�p�`�̏c�̃T�C�Y

#define RUNNING_MAN_W 700
#define RUNNING_MAN_H 400
#define RUNNING_MAN_W_COUNT 5
#define RUNNING_MAN_H_COUNT 2
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//void SetVertex(D3DXVECTOR3, D3DXVECTOR3, D3DXCOLOR);	//������\������
//void SetVertexTringle(float, float, int, int, float, D3DXCOLOR);	//�O�p�`�p���_�i�[����

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
//static	ID3D11Buffer		*g_VertexBuffer = NULL;		// ���_���
static	ID3D11Buffer		*g_VertexBufferTriangle = NULL;

static	ID3D11ShaderResourceView	*g_Texture = NULL;		//�e�N�X�`�����@�摜1���ɂ��ϐ�1�K�v
static	ID3D11ShaderResourceView	*g_BGTexture = NULL;	//�e�N�X�`�����@�摜1���ɂ��ϐ�1�K�v
static	char	*g_TextureName = (char*)"data\\texture\\runningman003.png";//�e�N�X�`���t�@�C���p�X
static	char	*g_BGTextureName = (char*)"data\\texture\\snow_template1.jpg";//�e�N�X�`���t�@�C���p�X


float	PositionX = SCREEN_WIDTH / 2;//��ʒ������W
float	PositionY = SCREEN_HEIGHT / 2;
float	Kakudo = 0;

D3DXVECTOR3		MoveVec;

//typedef struct {
//	D3DXVECTOR3 Position;//�\�����W
//	D3DXVECTOR2 Size;//�T�C�Y
//	D3DXCOLOR Color;//�F
//	float Rotate;//�p�x
//
//}PLAYER;
//PLAYER mario;
typedef struct {
	D3DXVECTOR3 Position;//�\�����W
	D3DXVECTOR2 Size;//�T�C�Y
	D3DXCOLOR Color;//�F
	float Rotate;//�p�x

}BG;

BG* sampleBG;

//�e�N�X�`���C���f�b�N�X
int TextureNo = 0;
int TextureNoBG = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPolygon(void)
{

	sampleBG = new BG();
	sampleBG->Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	sampleBG->Size = D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT);
	sampleBG->Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//mario.Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	////mario.Size = D3DXVECTOR2(SCREEN_WIDTH/2, BOX_H/2);
	//mario.Size = D3DXVECTOR2(RUNNING_MAN_W / RUNNING_MAN_W_COUNT, RUNNING_MAN_H / RUNNING_MAN_H_COUNT);
	//mario.Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);



	//�\�����W�������_���Ɍ��߂�
	PositionX = rand() % SCREEN_WIDTH;
	PositionY = rand() % SCREEN_HEIGHT;

	D3DXVECTOR3		StartPos = D3DXVECTOR3(PositionX, PositionY, 0);
	D3DXVECTOR3		EndPos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);

	MoveVec = EndPos - StartPos;				//GOAL�ւ̃x�N�g���쐬
	D3DXVec3Normalize(&MoveVec, &MoveVec);		//MoveVec�𐳋K������ �������P�ɂ���		
	MoveVec *= 2.0f;							//�ړ��̒�������Z����


	ID3D11Device *pDevice = GetDevice();


	//{//�O�p�`�p�̒��_����������
	//	// ���_�o�b�t�@����
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.Usage = D3D11_USAGE_DYNAMIC;
	//	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_TRIANGLE_VERTEX;//���_1�����_���̃T�C�Y�ō��
	//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBufferTriangle);
	//	//�O�p�`�p���_�i�[����
	//	D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//	SetVertexTringle(PositionX, PositionY, BOX_W, BOX_H, 0, col);
	//}

	
	////	�e�N�X�`���̃��[�h
	//D3DX11CreateShaderResourceViewFromFile(
	//	pDevice,
	//	g_TextureName,	//�e�N�X�`���摜�̃t�@�C����
	//	NULL,
	//	NULL,
	//	&g_Texture,		//�ǂݍ��񂾃e�N�X�`���̏�񂪓���ϐ��̃|�C���^
	//	NULL
	//);

	////	BG�e�N�X�`���̃��[�h
	//D3DX11CreateShaderResourceViewFromFile(
	//	pDevice,
	//	g_BGTextureName,	//�e�N�X�`���摜�̃t�@�C����
	//	NULL,
	//	NULL,
	//	&g_BGTexture,		//�ǂݍ��񂾃e�N�X�`���̏�񂪓���ϐ��̃|�C���^
	//	NULL
	//);

	//TextureNo = LoadTexture(g_TextureName);
	//if (TextureNo == -1) {
	//	exit(999);//�����I��
	//}
	//TextureNoBG = LoadTexture(g_BGTextureName);
	//if (TextureNoBG == -1) {
	//	exit(999);
	//}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPolygon(void)
{
	//// ���_�o�b�t�@�̉��
	//if (g_VertexBuffer)
	//{
	//	g_VertexBuffer->Release();//�g���I������̂ŉ������
	//	g_VertexBuffer = NULL;
	//}
	//UninitLine();	//�����̏I������





	if (g_VertexBufferTriangle)
	{
		g_VertexBufferTriangle->Release();//�g���I������̂ŉ������
		g_VertexBufferTriangle = NULL;
	}

	//�e�N�X�`���̉��
	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

	//BG�e�N�X�`���̉��
	if (g_BGTexture)
	{
		g_BGTexture->Release();
		g_BGTexture = NULL;
	}

	if (sampleBG != NULL) {
		delete sampleBG;
	}
	//UninitLineCircle();
	UninitTexture();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePolygon(void)
{

	//if (GetKeyboardPress(DIK_UP)) {
	//	mario.Position.y -= 5.0f;
	//}
	//else if (GetKeyboardPress(DIK_DOWN)) {
	//	mario.Position.y += 5.0f;
	//}

	//if (IsMouseLeftTriggered()) {
	//	mario.Position.x = GetMouseX();
	//	mario.Position.y = GetMouseY();
	//}

	//UpdateLine();	//�����̍X�V����

	//UpdateLineCircle();

	//PositionX += MoveVec.x;
	//PositionY += MoveVec.y;
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPolygon(void)
{
	{
		//// �\���������|���S���̒��_�o�b�t�@��ݒ�
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �Q�c�\�������邽�߂̃}�g���N�X��ݒ�
		SetWorldViewProjection2D();

		//// �v���~�e�B�u�g�|���W�ݒ�
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// �}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//SetMaterial(material);

	}
	return;
	{
		//// �\���������|���S���̒��_�o�b�t�@��ݒ�
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);
		//// �v���~�e�B�u�g�|���W�ݒ�
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//�O�p�`�̒��_�f�[�^���i�[
		
		//�e�N�X�`���̃Z�b�g
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_BGTexture);
		//GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNoBG));
		//// �|���S���`��
		//D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//DrawSpriteColorRotate(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0, col);
	}

	{
		//// �\���������|���S���̒��_�o�b�t�@��ݒ�
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);
		//// �v���~�e�B�u�g�|���W�ݒ�
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//�O�p�`�̒��_�f�[�^���i�[
		
		//�e�N�X�`���̃Z�b�g
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
		//GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNo));
		//// �|���S���`��
		//D3DXCOLOR	col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//DrawSpriteColorRotate(PositionX, PositionY, BOX_W / 2, BOX_H / 2, 35, col);
	}


	{
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_BGTexture);
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNoBG));

		DrawSpriteColorRotate(
			sampleBG->Position.x,
			sampleBG->Position.y,
			sampleBG->Size.x,
			sampleBG->Size.y,
			sampleBG->Rotate,
			sampleBG->Color,
			0,1.0f,1.0f,1
		);
	}
	return;
	{
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);
		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TextureNo));
		static float patern = 0;//�e�X�g�p
		//DrawSpriteColorRotate(
		//	mario.Position.x,
		//	mario.Position.y,
		//	mario.Size.x,
		//	mario.Size.y,
		//	mario.Rotate,
		//	mario.Color,
		//	patern,1.0f/5,1.0f/2.0f,5
		//);

		patern += 0.05f;
		if (patern >= 10.0f) {
			patern -= 10.0f;
		}
	}
}




////=============================================================================
//// ���_�f�[�^�ݒ�
////
////in
//// �J�n���W, �I����, �J���[
////
////=============================================================================
//void SetVertex(D3DXVECTOR3 StartPos, D3DXVECTOR3 EndPos, D3DXCOLOR col)
//{
//	D3D11_MAPPED_SUBRESOURCE msr;
//	//���_�o�b�t�@�ւ̏������݋������炤
//	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//
//	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
//
//	//��{�ڂ̒����̒��_���Z�b�g
//	vertex[0].Position = StartPos;	// D3DXVECTOR3(100.0f, 100.0f, 0.0f);
//	vertex[0].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
//	vertex[1].Position = EndPos;	// D3DXVECTOR3(SCREEN_WIDTH - 100.0f, SCREEN_HEIGHT - 100.0f, 0.0f);
//	vertex[1].Diffuse = col;		// D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
//
//	////��{�ڂ̒����̒��_���Z�b�g
//	//vertex[2].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, 0.0f + 50.0f, 0.0f);
//	//vertex[2].Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
//	//vertex[2].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
//	//vertex[3].Position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50.0f, 0.0f);
//	//vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
//	//vertex[3].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
//
//	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
//}

//=================================================
//�O�p�`�p���_�i�[����
//
//in
//	�\�����WX, �\�����WY, ���T�C�Y, �c�T�C�Y, ��]�p�x, �J���[
//=================================================
//void SetVertexTringle(float posx, float posy, int size_w, int size_h, float kakudo, D3DXCOLOR col)
//{
//	D3D11_MAPPED_SUBRESOURCE msr;
//	//���_�o�b�t�@�ւ̏������݋������炤
//	GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//	//���_�\���̂̌^�Ƀ|�C���^�[�^��ϊ�������
//	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
//
//	static	float ofset = 0.0f;
//	//���_ V0���Z�b�g
//	vertex[0].Position = D3DXVECTOR3(-size_w/2, -size_h/2, 0.0f);
//	vertex[0].Diffuse = col;	// D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
//	vertex[0].TexCoord = D3DXVECTOR2(0.0f+ofset, 0.0f);
//	//���_ V1���Z�b�g
//	vertex[1].Position = D3DXVECTOR3(+size_w/2, -size_h/2, 0.0f);
//	vertex[1].Diffuse = col;	// D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
//	vertex[1].TexCoord = D3DXVECTOR2(1.0f+ofset, 0.0f);
//	//���_ V2���Z�b�g
//	vertex[2].Position = D3DXVECTOR3(-size_w/2, +size_h/2,0.0f);
//	vertex[2].Diffuse = col;	// D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
//	vertex[2].TexCoord = D3DXVECTOR2(0.0f+ofset, 1.0f);
//	//���_ V3���Z�b�g
//	vertex[3].Position = D3DXVECTOR3(+size_w/2, +size_h/2, 0.0f);
//	vertex[3].Diffuse = col;	// D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
//	vertex[3].TexCoord = D3DXVECTOR2(1.0f+ofset, 1.0f);
//
//
//	//UV�A�j���[�V�����������������ɍ��
//
//	//�{���͈����Ŏ󂯎�肽�����
//	static	float	PaternNo = 0;	//�ŏ��Ɉ�񂾂�0������A���̌�̓O���[�o���ϐ��Ɠ����Œl���ۑ������
//	float			uv_w = 0.5f;	//�L�����N�^�[�̉��T�C�Y(UV���W�\��)
//	float			uv_h = 0.5f;	//�L�����N�^�[�̏c�T�C�Y(UV���W�\��)
//	int				NumPatern = 2;	//�L�����N�^�[�̉��̖���
//	//===========================
//	
//	PaternNo += 0.008f;	//�p�^�[���ԍ������������₷
//	if (PaternNo >= 4)//�p�^�[���ԍ���4�ȏ�ɂȂ�����
//	{	//
//		PaternNo -= 4.0f;//�p�^�[���ԍ���0�֖߂�
//	}
//	int		no = PaternNo;//�����l�֕ϊ��i�����_�ȉ����؂藎�Ƃ����j
//
//	//����iLEFT-TOP�j��UV���W���v�Z
//	float	uv_left = (no % NumPatern) * uv_w;	//����U���W
//	float	uv_top = (no / NumPatern) * uv_h;	//����V���W
//
//	//�e�N�X�`�����W������������
//	vertex[0].TexCoord = D3DXVECTOR2(uv_left,        uv_top);	//LEFT-TOP
//	vertex[1].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top);	//RIGHT-TOP
//	vertex[2].TexCoord = D3DXVECTOR2(uv_left, uv_top + uv_h);	//LEFT-BOTTOM
//	vertex[3].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top + uv_h);	//RIGHT-BOTTOM
//
//	//�x�����W�A���֕ϊ�
//	float	rad = D3DXToRadian(kakudo);
//	float	x, y;
//
//	for (int i = 0; i < 4; i++)
//	{
//		x = vertex[i].Position.x * cosf(rad) - vertex[i].Position.y * sinf(rad);
//		y = vertex[i].Position.y * cosf(rad) + vertex[i].Position.x * sinf(rad);
//
//		vertex[i].Position.x = x + posx;
//		vertex[i].Position.y = y + posy;
//	}
//
//	GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);//�������ݏI��
//}



