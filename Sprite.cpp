#include "main.h"
#include "renderer.h"
#include "Texture.h"
#include "Camera.h"
//========================
//マクロ定義
//========================
#define NUM_VERTEX 4

//========================
//グローバル変数
//========================
static	ID3D11Buffer* g_VertexBufferTriangle = NULL;

void InitSprite() {

	{//三角形用の頂点初期化処理
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;//頂点1個＊頂点数のサイズで作る
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBufferTriangle);
	}
}


void UninitSprite() {
	if (g_VertexBufferTriangle != NULL) {
		g_VertexBufferTriangle->Release();
		g_VertexBufferTriangle = NULL;
	}
}

// IN
//表示座標X　表示座標Y　横サイズ　縦サイズ　回転角度　カラー
//パターン番号　パターン横サイズ　パターン縦サイズ
//横に並ぶパターンの数
void DrawSpriteColorRotate(float posx, float posy, int size_w, int size_h, float kakudo, D3DXCOLOR col,float PaternNo,float uv_w,float uv_h,int NumPatern)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	//頂点バッファへの書き込み許可をもらう
	GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	//頂点構造体の型にポインター型を変換をする
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	static	float ofset = 0.0f;
	//頂点 V0をセット
	vertex[0].Position = D3DXVECTOR3(-size_w / 2, -size_h / 2, 0.0f);
	vertex[0].Diffuse = col;	// D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	//vertex[0].TexCoord = D3DXVECTOR2(0.0f + ofset, 0.0f);
	//頂点 V1をセット
	vertex[1].Position = D3DXVECTOR3(+size_w / 2, -size_h / 2, 0.0f);
	vertex[1].Diffuse = col;	// D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	//vertex[1].TexCoord = D3DXVECTOR2(1.0f + ofset, 0.0f);
	//頂点 V2をセット
	vertex[2].Position = D3DXVECTOR3(-size_w / 2, +size_h / 2, 0.0f);
	vertex[2].Diffuse = col;	// D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	//vertex[2].TexCoord = D3DXVECTOR2(0.0f + ofset, 1.0f);
	//頂点 V3をセット
	vertex[3].Position = D3DXVECTOR3(+size_w / 2, +size_h / 2, 0.0f);
	vertex[3].Diffuse = col;	// D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	//vertex[3].TexCoord = D3DXVECTOR2(1.0f + ofset, 1.0f);


	//UVアニメーション処理ををここに作る

	//本当は引数で受け取りたい情報
	//static	float	PaternNo = 0;	//最初に一回だけ0が入る、その後はグローバル変数と同じで値が保存される
	//float			uv_w = 0.5f;	//キャラクターの横サイズ(UV座標表現)
	//float			uv_h = 0.5f;	//キャラクターの縦サイズ(UV座標表現)
	//int				NumPatern = 2;	//キャラクターの横の枚数
	//===========================

	//PaternNo += 0.008f;	//パターン番号を少しずつ増やす
	//if (PaternNo >= 4)//パターン番号が4以上になったら
	//{	//
	//	PaternNo -= 4.0f;//パターン番号を0へ戻す
	//}
	int		no = PaternNo;//整数値へ変換（小数点以下が切り落とされる）

	//左上（LEFT-TOP）のUV座標を計算
	float	uv_left = (no % NumPatern) * uv_w;	//左上U座標
	float	uv_top = (no / NumPatern) * uv_h;	//左上V座標

	//テクスチャ座標を書き換える
	vertex[0].TexCoord = D3DXVECTOR2(uv_left, uv_top);	//LEFT-TOP
	vertex[1].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top);	//RIGHT-TOP
	vertex[2].TexCoord = D3DXVECTOR2(uv_left, uv_top + uv_h);	//LEFT-BOTTOM
	vertex[3].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top + uv_h);	//RIGHT-BOTTOM

	//度をラジアンへ変換
	float	rad = D3DXToRadian(kakudo);
	float	x, y;

	for (int i = 0; i < 4; i++)
	{
		x = vertex[i].Position.x * cosf(rad) - vertex[i].Position.y * sinf(rad);
		y = vertex[i].Position.y * cosf(rad) + vertex[i].Position.x * sinf(rad);

		vertex[i].Position.x = x + posx;
		vertex[i].Position.y = y + posy;
	}

	GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);//書き込み終了

		// 表示したいポリゴンの頂点バッファを設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);
	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画
	GetDeviceContext()->Draw(NUM_VERTEX, 0);
}

