/*==============================================================================

   共通ヘッダー [main.h]
                                                         Author : 
                                                         Date   : 
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>
#include <ctime>
#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <chrono>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#define SCREEN_WIDTH	(1920)				// ウインドウの幅
#define SCREEN_HEIGHT	(1080)				// ウインドウの高さ
typedef D3DXVECTOR3 vector3s;
typedef D3DXVECTOR2 vector2s;
typedef D3DXVECTOR4 vector4s;


enum E_Dir {
	Dir_Up,
	Dir_Down,
	Dir_Left,
	Dir_Right,
};

#pragma region tool_method

inline float F_Clamp(float src, float min, float max);
inline int I_Lerp(float src, float dest, float progress);
inline float VecDistS(vector3s src, vector3s dest);
inline float VecDist(vector3s src, vector3s dest);
inline float VecLengthS(vector3s src);
inline float VecLengthS(vector2s src);
inline float VecLength(vector3s src);
inline float VecLength(vector2s src);
inline float VecDot(vector3s src, vector3s dest);
inline float VecDot(vector2s src, vector2s dest);
inline vector3s& VecClamp(vector3s src, float minV, float maxV);
inline vector2s& VecNormalize(const vector2s& src);
inline vector3s& VecNormalize(const vector3s& src);
inline vector3s& VecLerp(const vector3s& src, const vector3s& dest, float progress);
inline float s_max(float v1, float v2);
inline float s_min(float v1, float v2);
bool AdjustScreenPoint(vector3s& pos, vector2s size);
inline vector3s& GetReflectDir2D(const vector3s& src, const vector3s& normal);

inline float F_Clamp(float src, float min, float max) {
	float result = src;
	if (src < min) {
		result = min;
	}
	else if (src > max) {
		result = max;
	}

	return result;
}

inline int I_Lerp(float src, float dest, float progress) {
	if (progress >= 1) {
		return dest;
	}
	else if (progress <= 0) {
		return src;
	}

	return src + (dest - src) * progress;
}

inline float VecDistS(vector3s src, vector3s dest) {
	vector3s temp = src - dest;
	return temp.x * temp.x + temp.y * temp.y;
}

inline float VecDist(vector3s src, vector3s dest) {
	vector3s temp = src - dest;
	return sqrtf(temp.x * temp.x + temp.y * temp.y);
}

inline float VecLengthS(vector3s src) {
	return src.x * src.x + src.y * src.y + src.z * src.z;
}

inline float VecLengthS(vector2s src) {
	return src.x * src.x + src.y * src.y;
}

inline float VecLength(vector3s src) {
	return sqrtf(src.x * src.x + src.y * src.y + src.z * src.z);
}

inline float VecLength(vector2s src) {
	return sqrtf(src.x * src.x + src.y * src.y);
}

inline float VecDot(vector3s src, vector3s dest) {
	return src.x * dest.x + src.y * dest.y + src.z * dest.z;
}

inline float VecDot(vector2s src, vector2s dest) {
	return src.x * dest.x + src.y * dest.y;
}

inline vector2s& VecNormalize(const vector2s& src) {
	float length = VecLength(src);
	vector2s result;

	if (length <= 0.0001f) {
		result.x = 0;
		result.y = 0;
	}
	else {
		result = src / length;
	}
	return result;
}

inline vector3s& VecNormalize(const vector3s& src) {
	float length = VecLength(src);
	vector3s result;

	if (length <= 0.0001f) {
		result.x = 0;
		result.y = 0;
		result.z = 0;
	}
	else {
		result = src / length;
	}
	return result;
}

inline vector3s& operator*(vector3s& src, const float factor) {
	vector3s result;
	result.x = src.x * factor;
	result.y = src.y * factor;
	result.z = src.z * factor;
	return result;
}

inline vector3s& VecClamp(vector3s src, float minV, float maxV) {
	vector3s temp = src;
	float curLengthS = VecLengthS(temp);
	if (curLengthS < minV * minV) {
		vector3s tempNormalize = VecNormalize(temp);
		return tempNormalize * minV;
	}
	else if (curLengthS > maxV * maxV) {
		return VecNormalize(temp) * maxV;
	}

	return temp;
}

inline vector3s& VecLerp(const vector3s& src, const vector3s& dest, float progress) {
	vector3s result;
	if (progress >= 1) {
		result = dest;
	}
	else if (progress <= 0) {
		result = src;
	}

	result = src + progress * (dest - src);
	return result;
}

inline vector3s& GetReflectDir2D(const vector3s& src, const vector3s& normal) {
	vector3s result = vector3s(0, 0, 0);
	vector3s n = normal;
	n.z = 0.0f;
	n = VecNormalize(n);

	float num = src.x * n.x + src.y * n.y;

	result.x = src.x - ((2.0f * num) * n.x);
	result.y = src.y - ((2.0f * num) * n.y);

	return VecNormalize(result);
}

inline float s_max(float v1, float v2) {
	if (v1 > v2) {
		return v1;
	}
	else {
		return v2;
	}
}

inline float s_min(float v1, float v2) {
	if (v1 > v2) {
		return v2;
	}
	else {
		return v1;
	}
}


inline vector2s& VecTransformTToD(const vector3s& src) {
	vector2s result = vector2s(src.x, src.y);
	return result;
}

//inline vector3s& operator*(const vector3s& src, float factor);
//
//
//inline vector3s& operator*(const vector3s& src, const float& factor);
//
//inline vector3s& operator*(const vector3s& src, const vector3s& dest);
//
inline vector2s& operator*(const vector2s& src, const vector2s& dest) {
	vector2s result;
	result.x = src.x * dest.x;
	result.y = src.y * dest.y;
	return result;
}
//inline vector2s& operator*(const vector2s& src, const float& factor);
//inline vector2s& operator*( const float& factor,const vector2s& src);


inline int GetRandomRangeValueI(int _range) {
	if (_range == 0)return 0;
	int range = abs(_range);
	//[0 , range ] mapping to [ -range , range]
	return (rand() % range * 2 -  range);
}

inline int GetAbsRandomRangeValueI(int _range) {
	if (_range == 0)return 0;

	return rand() % _range;
}

#pragma endregion tool_method

#include "dinput.h"
#include "mmsystem.h"
#include "Sprite.h"
#include "Texture.h"
#include "sound.h"
#include "LineCircle.h"
#include "renderer.h"
#include "input.h"
#include "polygon.h"	//新しく追加するプログラムの情報
#include "Explosion.h"
#include "Game.h"

#include "GameLogic.h"
#include "GameObject.h"
#include "Component.h"
#include "ObjectPool.h"
#include "DataManager.h"
#pragma warning(pop)



//*****************************************************************************
// マクロ定義
//*****************************************************************************

//base
#define SCREEN_LIMIT_UP (0)//画面上限界座標
#define SCREEN_LIMIT_DOWN (1000)
#define SCREEN_LIMIT_RIGHT (1000)
#define SCREEN_LIMIT_LEFT (0)

#define MOUSE_SENSITY 0.05f

#define CAMERA_SHAKE_REDUCE_FACTOR 0.9f

#define SHOOT_RAY_POS_OFFSET 10.0f
#define RAY_REFLECTION_COUNT 5
#define RAY_MAX_LENGTH 1470
#define CELL_AFFECT_INTERVAL 0.05f

#define RAD2DEG 180.0f/3.1415926f
#define DEG2RAD 3.1415926f/180

#define SECOND_DETECT_OFFSET_X 5.0f
#define SECOND_DETECT_OFFSET_Y 5.0f

//23.01.26
#define SOUND_BOSS_FLY_TIME 3.0f

//player
#define CHARACTER_TEX_WIDTH 640
#define CHARACTER_TEX_HEIGHT 960
#define PLAYER_TEX_SIZE_X 64
#define PLAYER_TEX_SIZE_Y 64
#define PLAYER_TEX_W_COUNT 10
#define PLAYER_TEX_H_COUNT 6
#define PLAYER_IDLE_INDEX_X (1-1)
#define PLAYER_IDLE_INDEX_Y (14-(15-1))
#define PLAYER_IDLE_FRAME_COUNT 5
#define PLAYER_MOVE_INDEX_X (1-1)
#define PLAYER_MOVE_INDEX_Y (14-(14-1))
#define PLAYER_MOVE_FRAME_COUNT 8
#define PALYER_MAX_MOVE_SPEED 3.0f
//=======================================
//自分が変更
//#define PLAYER_MOVEMENT_SPEED 10.0f
#define PLAYER_MOVEMENT_SPEED 400.0f
#define PLAYER_ROTATION_SPEED 1.0f
//自分が変更
//======================================
#define SHOOT_POS_LIGHT_TEX_SIZE_X 64
#define SHOOT_POS_LIGHT_TEX_SIZE_Y 64
#define SHOOT_POS_LIGHT_TEX_W_COUNT 10
#define SHOOT_POS_LIGHT_TEX_H_COUNT 10
#define SHOOT_POS_LIGHT_START_INDEX_X (1-1)
#define SHOOT_POS_LIGHT_START_INDEX_Y (14-(13-1))
#define SHOOT_POS_LIGHT_FRAME_COUNT 10
#define SHOOT_POINT_OFFSET 20
//=============================================
//自分が追加した
#define PLAYER_INAIRJUMP_LIMIT	1	//空中ジャンプの制限回数
//#define MAX_BULLET	3
#define PLAYER_TUMBLE_TIME 60
#define PLAYER_SLOPE_LIMIT 45.0f
#define PLAYER_DEFAULT_REACTION_ROTZ 30.0f	//弾の反動で回転する角度
#define PLAYER_REACTION_ROTZ_SPEED 4.0f

#define PLAYER_RECOVER_ROTATION_SPEED 5.0f

#define PLAYER_DUSH_ACCELERATION 2.0f
#define PLAYER_DUSH_MAX_SPEED	 8.0f
//自分が追加した
//============================================
//npc
#define NPC_TEX_SIZE_X 64
#define NPC_TEX_SIZE_Y 64
#define NPC_TEX_W_COUNT 10
#define NPC_TEX_H_COUNT 6
#define NPC_IDLE_INDEX_X (1-1)
#define NPC_IDLE_INDEX_Y (14-(12-1))
#define NPC_IDLE_FRAME_COUNT 5
#define NPC_MOVE_INDEX_X (1-1)
#define NPC_MOVE_INDEX_Y (14-(11-1))
#define NPC_MOVE_FRAME_COUNT 8
#define NPC_HP 0.001f

#define WALL_HP 0.5f

//ai
#define AI_NPC_MOVE_SPEED 100

//obstacle 
#define OBSTACLE_CHECK_TIMES 3

#define TEST_TEX_SIZE_X 237
#define TEST_TEX_SIZE_Y 314


//=============================================
//=============================================23.1 テクスチャーパラメータ

//プレイヤーアニメーション
#define TEX_PLAYER_SIZE_X 12800
#define TEX_PLAYER_SIZE_Y 6400
#define TEX_PLAYER_ANIM_CELL_SIZE_X 640
#define TEX_PLAYER_ANIM_CELL_SIZE_Y 640
#define TEX_PLAYER_ANIM_CELL_W_COUNT 20
#define TEX_PLAYER_ANIM_CELL_H_COUNT 10

#define TEX_PLAYER_ANIM_IDLE_START_INDEX_X (1 - 1)		//待機
#define TEX_PLAYER_ANIM_IDLE_START_INDEX_Y (2 - 1)
#define TEX_PLAYER_ANIM_IDLE_FRAME_COUNT 19
#define TEX_PLAYER_ANIM_MOVE_START_INDEX_X (1 - 1)		//歩く
#define TEX_PLAYER_ANIM_MOVE_START_INDEX_Y (6 - 1)
#define TEX_PLAYER_ANIM_MOVE_FRAME_COUNT 19
#define TEX_PLAYER_ANIM_SLIDE_START_INDEX_X (1 - 1)		//滑り
#define TEX_PLAYER_ANIM_SLIDE_START_INDEX_Y (3 - 1)
#define TEX_PLAYER_ANIM_SLIDE_FRAME_COUNT 12
#define TEX_PLAYER_ANIM_TUMBLE_START_INDEX_X (1 - 1)		//転ぶ1
#define TEX_PLAYER_ANIM_TUMBLE_START_INDEX_Y (4 - 1)
#define TEX_PLAYER_ANIM_TUMBLE_FRAME_COUNT 20
#define TEX_PLAYER_ANIM_TUMBLE_2_START_INDEX_X (1 - 1)		//転ぶ2
#define TEX_PLAYER_ANIM_TUMBLE_2_START_INDEX_Y (5 - 1)
#define TEX_PLAYER_ANIM_TUMBLE_2_FRAME_COUNT 20
#define TEX_PLAYER_ANIM_HURT_START_INDEX_X (1 - 1)		//攻撃受け
#define TEX_PLAYER_ANIM_HURT_START_INDEX_Y (1 - 1)
#define TEX_PLAYER_ANIM_HURT_FRAME_COUNT 15
#define TEX_PLAYER_ANIM_WIN_START_INDEX_X (1 - 1)				//勝利アニメーション
#define TEX_PLAYER_ANIM_WIN_START_INDEX_Y (7 - 1)
#define TEX_PLAYER_ANIM_WIN_FRAME_COUNT 26

#define TEX_EFFECT_RICE_BIG_START_INDEX_X (1 - 1)		//rice effect
#define TEX_EFFECT_RICE_BIG_START_INDEX_Y (9 - 1)
#define TEX_EFFECT_RICE_BIG_FRAME_COUNT 16
#define TEX_EFFECT_RICE_SMALL_START_INDEX_X (1 - 1)		//rice effect 2
#define TEX_EFFECT_RICE_SMALL_START_INDEX_Y (10 - 1)
#define TEX_EFFECT_RICE_SMALL_FRAME_COUNT 12

//桜吹雪
#define SAKURA_SPEED (1.8f)

#define TEX_EFFECT_SAKURA_START_INDEX_X (0)	
#define TEX_EFFECT_SAKURA_START_INDEX_Y (0)
#define TEX_EFFECT_SAKURA_FRAME_COUNT 30
#define TEX_EFFECT_SAKURA_CELL_W_COUNT 6

#define TEX_EFFECT_SAKURA_SIZE_X 11520
#define TEX_EFFECT_SAKURA_SIZE_Y 5400
#define TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_X 1920
#define TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_Y 1080

//リザルト
#define RESULTEFF_SPEED (1.8f)

#define TEX_EFFECT_RESULT_START_INDEX_X (0)	
#define TEX_EFFECT_RESULT_START_INDEX_Y (0)
#define TEX_EFFECT_RESULT_FRAME_COUNT 30
#define TEX_EFFECT_RESULT_CELL_W_COUNT 6

#define TEX_EFFECT_RESULT_SIZE_X 11520
#define TEX_EFFECT_RESULT_SIZE_Y 5400
#define TEX_EFFECT_RESULT_ANIM_CELL_SIZE_X 1920
#define TEX_EFFECT_RESULT_ANIM_CELL_SIZE_Y 1080

//光る文字
#define LIGHTTEXT_SPEED (1.8f)

#define TEX_EFFECT_LIGHTTEXT_START_INDEX_X (0)	
#define TEX_EFFECT_LIGHTTEXT_START_INDEX_Y (0)
#define TEX_EFFECT_LIGHTTEXT_FRAME_COUNT 30
#define TEX_EFFECT_LIGHTTEXT_CELL_W_COUNT 6

#define TEX_EFFECT_LIGHTTEXT_SIZE_X 11520
#define TEX_EFFECT_LIGHTTEXT_SIZE_Y 5400
#define TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_X 1920
#define TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_Y 1080

//敵-雑魚
#define TEX_ENEMY_SIZE_X 12160
#define TEX_ENEMY_SIZE_Y 5120
#define TEX_ENEMY_CELL_SIZE_X 640
#define TEX_ENEMY_CELL_SIZE_Y 640
#define TEX_ENEMY_CELL_W_COUNT 19
#define TEX_ENEMY_CELL_H_COUNT 8
	//ヤドカリコロネ
#define TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_X (1 - 1)		//待機
#define TEX_ENEMY_CORNET_ANIM_IDLE_START_INDEX_Y (2 - 1)
#define TEX_ENEMY_CORNET_ANIM_IDLE_FRAME_COUNT 19
#define TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_X (1 - 1)		//移動
#define TEX_ENEMY_CORNET_ANIM_MOVE_START_INDEX_Y (1 - 1)
#define TEX_ENEMY_CORNET_ANIM_MOVE_FRAME_COUNT 19

	//パスタエイリアン
#define TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_X (1 - 1)		//待機
#define TEX_ENEMY_PASTA_ANIM_IDLE_START_INDEX_Y (4 - 1)
#define TEX_ENEMY_PASTA_ANIM_IDLE_FRAME_COUNT 19
#define TEX_ENEMY_PASTA_ANIM_MOVE_START_INDEX_X (1 - 1)		//移動
#define TEX_ENEMY_PASTA_ANIM_MOVE_START_INDEX_Y (3 - 1)
#define TEX_ENEMY_PASTA_ANIM_MOVE_FRAME_COUNT 19

	//一反木綿
#define TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_X (1 - 1)		//待機
#define TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_START_INDEX_Y (6 - 1)
#define TEX_ENEMY_ITTANMOMEN_ANIM_IDLE_FRAME_COUNT 19
#define TEX_ENEMY_ITTANMOMEN_ANIM_MOVE_START_INDEX_X (1 - 1)		//移動
#define TEX_ENEMY_ITTANMOMEN_ANIM_MOVE_START_INDEX_Y (5 - 1)
#define TEX_ENEMY_ITTANMOMEN_ANIM_MOVE_FRAME_COUNT 19

	//餃子亀
#define TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_X (1 - 1)		//待機
#define TEX_ENEMY_DUMPLING_ANIM_IDLE_START_INDEX_Y (8 - 1)
#define TEX_ENEMY_DUMPLING_ANIM_IDLE_FRAME_COUNT 19
#define TEX_ENEMY_DUMPLING_ANIM_MOVE_START_INDEX_X (1 - 1)		//移動
#define TEX_ENEMY_DUMPLING_ANIM_MOVE_START_INDEX_Y (7 - 1)
#define TEX_ENEMY_DUMPLING_ANIM_MOVE_FRAME_COUNT 19

//シーンオブジェクト
#define TEX_ENVIRONMENT_ITEM_SIZE_X 3840
#define TEX_ENVIRONMENT_ITEM_SIZE_Y 1024
#define TEX_ENVIRONMENT_ITEM_CELL_SIZE_X 256
#define TEX_ENVIRONMENT_ITEM_CELL_SIZE_Y 256
#define TEX_ENVIRONMENT_ITEM_CELL_W_COUNT 15
#define TEX_ENVIRONMENT_ITEM_CELL_H_COUNT 4

#define TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_X (1 - 1)			//風
#define TEX_ENVIRONMENT_ITEM_WIND_START_INDEX_Y (2 - 1)
#define TEX_ENVIRONMENT_ITEM_WIND_FRAME_COUNT 15

#define TEX_ENVIRONMENT_ICE_START_INDEX_X (8 - 1)				//氷
#define TEX_ENVIRONMENT_ICE_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_SPRING_START_INDEX_X (1 - 1)			//バネ
#define TEX_ENVIRONMENT_SPRING_START_INDEX_Y (3 - 1)
#define TEX_ENVIRONMENT_SPRING_ANIM_FRAME_COUNT 14

	//アイテム
#define TEX_ENVIRONMENT_ITEM_ONIGIRI_START_INDEX_X (1 - 1)		//おにぎり
#define TEX_ENVIRONMENT_ITEM_ONIGIRI_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_ITEM_SUGAR_START_INDEX_X (2 - 1)		//角砂糖
#define TEX_ENVIRONMENT_ITEM_SUGAR_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_ITEM_PEPPER_START_INDEX_X (4 - 1)		//唐辛子
#define TEX_ENVIRONMENT_ITEM_PEPPER_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_ITEM_PICKLED_PLUMS_START_INDEX_X (5 - 1)		//梅干し
#define TEX_ENVIRONMENT_ITEM_PICKLED_PLUMS_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_ITEM_MISO_SOUP_START_INDEX_X (7 - 1)		//味噌汁
#define TEX_ENVIRONMENT_ITEM_MISO_SOUP_START_INDEX_Y (4 - 1)

#define TEX_ENVIRONMENT_ITEM_RICE_START_INDEX_X (6 - 1)		//米粒
#define TEX_ENVIRONMENT_ITEM_RICE_START_INDEX_Y (4 - 1)

//ベルトコンベア
#define TEX_BELTCONVEYOR_SIZE_X 3840
#define TEX_BELTCONVEYOR_SIZE_Y 256
#define TEX_BELTCONVEYOR_CELL_SIZE_X 256
#define TEX_BELTCONVEYOR_CELL_SIZE_Y 256
#define TEX_BELTCONVEYOR_W_COUNT 15
#define TEX_BELTCONVEYOR_H_COUNT 1

#define TEX_BELTCONVEYOR_START_INDEX_X (1 - 1)
#define TEX_BELTCONVEYOR_START_INDEX_Y (1 - 1)
#define TEX_BELTCONVEYOR_ANIM_FRAME_COUNT 15
#define TEX_BELTCONVEYOR_CELL_SCALE_SIZE_X 80.0f
#define TEX_BELTCONVEYOR_CELL_SCALE_SIZE_Y 80.0f

//エフェクト
#define TEX_EFFECT_SIZE_X 9600
#define TEX_EFFECT_SIZE_Y 1920

#define TEX_EFFECT_RECOVER_SIZE_X 1536
#define TEX_EFFECT_RECOVER_SIZE_Y 1280

#define TEX_EFFECT_BUTTON_SIZE_X 514
#define TEX_EFFECT_BUTTON_SIZE_Y 257

#define TEX_EFFECT_CELL_SIZE_X 640
#define TEX_EFFECT_CELL_SIZE_Y 640
#define TEX_EFFECT_W_COUNT 15
#define TEX_EFFECT_H_COUNT 3

#define TEX_EFFECT_RECOVER_CELL_SIZE_X 256
#define TEX_EFFECT_RECOVER_CELL_SIZE_Y 256
#define TEX_EFFECT_RECOVER_W_COUNT 6
#define TEX_EFFECT_RECOVER_H_COUNT 5

#define TEX_EFFECT_BUTTON_CELL_SIZE_X 257
#define TEX_EFFECT_BUTTON_CELL_SIZE_Y 257
#define TEX_EFFECT_BUTTON_W_COUNT 2
#define TEX_EFFECT_BUTTON_H_COUNT 1

#define TEX_EFFECT_JUMP_CELL_W_COUNT 10
#define TEX_EFFECT_JUMP_CELL_H_COUNT 3
#define TEX_EFFECT_JUMP_CELL_SIZE_X 960
#define TEX_EFFECT_JUMP_CELL_SIZE_Y 640

#define TEX_EFFECT_JUMP_DUST_START_INDEX_X (1 - 1)
#define TEX_EFFECT_JUMP_DUST_START_INDEX_Y (1 - 1)
#define TEX_EFFECT_JUMP_DUST_ANIM_FRAME_COUNT 10

#define TEX_EFFECT_HIT_START_INDEX_X (1 - 1)
#define TEX_EFFECT_HIT_START_INDEX_Y (2 - 1)
#define TEX_EFFECT_HIT_ANIM_FRAME_COUNT 7

#define TEX_EFFECT_RUSH_START_INDEX_X (1 - 1)
#define TEX_EFFECT_RUSH_START_INDEX_Y (3 - 1)
#define TEX_EFFECT_RUSH_ANIM_FRAME_COUNT 3

#define TEX_EFFECT_RECOVER_START_INDEX_X (1 - 1)
#define TEX_EFFECT_RECOVER_START_INDEX_Y (1 - 1)
#define TEX_EFFECT_RECOVER_ANIM_FRAME_COUNT 30

#define TEX_EFFECT_BUTTON_START_INDEX_X (1 - 1)
#define TEX_EFFECT_BUTTON_START_INDEX_Y (1 - 1)
#define TEX_EFFECT_BUTTON_ANIM_FRAME_COUNT 2
//ボス
#define TEX_BOSS_SIZE_X 12000
#define TEX_BOSS_SIZE_Y 1260
#define TEX_BOSS_CELL_SIZE_X 600
#define TEX_BOSS_CELL_SIZE_Y 420
#define TEX_BOSS_W_COUNT 20
#define TEX_BOSS_H_COUNT 3

#define TEX_BOSS_ANIM_IDLE_START_INDEX_X (1 - 1)
#define TEX_BOSS_ANIM_IDLE_START_INDEX_Y (1 - 1)
#define TEX_BOSS_ANIM_IDLE_FRAME_COUNT 20
#define TEX_BOSS_ANIM_MOVE_START_INDEX_X (1 - 1)
#define TEX_BOSS_ANIM_MOVE_START_INDEX_Y (2 - 1)
#define TEX_BOSS_ANIM_MOVE_FRAME_COUNT 20
#define TEX_BOSS_ANIM_FLY_START_INDEX_X (1 - 1)
#define TEX_BOSS_ANIM_FLY_START_INDEX_Y (3 - 1)
#define TEX_BOSS_ANIM_FLY_FRAME_COUNT 18

//mochi
#define TEX_MOCHI_SIZE_X 3840
#define TEX_MOCHI_SIZE_Y 180
#define TEX_MOCHI_COUNT_W 15
#define TEX_MOCHI_COUNT_H 1
#define TEX_MOCHI_CELL_SIZE_X 256
#define TEX_MOCHI_CELL_SIZE_Y 180
#define TEX_MOCHI_ANIM_START_INDEX_X (1 - 1)
#define TEX_MOCHI_ANIM_START_INDEX_Y (1 - 1)
#define TEX_MOCHI_ANIM_FRAME_COUNT 15


//num_tex
#define TEX_NUM_SIZE_X 2560
#define TEX_NUM_SIZE_Y 256
#define TEX_NUM_COUNT_W 10
#define TEX_NUM_COUNT_H 1
#define TEX_NUM_CELL_SIZE_X 256
#define TEX_NUM_CELL_SIZE_Y 256
#define TEX_NUM_START_INDEX_X (1 - 1)
#define TEX_NUM_START_INDEX_Y (1 - 1)

//ファイアボール
#define TEX_FIREBALL_SIZE_X 4096
#define TEX_FIREBALL_SIZE_Y 256
#define TEX_FIREBALL_COUNT_W 16
#define TEX_FIREBALL_COUNT_H 1
#define TEX_FIREBALL_CELL_SIZE_X 256
#define TEX_FIREBALL_CELL_SIZE_Y 256
#define TEX_FIREBALL_START_INDEX_X (1 - 1)
#define TEX_FIREBALL_START_INDEX_Y (1 - 1)
#define TEX_FIREBALL_ANIM_FRAME_COUNT 16



//======================================他のパラメータ
#define MOCHI_GROUND_COLLIDER_Y_FACTOR 0.6f
#define MOCHI_GROUND_COLLIDER_Y_OFFSET_FACTOR 0.2f


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

const vector2s screen_size = vector2s(1.0f / SCREEN_WIDTH, 1.0f / SCREEN_HEIGHT);

const float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
const vector3s upVector = vector3s(0.0f, -1.0f, 0.0f);
const vector3s rightVector = vector3s(1.0f, 0.0f, 0.0f);
const vector3s downVector = vector3s(0.0f, 1.0f, 0.0f);
const vector3s leftVector = vector3s(-1.0f, 0.0f, 0.0f);

const vector2s sceneTransformBGSize = vector2s(SCREEN_WIDTH, SCREEN_HEIGHT);
const vector2s defaultRaySize = vector2s(100.0f, 20.0f);
//const vector2s character_cell_tex_size = vector2s((float)PLAYER_TEX_SIZE_X / CHARACTER_TEX_WIDTH, (float)PLAYER_TEX_SIZE_Y / CHARACTER_TEX_HEIGHT);
const vector2s character_size = vector2s(200.0f, 200.0f);
const vector2s item_size = vector2s(character_size.x * 0.66f, character_size.y * 0.66f);
const vector2s jump_effct_size = vector2s(character_size.x * 1.5f, character_size.y );
const vector2s boss_size = vector2s(200.0f, 200.0f);
const vector2s rush_effect_size = vector2s(character_size.x * 1.5f, character_size.y);
const vector2s bullet_size = vector2s(40.0f, 40.0f);

const D3DXCOLOR affect_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR wall_color = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
const D3DXCOLOR obstacle_color = D3DXCOLOR(0.509f, 0.580f, 0.376f, 1.0f);
//other
const vector3s player_stage_1_defaultPos = vector3s(80.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
const vector3s player_stage_1_shoot_dir = vector3s(0.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
const vector3s player_stage_3_defaultPos = vector3s(80.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
const vector3s player_stage_3_shoot_dir = vector3s(0.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
const vector3s player_stage_4_defaultPos = vector3s(150.0f, 100.0f, 0.0f);
const vector3s player_stage_4_shoot_dir = vector3s(150.0f, 0.0f, 0.0f);
const vector3s player_stage_5_defaultPos = vector3s(80.0f, SCREEN_HEIGHT * 0.5f, 0.0f);
const vector3s player_stage_5_shoot_dir = vector3s(0.0f, SCREEN_HEIGHT * 0.5f, 0.0f);

const vector2s test_tex_size = vector2s(1.0f / TEST_TEX_SIZE_X, 1.0f / TEST_TEX_SIZE_Y);

//=================23.1 
const vector2s character_cell_tex_size = vector2s((float)TEX_PLAYER_ANIM_CELL_SIZE_X / TEX_PLAYER_SIZE_X, (float)TEX_PLAYER_ANIM_CELL_SIZE_Y / TEX_PLAYER_SIZE_Y);
const vector2s environment_item_cell_tex_size = vector2s((float)TEX_ENVIRONMENT_ITEM_CELL_SIZE_X / TEX_ENVIRONMENT_ITEM_SIZE_X, (float)TEX_ENVIRONMENT_ITEM_CELL_SIZE_Y / TEX_ENVIRONMENT_ITEM_SIZE_Y);
const vector2s enemy_cell_tex_size = vector2s((float)TEX_ENEMY_CELL_SIZE_X / TEX_ENEMY_SIZE_X, (float)TEX_ENEMY_CELL_SIZE_Y / TEX_ENEMY_SIZE_Y);
const vector2s belt_conveyor_cell_tex_size = vector2s((float)TEX_BELTCONVEYOR_CELL_SIZE_X / TEX_BELTCONVEYOR_SIZE_X, (float)TEX_BELTCONVEYOR_SIZE_Y / TEX_BELTCONVEYOR_SIZE_Y);
const vector2s effect_recover_cell_tex_size = vector2s((float)TEX_EFFECT_RECOVER_CELL_SIZE_X / TEX_EFFECT_RECOVER_SIZE_X, (float)TEX_EFFECT_RECOVER_CELL_SIZE_Y / TEX_EFFECT_RECOVER_SIZE_Y);
const vector2s effect_button_cell_tex_size = vector2s((float)TEX_EFFECT_BUTTON_CELL_SIZE_X / TEX_EFFECT_BUTTON_SIZE_X, (float)TEX_EFFECT_BUTTON_CELL_SIZE_Y / TEX_EFFECT_BUTTON_SIZE_Y);

const vector2s effect_cell_tex_size = vector2s((float)TEX_EFFECT_CELL_SIZE_X / TEX_EFFECT_SIZE_X, (float)TEX_EFFECT_CELL_SIZE_Y / TEX_EFFECT_SIZE_Y);
const vector2s effect_jump_cell_tex_size = vector2s((float)TEX_EFFECT_JUMP_CELL_SIZE_X / TEX_EFFECT_SIZE_X, (float)TEX_EFFECT_JUMP_CELL_SIZE_Y / TEX_EFFECT_SIZE_Y);
const vector2s boss_cell_tex_size = vector2s((float)TEX_BOSS_CELL_SIZE_X / TEX_BOSS_SIZE_X, (float)TEX_BOSS_CELL_SIZE_Y / TEX_BOSS_SIZE_Y);
const vector2s mochi_tex_size = vector2s((float)TEX_MOCHI_CELL_SIZE_X / TEX_MOCHI_SIZE_X, (float)TEX_MOCHI_CELL_SIZE_Y / TEX_MOCHI_SIZE_Y);
const vector2s num_tex_size = vector2s((float)TEX_NUM_CELL_SIZE_X / TEX_NUM_SIZE_X, (float)TEX_NUM_CELL_SIZE_Y / TEX_NUM_SIZE_Y);
const vector2s fire_ball_tex_size = vector2s((float)TEX_FIREBALL_CELL_SIZE_X / TEX_FIREBALL_SIZE_X, (float)TEX_FIREBALL_CELL_SIZE_Y / TEX_FIREBALL_SIZE_Y);


const vector2s sakuraEff_size = vector2s(TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_X, TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_Y);
const vector2s sakura_cell_tex_size = vector2s((float)TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_X / TEX_EFFECT_SAKURA_SIZE_X, (float)TEX_EFFECT_SAKURA_ANIM_CELL_SIZE_Y / TEX_EFFECT_SAKURA_SIZE_Y);
const vector2s resultEff_size = vector2s(TEX_EFFECT_RESULT_ANIM_CELL_SIZE_X, TEX_EFFECT_RESULT_ANIM_CELL_SIZE_Y);
const vector2s result_cell_tex_size = vector2s((float)TEX_EFFECT_RESULT_ANIM_CELL_SIZE_X / TEX_EFFECT_RESULT_SIZE_X, (float)TEX_EFFECT_RESULT_ANIM_CELL_SIZE_Y / TEX_EFFECT_RESULT_SIZE_Y);
const vector2s lightTextEff_size = vector2s(TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_X, TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_Y);
const vector2s lightText_cell_tex_size = vector2s((float)TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_X / TEX_EFFECT_LIGHTTEXT_SIZE_X, (float)TEX_EFFECT_LIGHTTEXT_ANIM_CELL_SIZE_Y / TEX_EFFECT_LIGHTTEXT_SIZE_Y);


//================================サイズ(23.01.19)
const vector2s const_size_player = vector2s(200.0f, 200.0f);
const vector2s const_size_item = vector2s(140.0f, 140.0f);
const vector2s const_size_enemy_dumpling = vector2s(230.0f, 230.0f);
const vector2s const_size_enemy_pasta = vector2s(230.0f, 230.0f);
const vector2s const_size_enemy_ittanmomen = vector2s(260.0f, 260.0f);
const vector2s const_size_enemy_cornet = vector2s(230.0f, 230.0f);
const vector2s const_size_enemy_boss = vector2s(600.0f, 420.0f);

vector2s GetMouseMovement();
HWND& GetHWnd();