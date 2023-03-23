#pragma once
#include "Game.h"
#include "main.h"
#include "renderer.h"
#include "Sprite.h"

#include "Explosion.h"
#include "Texture.h"
#include "polygon.h"

GameMgr* GameMgr::instance = NULL;//シングルトン変数を初期化する
GameMgr::Deletor deletor;

void InitGame() {
	//ゲーム関連初期化
	InitSprite();
	InitPolygon();		//ポリゴンの初期化を実行

	InitExplosion();

	srand(timeGetTime());	//乱数初期化

	GameMgr::GetInstance()->DoInit();//ゲームを初期化する

}

void UninitGame() {
	UninitSprite();
	UninitExplosion();
	// レンダリングの終了処理
	UninitRenderer();

	GameMgr::GetInstance()->DoUninit();//ゲームの終了処理
}

void UpdateGame() {
	UpdatePolygon();	//ポリゴンの更新処理
	UpdateExplosion();

	GameMgr::GetInstance()->DoUpdate(0.016f);//ゲームのアップデート（1フレイムの持続時間はとりあえず0.016秒に設定してた（一秒ごとに60フレイム）

}

void DrawGame() {
	DrawPolygon();			//ポリゴンの表示
	//DrawExplosion();

	GameMgr::GetInstance()->sceneObjMgr->DoDraw();//ゲームの描画処理
}