void DrawCell(Transform* transform, DrawData* data) {
	if (transform == NULL || data == NULL)return;

	if (data->GetDrawDataType() == E_DrawDataType::DrawData_Tile) {
        DrawTile(transform, data);
	}
	else if (data->GetDrawDataType() == E_DrawDataType::DrawData_Normal) {

		GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(data->GetTextureNo()));//テクスチャのセット

		vector3s pos = transform->GetPos();//位置
		vector3s rot = transform->GetRot();//回転
		vector3s scale = transform->GetScale();//スケール
		D3DCOLOR color = data->GetColor();//カラー
		vector2s size = data->GetSize();//元のサイズ

		float halfX = fabs((double)scale.x * (double)size.x * 0.5f);//実際の横の半分のサイズ（スケールに応じて拡大や縮小
		float halfY = fabs((double)scale.y * (double)size.y * 0.5f);//実際の縦の半分のサイズ（スケールに応じて拡大や縮小
		//fabs : float型の値の絶対値を取得する

		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = D3DXVECTOR3(-halfX, -halfY, pos.z);
		vertex[0].Diffuse = color;	// D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		vertex[1].Position = D3DXVECTOR3(halfX, -halfY, pos.z);
		vertex[1].Diffuse = color;	// D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
		vertex[2].Position = D3DXVECTOR3(-halfX, halfY, pos.z);
		vertex[2].Diffuse = color;	// D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		vertex[3].Position = D3DXVECTOR3(halfX, halfY, pos.z);
		vertex[3].Diffuse = color;	// D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);

		//Z軸回りの回転
		if ((int)(rot.z) != 0) {
			float tempX = 0.0f;
			float tempY = 0.0f;
			float cosRotZ = cosf(rot.z * DEG2RAD);
			float sinRotZ = sinf(rot.z * DEG2RAD);
			for (int index = 0; index < 4; index++) {
				tempX = vertex[index].Position.x;
				tempY = vertex[index].Position.y;

				vertex[index].Position.x = (tempX * cosRotZ - tempY * sinRotZ);
				vertex[index].Position.y = (tempX * sinRotZ + tempY * cosRotZ);
			}
		}



		//vertex[0].TexCoord = D3DXVECTOR2(uv_left, uv_top);	//LEFT-TOP
		//vertex[1].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top);	//RIGHT-TOP
		//vertex[2].TexCoord = D3DXVECTOR2(uv_left, uv_top + uv_h);	//LEFT-BOTTOM
		//vertex[3].TexCoord = D3DXVECTOR2(uv_left + uv_w, uv_top + uv_h);	//RIGHT-BOTTOM

		int xRotateFactor = (int)rot.x != 0 ? -1 : 0;//左右反転するかどうか
		int yRotateFactor = (int)rot.y != 0 ? -1 : 0;//上下反転するかどうか

		//テクスチャ―マッピング
		if (data->usePreComputeUV == false) {//UV座標をテクスチャ―パラメータによって計算で取得する場合
			vertex[1].TexCoord = vector2s(data->texIndex.x + 1 + xRotateFactor, data->texIndex.y - yRotateFactor);//1,0 ---- 1
			vertex[0].TexCoord = vector2s(data->texIndex.x - xRotateFactor, data->texIndex.y - yRotateFactor);//0,0 ---- 0
			vertex[3].TexCoord = vector2s(data->texIndex.x + 1 + xRotateFactor, data->texIndex.y + 1 + yRotateFactor);//1,1 ---- 3
			vertex[2].TexCoord = vector2s(data->texIndex.x - xRotateFactor, data->texIndex.y + 1 + yRotateFactor);//0,1 ---- 2
		}
		else {//あらかじめ決められたUV座標を使う場合

			vertex[1].TexCoord = vector2s(data->preComputeUV.x + (data->preComputeUVSize.x * (xRotateFactor + 1)),
				data->preComputeUV.y - (data->preComputeUVSize.y * yRotateFactor));//---- 1
			vertex[0].TexCoord = vector2s(data->preComputeUV.x - (data->preComputeUVSize.x * xRotateFactor),
				data->preComputeUV.y - (data->preComputeUVSize.y * yRotateFactor));//---- 0
			vertex[3].TexCoord = vector2s(data->preComputeUV.x + (data->preComputeUVSize.x * (xRotateFactor + 1)),
				data->preComputeUV.y + (data->preComputeUVSize.y * (yRotateFactor + 1)));//---- 3
			vertex[2].TexCoord = vector2s(data->preComputeUV.x - (data->preComputeUVSize.x * xRotateFactor),
				data->preComputeUV.y + (data->preComputeUVSize.y * (yRotateFactor + 1)));//---- 2
		}

		//mapping to [0,1]
		for (int index = 0; index < 4; index++) {//テクスチャ―UV座標を　0から1までの範囲内にマッピングする（座標を変換する）
			vertex[index].TexCoord.x *= data->tex_texel_size.x;
			vertex[index].TexCoord.y *= data->tex_texel_size.y;
		}

		//vertex[0].TexCoord = D3DXVECTOR2(0.0f - xRotateFactor , 0.0f - yRotateFactor );	//LEFT-TOP
		//vertex[1].TexCoord = D3DXVECTOR2(1.0f + xRotateFactor , 0.0f - yRotateFactor);	//RIGHT-TOP
		//vertex[2].TexCoord = D3DXVECTOR2(0.0f - xRotateFactor ,1.0f + yRotateFactor);	//LEFT-BOTTOM
		//vertex[3].TexCoord = D3DXVECTOR2(1.0f + xRotateFactor, 1.0f + yRotateFactor);	//RIGHT-BOTTOM

		if (GameMgr::GetInstance()->effectMgr->cameraShakeSwitch == true && data->shakeWithCamera == true) {
			pos = pos + GameMgr::GetInstance()->effectMgr->shakePosOffset;//カメラ揺れ
		}

		for (int i = 0; i < 4; i++)
		{
			vertex[i].Position.x += pos.x;
			vertex[i].Position.y += pos.y;
		}
        //追加==================
        //=====カメラ処理=======
        MainCamera* mainCamera = MainCamera::GetInstance();
        vector2s CameraPos = *mainCamera->GetCamPos();

        for (int i = 0; i < 4; i++)
        {
            vertex[i].Position.x -= CameraPos.x;
            vertex[i].Position.y -= CameraPos.y;
        }

        GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);

		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;

		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);

		GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		GetDeviceContext()->Draw(NUM_VERTEX, 0);


	}
}


void DrawTile(Transform* transform, DrawData* data) {
    if (data == NULL || data->GetDrawDataType() != E_DrawDataType::DrawData_Tile)return;


    TileData* tileData = data->GetTileData();
    if (tileData == NULL)return;

    //D3D11_MAPPED_SUBRESOURCE msr;
    //GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

    //VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

    vector3s pos = vector3s(0.0f, 0.0f, 0.0f);
    vector3s rot = pos;//z axis
    vector3s scale = vector3s(1.0f, 1.0f, 1.0f);
    if (transform != NULL) {
        pos = transform->GetPos();
        rot = transform->GetRot();
        scale = transform->GetScale();
    }

    vector2s size = data->GetSize();
    vector2s scaleSize = size * vector2s(scale.x, scale.y);
    float halfX = fabs(scale.x * size.x * 0.5f);
    float halfY = fabs(scale.y * size.y * 0.5f);

    //slice tile
    //corner

    vector2s defaultCornerSize_1 = tileData->cornerSize_1;
    vector2s defaultCornerSize_2 = tileData->cornerSize_2;
    vector2s cornerSize_1 = tileData->cornerSize_1;
    vector2s cornerSize_2 = tileData->cornerSize_2;
    cornerSize_1 = cornerSize_1 * tileData->cornerScale_1;
    cornerSize_2 = cornerSize_2 * tileData->cornerScale_2;

    if ((cornerSize_1.x + cornerSize_2.x) > tileData->texRawSize.x
        || (cornerSize_1.y + cornerSize_2.y) > tileData->texRawSize.y) {
        return;//size error
    }

    vector2s texSize_I = tileData->GetTexSize_Inverse();

    vector2s centerTexSize = tileData->GetCenterTexSize();
    vector2s centerSize = vector2s(size.x - cornerSize_1.x - cornerSize_2.x, size.y - cornerSize_1.y - cornerSize_2.y);

    //LT corner
    vector3s tl_tl = vector3s(pos.x - halfX, pos.y - halfY, pos.z);
    vector3s tl_tr = vector3s(tl_tl.x + cornerSize_1.x, tl_tl.y, pos.z);
    vector3s tl_bl = vector3s(tl_tl.x, tl_tl.y + cornerSize_1.y, pos.z);//origin point
    vector3s tl_br = vector3s(tl_tl.x + cornerSize_1.x, tl_tl.y + cornerSize_1.y, pos.z);

    vector2s tl_bl_uv = vector2s(0.0f, (tileData->texRawSize.y - defaultCornerSize_1.y) * texSize_I.y);//origin point
    vector2s tl_tl_uv = vector2s(0.0f, 1.0f);
    vector2s tl_tr_uv = vector2s(defaultCornerSize_1.x * texSize_I.x, 1.0f);
    vector2s tl_br_uv = vector2s(defaultCornerSize_1.x * texSize_I.x, (tileData->texRawSize.y - defaultCornerSize_1.y) * texSize_I.y);

    //RT corner
    vector3s tr_tr = vector3s(pos.x + halfX, pos.y - halfY, pos.z);
    vector3s tr_tl = vector3s(tr_tr.x - cornerSize_2.x, tr_tr.y, pos.z);
    vector3s tr_bl = vector3s(tr_tr.x - cornerSize_2.x, tr_tr.y + cornerSize_1.y, pos.z);//origin point
    vector3s tr_br = vector3s(tr_tr.x, tr_tr.y + cornerSize_1.y, pos.z);

    vector2s tr_bl_uv = vector2s((tileData->texRawSize.x - defaultCornerSize_2.x) * texSize_I.x,
        (tileData->texRawSize.y - defaultCornerSize_1.y) * texSize_I.y);//origin point
    vector2s tr_tl_uv = vector2s((tileData->texRawSize.x - defaultCornerSize_2.x) * texSize_I.x, 1.0f);
    vector2s tr_tr_uv = vector2s(1.0f, 1.0f);
    vector2s tr_br_uv = vector2s(1.0f, (tileData->texRawSize.y - defaultCornerSize_1.y) * texSize_I.y);



    //LB corner
    vector3s bl_bl = vector3s(pos.x - halfX, pos.y + halfY, pos.z);//origin point
    vector3s bl_tl = vector3s(bl_bl.x, bl_bl.y - cornerSize_2.y, pos.z);
    vector3s bl_tr = vector3s(bl_bl.x + cornerSize_1.x, bl_bl.y - cornerSize_2.y, pos.z);
    vector3s bl_br = vector3s(bl_bl.x + cornerSize_1.x, bl_bl.y, pos.z);

    vector2s bl_bl_uv = vector2s(0.0f, 0.0f);//origin point
    vector2s bl_tl_uv = vector2s(0.0f, defaultCornerSize_2.y * texSize_I.y);
    vector2s bl_tr_uv = vector2s(defaultCornerSize_1.x * texSize_I.x, defaultCornerSize_2.y * texSize_I.y);
    vector2s bl_br_uv = vector2s(defaultCornerSize_1.x * texSize_I.x, 0.0f);



    //RB corner
    vector3s br_br = vector3s(pos.x + halfX, pos.y + halfY, pos.z);
    vector3s br_bl = vector3s(br_br.x - cornerSize_2.x, br_br.y, pos.z);//origin point
    vector3s br_tr = vector3s(br_br.x, br_br.y - cornerSize_2.y, pos.z);
    vector3s br_tl = vector3s(br_br.x - cornerSize_2.x, br_br.y - cornerSize_2.y, pos.z);

    vector2s br_bl_uv = vector2s((tileData->texRawSize.x - defaultCornerSize_2.x) * texSize_I.x, 0.0f);//origin point
    vector2s br_tl_uv = vector2s((tileData->texRawSize.x - defaultCornerSize_2.x) * texSize_I.x,
        defaultCornerSize_2.y * texSize_I.y);
    vector2s br_tr_uv = vector2s(1.0f, defaultCornerSize_2.y * texSize_I.y);
    vector2s br_br_uv = vector2s(1.0f, 0.0f);

    //vertex* vertices = data->useDefaultVertices ? &defaultVertices[0] : data->GetVertex();
    //if (vertices == NULL) {
    //    return;
    //}

    if ((int)(rot.z) != 0) {
        float tempX = 0.0f;
        float tempY = 0.0f;
        float cosRotZ = cosf(rot.z * DEG2RAD);
        float sinRotZ = sinf(rot.z * DEG2RAD);

        //tl corner
        tempX = tl_tl.x;
        tempY = tl_tl.y;
        tl_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tl_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tl_bl.x;
        tempY = tl_bl.y;
        tl_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tl_bl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tl_tr.x;
        tempY = tl_tr.y;
        tl_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tl_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tl_br.x;
        tempY = tl_br.y;
        tl_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tl_bl.y = (tempX * sinRotZ + tempY * cosRotZ);

        //bl corner
        tempX = bl_tl.x;
        tempY = bl_tl.y;
        bl_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        bl_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = bl_bl.x;
        tempY = bl_bl.y;
        bl_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        bl_bl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = bl_tr.x;
        tempY = bl_tr.y;
        bl_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        bl_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = bl_br.x;
        tempY = bl_br.y;
        bl_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        bl_bl.y = (tempX * sinRotZ + tempY * cosRotZ);

        //tr corner
        tempX = tr_tl.x;
        tempY = tr_tl.y;
        tr_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tr_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tr_bl.x;
        tempY = tr_bl.y;
        tr_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tr_bl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tr_tr.x;
        tempY = tr_tr.y;
        tr_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tr_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = tr_br.x;
        tempY = tr_br.y;
        tr_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        tr_bl.y = (tempX * sinRotZ + tempY * cosRotZ);

        //br corner
        tempX = br_tl.x;
        tempY = br_tl.y;
        br_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        br_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = br_bl.x;
        tempY = br_bl.y;
        br_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        br_bl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = br_tr.x;
        tempY = br_tr.y;
        br_tl.x = (tempX * cosRotZ - tempY * sinRotZ);
        br_tl.y = (tempX * sinRotZ + tempY * cosRotZ);
        tempX = br_br.x;
        tempY = br_br.y;
        br_bl.x = (tempX * cosRotZ - tempY * sinRotZ);
        br_bl.y = (tempX * sinRotZ + tempY * cosRotZ);
    }


    DrawVertexData vertexData;
    //vertexData.vertices = vertex;

    //TL
    vertexData.tl = VecTransformTToD(tl_tl);
    vertexData.tr = VecTransformTToD(tl_tr);
    vertexData.bl = VecTransformTToD(tl_bl);
    vertexData.br = VecTransformTToD(tl_br);
    vertexData.tl_uv = tl_tl_uv;
    vertexData.tr_uv = tl_tr_uv;
    vertexData.bl_uv = tl_bl_uv;
    vertexData.br_uv = tl_br_uv;
    vertexData.AdjustTexCoord();
    //vertexData.z = pos.z * screen_size.x;
    vertexData.z = pos.z;
    vertexData.useDefaultTexture = true;
    DrawTilePart(vertexData,data);

    //TR
    vertexData.tl = VecTransformTToD(tr_tl);
    vertexData.tr = VecTransformTToD(tr_tr);
    vertexData.bl = VecTransformTToD(tr_bl);
    vertexData.br = VecTransformTToD(tr_br);
    vertexData.tl_uv = tr_tl_uv;
    vertexData.tr_uv = tr_tr_uv;
    vertexData.bl_uv = tr_bl_uv;
    vertexData.br_uv = tr_br_uv;
    vertexData.AdjustTexCoord();
    vertexData.useDefaultTexture = true;
    DrawTilePart(vertexData,data);

    //BL
    vertexData.tl = VecTransformTToD(bl_tl);
    vertexData.tr = VecTransformTToD(bl_tr);
    vertexData.bl = VecTransformTToD(bl_bl);
    vertexData.br = VecTransformTToD(bl_br);
    vertexData.tl_uv = bl_tl_uv;
    vertexData.tr_uv = bl_tr_uv;
    vertexData.bl_uv = bl_bl_uv;
    vertexData.br_uv = bl_br_uv;
    vertexData.AdjustTexCoord();
    vertexData.useDefaultTexture = true;
    DrawTilePart(vertexData, data);

    //BR
    vertexData.tl = VecTransformTToD(br_tl);
    vertexData.tr = VecTransformTToD(br_tr);
    vertexData.bl = VecTransformTToD(br_bl);
    vertexData.br = VecTransformTToD(br_br);
    vertexData.tl_uv = br_tl_uv;
    vertexData.tr_uv = br_tr_uv;
    vertexData.bl_uv = br_bl_uv;
    vertexData.br_uv = br_br_uv;
    vertexData.AdjustTexCoord();
    vertexData.useDefaultTexture = true;
    DrawTilePart(vertexData, data);

    vector2s topBorderSize = vector2s(scaleSize.x - cornerSize_1.x - cornerSize_2.x, cornerSize_1.y);
    vector2s bottomBorderSize = vector2s(scaleSize.x - cornerSize_1.x - cornerSize_2.x, cornerSize_2.y);
    vector2s leftBorderSize = vector2s(cornerSize_1.x, scaleSize.y - cornerSize_1.y - cornerSize_2.y);
    vector2s rightBorderSize = vector2s(cornerSize_2.x, scaleSize.y - cornerSize_1.y - cornerSize_2.y);

    //TODO:xAxis,yAxis rotate

    if (tileData->borderTileType == E_TileType::TileType_Stretch) {
        //Up Border
        vector3s top_border_tl = tl_tr;
        vector3s top_border_bl = tl_br;
        vector3s top_border_tr = tr_tl;
        vector3s top_border_br = tr_bl;

        vector2s top_border_tl_uv = tl_tr_uv;
        vector2s top_border_bl_uv = tl_br_uv;
        vector2s top_border_tr_uv = tr_tl_uv;
        vector2s top_border_br_uv = tr_bl_uv;
        vertexData.tl = VecTransformTToD(top_border_tl);
        vertexData.tr = VecTransformTToD(top_border_tr);
        vertexData.bl = VecTransformTToD(top_border_bl);
        vertexData.br = VecTransformTToD(top_border_br);
        vertexData.tl_uv = top_border_tl_uv;
        vertexData.tr_uv = top_border_tr_uv;
        vertexData.bl_uv = top_border_bl_uv;
        vertexData.br_uv = top_border_br_uv;
        vertexData.AdjustTexCoord();
        vertexData.useDefaultTexture = true;
        DrawTilePart(vertexData, data);

        //Bottom Border
        vector3s bottom_border_tl = bl_tr;
        vector3s bottom_border_bl = bl_br;
        vector3s bottom_border_tr = br_tl;
        vector3s bottom_border_br = br_bl;

        vector2s bottom_border_tl_uv = bl_tr_uv;
        vector2s bottom_border_bl_uv = bl_br_uv;
        vector2s bottom_border_tr_uv = br_tl_uv;
        vector2s bottom_border_br_uv = br_bl_uv;
        vertexData.tl = VecTransformTToD(bottom_border_tl);
        vertexData.tr = VecTransformTToD(bottom_border_tr);
        vertexData.bl = VecTransformTToD(bottom_border_bl);
        vertexData.br = VecTransformTToD(bottom_border_br);
        vertexData.tl_uv = bottom_border_tl_uv;
        vertexData.tr_uv = bottom_border_tr_uv;
        vertexData.bl_uv = bottom_border_bl_uv;
        vertexData.br_uv = bottom_border_br_uv;
        vertexData.AdjustTexCoord();
        vertexData.useDefaultTexture = true;
        DrawTilePart(vertexData, data);


        //Left Border
        vector3s left_border_tl = tl_bl;
        vector3s left_border_bl = bl_tl;
        vector3s left_border_tr = tl_br;
        vector3s left_border_br = bl_tr;

        vector2s left_border_tl_uv = tl_bl_uv;
        vector2s left_border_bl_uv = bl_tl_uv;
        vector2s left_border_tr_uv = tl_br_uv;
        vector2s left_border_br_uv = bl_tr_uv;
        vertexData.tl = VecTransformTToD(left_border_tl);
        vertexData.tr = VecTransformTToD(left_border_tr);
        vertexData.bl = VecTransformTToD(left_border_bl);
        vertexData.br = VecTransformTToD(left_border_br);
        vertexData.tl_uv = left_border_tl_uv;
        vertexData.tr_uv = left_border_tr_uv;
        vertexData.bl_uv = left_border_bl_uv;
        vertexData.br_uv = left_border_br_uv;
        vertexData.AdjustTexCoord();
        vertexData.useDefaultTexture = true;
        DrawTilePart(vertexData, data);

        //Right Border
        vector3s right_border_tl = tr_bl;
        vector3s right_border_bl = br_tl;
        vector3s right_border_tr = tr_br;
        vector3s right_border_br = br_tr;

        vector2s right_border_tl_uv = tr_bl_uv;
        vector2s right_border_bl_uv = br_tl_uv;
        vector2s right_border_tr_uv = tr_br_uv;
        vector2s right_border_br_uv = br_tr_uv;
        vertexData.tl = VecTransformTToD(right_border_tl);
        vertexData.tr = VecTransformTToD(right_border_tr);
        vertexData.bl = VecTransformTToD(right_border_bl);
        vertexData.br = VecTransformTToD(right_border_br);
        vertexData.tl_uv = right_border_tl_uv;
        vertexData.tr_uv = right_border_tr_uv;
        vertexData.bl_uv = right_border_bl_uv;
        vertexData.br_uv = right_border_br_uv;
        vertexData.AdjustTexCoord();
        vertexData.useDefaultTexture = true;
        DrawTilePart(vertexData, data);

    }
    else if (tileData->borderTileType == E_TileType::TileType_Repeat) {
        //TODO: it will use slice texture




        //Up Border
        vector3s top_border_tl = tl_tr;
        vector3s top_border_bl = tl_br;
        vector3s top_border_tr = tr_tl;
        vector3s top_border_br = tr_bl;
        if (tileData->topBorderTex == -1) {

            vector2s top_border_tl_uv = tl_tr_uv;
            vector2s top_border_bl_uv = tl_br_uv;
            vector2s top_border_tr_uv = tr_tl_uv;
            vector2s top_border_br_uv = tr_bl_uv;
            vertexData.tl = VecTransformTToD(top_border_tl);
            vertexData.tr = VecTransformTToD(top_border_tr);
            vertexData.bl = VecTransformTToD(top_border_bl);
            vertexData.br = VecTransformTToD(top_border_br);
            vertexData.tl_uv = top_border_tl_uv;
            vertexData.tr_uv = top_border_tr_uv;
            vertexData.bl_uv = top_border_bl_uv;
            vertexData.br_uv = top_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = true;
            DrawTilePart(vertexData, data);
        }
        else {
            float repeatCount = tileData->tb_repeatCount.x;
            if (repeatCount <= 0) {
                repeatCount = topBorderSize.x < 0.000001f ? 1.0f : topBorderSize.x / (tileData->topBorderTexSize.x * tileData->tb_texScale.x);
            }
            else {
                repeatCount = 1.0f;
            }

            vector2s top_border_tl_uv = vector2s(0.0f, 1.0f);
            vector2s top_border_bl_uv = vector2s(0.0f, 0.0f);
            vector2s top_border_tr_uv = vector2s(repeatCount, 1.0f);
            vector2s top_border_br_uv = vector2s(repeatCount, 0.0f);
            vertexData.tl = VecTransformTToD(top_border_tl);
            vertexData.tr = VecTransformTToD(top_border_tr);
            vertexData.bl = VecTransformTToD(top_border_bl);
            vertexData.br = VecTransformTToD(top_border_br);
            vertexData.tl_uv = top_border_tl_uv;
            vertexData.tr_uv = top_border_tr_uv;
            vertexData.bl_uv = top_border_bl_uv;
            vertexData.br_uv = top_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = false;
            vertexData.curTexNo = tileData->topBorderTex;
            DrawTilePart(vertexData, data);
        }

        //Bottom Border
        vector3s bottom_border_tl = bl_tr;
        vector3s bottom_border_bl = bl_br;
        vector3s bottom_border_tr = br_tl;
        vector3s bottom_border_br = br_bl;
        if (tileData->bottomBorderTex == -1) {

            vector2s bottom_border_tl_uv = bl_tr_uv;
            vector2s bottom_border_bl_uv = bl_br_uv;
            vector2s bottom_border_tr_uv = br_tl_uv;
            vector2s bottom_border_br_uv = br_bl_uv;
            vertexData.tl = VecTransformTToD(bottom_border_tl);
            vertexData.tr = VecTransformTToD(bottom_border_tr);
            vertexData.bl = VecTransformTToD(bottom_border_bl);
            vertexData.br = VecTransformTToD(bottom_border_br);
            vertexData.tl_uv = bottom_border_tl_uv;          
            vertexData.tr_uv = bottom_border_tr_uv;
            vertexData.bl_uv = bottom_border_bl_uv;
            vertexData.br_uv = bottom_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = true;
            DrawTilePart(vertexData, data);
        }
        else {
            float repeatCount = tileData->tb_repeatCount.y;
            if (repeatCount <= 0) {
                repeatCount = bottomBorderSize.x < 0.000001f ? 1.0f : bottomBorderSize.x / (tileData->bottomBorderTexSize.x * tileData->tb_texScale.y);
            }
            else {
                repeatCount = 1.0f;
            }

            vector2s bottom_border_tl_uv = vector2s(0.0f, 1.0f);
            vector2s bottom_border_bl_uv = vector2s(0.0f, 0.0f);
            vector2s bottom_border_tr_uv = vector2s(repeatCount, 1.0f);
            vector2s bottom_border_br_uv = vector2s(repeatCount, 0.0f);
            vertexData.tl = VecTransformTToD(bottom_border_tl);
            vertexData.tr = VecTransformTToD(bottom_border_tr);
            vertexData.bl = VecTransformTToD(bottom_border_bl);
            vertexData.br = VecTransformTToD(bottom_border_br);
            vertexData.tl_uv = bottom_border_tl_uv;
            vertexData.tr_uv = bottom_border_tr_uv;
            vertexData.bl_uv = bottom_border_bl_uv;
            vertexData.br_uv = bottom_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = false;
            vertexData.curTexNo = tileData->bottomBorderTex;
            DrawTilePart(vertexData, data);
        }

        //Left Border
        vector3s left_border_tl = tl_bl;
        vector3s left_border_bl = bl_tl;
        vector3s left_border_tr = tl_br;
        vector3s left_border_br = bl_tr;
        if (tileData->leftBorderTex == -1) {

            vector2s left_border_tl_uv = tl_bl_uv;
            vector2s left_border_bl_uv = bl_tl_uv;
            vector2s left_border_tr_uv = tl_br_uv;
            vector2s left_border_br_uv = bl_tr_uv;
            vertexData.tl = VecTransformTToD(left_border_tl);
            vertexData.tr = VecTransformTToD(left_border_tr);
            vertexData.bl = VecTransformTToD(left_border_bl);
            vertexData.br = VecTransformTToD(left_border_br);
            vertexData.tl_uv = left_border_tl_uv;
            vertexData.tr_uv = left_border_tr_uv;
            vertexData.bl_uv = left_border_bl_uv;
            vertexData.br_uv = left_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = true;
            DrawTilePart(vertexData, data);
        }
        else {
            float repeatCount = tileData->lr_repeatCount.x;
            if (repeatCount <= 0) {
                repeatCount = leftBorderSize.y < 0.000001f ? 1.0f : leftBorderSize.y / (tileData->leftBorderTexSize.y * tileData->lr_texScale.x);
            }
            else {
                repeatCount = 1.0f;
            }

            vector2s left_border_tl_uv = vector2s(0.0f, repeatCount);
            vector2s left_border_bl_uv = vector2s(0.0f, 0.0f);
            vector2s left_border_tr_uv = vector2s(1.0f, repeatCount);
            vector2s left_border_br_uv = vector2s(1.0f, 0.0f);
            vertexData.tl = VecTransformTToD(left_border_tl);
            vertexData.tr = VecTransformTToD(left_border_tr);
            vertexData.bl = VecTransformTToD(left_border_bl);
            vertexData.br = VecTransformTToD(left_border_br);
            vertexData.tl_uv = left_border_tl_uv;
            vertexData.tr_uv = left_border_tr_uv;
            vertexData.bl_uv = left_border_bl_uv;
            vertexData.br_uv = left_border_br_uv;
            //vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = false;
            vertexData.curTexNo = tileData->leftBorderTex;
            DrawTilePart(vertexData, data);
        }

        //Right Border
        vector3s right_border_tl = tr_bl;
        vector3s right_border_bl = br_tl;
        vector3s right_border_tr = tr_br;
        vector3s right_border_br = br_tr;
        if (tileData->rightBorderTex == -1) {

            vector2s right_border_tl_uv = tr_bl_uv;
            vector2s right_border_bl_uv = br_tl_uv;
            vector2s right_border_tr_uv = tr_br_uv;
            vector2s right_border_br_uv = br_tr_uv;
            vertexData.tl = VecTransformTToD(right_border_tl);
            vertexData.tr = VecTransformTToD(right_border_tr);
            vertexData.bl = VecTransformTToD(right_border_bl);
            vertexData.br = VecTransformTToD(right_border_br);
            vertexData.tl_uv = right_border_tl_uv;
            vertexData.tr_uv = right_border_tr_uv;
            vertexData.bl_uv = right_border_bl_uv;
            vertexData.br_uv = right_border_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = true;
            DrawTilePart(vertexData, data);
        }
        else {
            float repeatCount = tileData->lr_repeatCount.y;
            if (repeatCount <= 0) {
                repeatCount = rightBorderSize.y < 0.000001f ? 1.0f : rightBorderSize.y / (tileData->rightBorderTexSize.y * tileData->tb_texScale.y);
            }
            else {
                repeatCount = 1.0f;
            }

            vector2s right_border_tl_uv = vector2s(0.0f, repeatCount);
            vector2s right_border_bl_uv = vector2s(0.0f, 0.0f);
            vector2s right_border_tr_uv = vector2s(1.0f, repeatCount);
            vector2s right_border_br_uv = vector2s(1.0f, 0.0f);
            vertexData.tl = VecTransformTToD(right_border_tl);
            vertexData.tr = VecTransformTToD(right_border_tr);
            vertexData.bl = VecTransformTToD(right_border_bl);
            vertexData.br = VecTransformTToD(right_border_br);
            vertexData.tl_uv = right_border_tl_uv;
            vertexData.tr_uv = right_border_tr_uv;
            vertexData.bl_uv = right_border_bl_uv;
            vertexData.br_uv = right_border_br_uv;
            //vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = false;
            vertexData.curTexNo = tileData->rightBorderTex;
            DrawTilePart(vertexData, data);
        }
    }


    if (tileData->centerTileType == E_TileType::TileType_Stretch) {
        //center
        vector3s center_tl = tl_br;
        vector3s center_bl = bl_tr;
        vector3s center_tr = tr_bl;
        vector3s center_br = br_tl;

        vector2s center_tl_uv = tl_br_uv;
        vector2s center_bl_uv = bl_tr_uv;
        vector2s center_tr_uv = tr_bl_uv;
        vector2s center_br_uv = br_tl_uv;

        //RIGHT
        vertexData.tl = VecTransformTToD(center_tl);
        vertexData.tr = VecTransformTToD(center_tr);
        vertexData.bl = VecTransformTToD(center_bl);
        vertexData.br = VecTransformTToD(center_br);
        vertexData.tl_uv = center_tl_uv;
        vertexData.tr_uv = center_tr_uv;
        vertexData.bl_uv = center_bl_uv;
        vertexData.br_uv = center_br_uv;
        vertexData.AdjustTexCoord();
        vertexData.useDefaultTexture = false;
        vertexData.curTexNo = tileData->centerTex;
        DrawTilePart(vertexData, data);
    }
    else if (tileData->centerTileType == E_TileType::TileType_Repeat) {
        //TODO:it will use slice texture

        //center
        vector3s center_tl = tl_br;
        vector3s center_bl = bl_tr;
        vector3s center_tr = tr_bl;
        vector3s center_br = br_tl;

        if (tileData->centerTex == -1) {

            vector2s center_tl_uv = tl_br_uv;
            vector2s center_bl_uv = bl_tr_uv;
            vector2s center_tr_uv = tr_bl_uv;
            vector2s center_br_uv = br_tl_uv;

            vertexData.tl = VecTransformTToD(center_tl);
            vertexData.tr = VecTransformTToD(center_tr);
            vertexData.bl = VecTransformTToD(center_bl);
            vertexData.br = VecTransformTToD(center_br);
            vertexData.tl_uv = center_tl_uv;
            vertexData.tr_uv = center_tr_uv;
            vertexData.bl_uv = center_bl_uv;
            vertexData.br_uv = center_br_uv;
            vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = true;
            DrawTilePart(vertexData, data);
        }
        else {
            float repeatCountX = 1.0f;
            float repeatCountY = 1.0f;

            repeatCountX = topBorderSize.x < 0.000001f ? 1.0f : topBorderSize.x / (tileData->centerTexSize.x * tileData->center_texScale.x);
            repeatCountY = rightBorderSize.y < 0.000001f ? 1.0f : rightBorderSize.y / (tileData->centerTexSize.y * tileData->center_texScale.y);

            vector2s center_tl_uv = vector2s(0.0f, repeatCountY);
            vector2s center_bl_uv = vector2s(0.0f, 0.0f);
            vector2s center_tr_uv = vector2s(repeatCountX, repeatCountY);
            vector2s center_br_uv = vector2s(repeatCountX, 0.0f);

            vertexData.tl = VecTransformTToD(center_tl);
            vertexData.tr = VecTransformTToD(center_tr);
            vertexData.bl = VecTransformTToD(center_bl);
            vertexData.br = VecTransformTToD(center_br);
            vertexData.tl_uv = center_tl_uv;
            vertexData.tr_uv = center_tr_uv;
            vertexData.bl_uv = center_bl_uv;
            vertexData.br_uv = center_br_uv;
            //vertexData.AdjustTexCoord();
            vertexData.useDefaultTexture = false;
            vertexData.curTexNo = tileData->centerTex;
            DrawTilePart(vertexData, data);
        }
    }
    //GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);
}

void DrawTilePart(DrawVertexData& drawData,DrawData* data) {
    if (data == NULL)return;
    //if (drawData.vertices == NULL)return;
    //vertex& v_1 = defaultVertices[0];

    if (drawData.useDefaultTexture == true) {
        GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(data->GetTextureNo()));//テクスチャのセット

    }
    else {
        GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(drawData.curTexNo));//テクスチャのセット
    }

    D3D11_MAPPED_SUBRESOURCE msr;
    GetDeviceContext()->Map(g_VertexBufferTriangle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

    VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

    //vertex[0].Position = vector3s(drawData.br.x, drawData.br.y, drawData.z);
    //vertex[1].Position = vector3s(drawData.bl.x, drawData.bl.y, drawData.z);
    //vertex[2].Position = vector3s(drawData.tr.x, drawData.tr.y, drawData.z);
    //vertex[3].Position = vector3s(drawData.tl.x, drawData.tl.y, drawData.z);
    //vertex[0].TexCoord = drawData.br_uv;
    //vertex[1].TexCoord = drawData.bl_uv;
    //vertex[2].TexCoord = drawData.tr_uv;
    //vertex[3].TexCoord = drawData.tl_uv;

    vertex[0].Position = vector3s(drawData.tl.x, drawData.tl.y, drawData.z);
    vertex[1].Position = vector3s(drawData.tr.x, drawData.tr.y, drawData.z);
    vertex[2].Position = vector3s(drawData.bl.x, drawData.bl.y, drawData.z);
    vertex[3].Position = vector3s(drawData.br.x, drawData.br.y, drawData.z);
    vertex[0].TexCoord = drawData.tl_uv;
    vertex[1].TexCoord = drawData.tr_uv;
    vertex[2].TexCoord = drawData.bl_uv;
    vertex[3].TexCoord = drawData.br_uv;

    vertex[0].Diffuse =data->GetColor();
    vertex[1].Diffuse =data->GetColor();
    vertex[2].Diffuse =data->GetColor();
    vertex[3].Diffuse =data->GetColor();


    //追加==================
    //=====カメラ処理=======
    MainCamera* mainCamera = MainCamera::GetInstance();
    vector2s CameraPos = *mainCamera->GetCamPos();

    for (int i = 0; i < 4; i++)
    {
        vertex[i].Position.x -= CameraPos.x;
        vertex[i].Position.y -= CameraPos.y;
    }

    GetDeviceContext()->Unmap(g_VertexBufferTriangle, 0);

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;

    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferTriangle, &stride, &offset);

    GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    GetDeviceContext()->Draw(NUM_VERTEX, 0);

}