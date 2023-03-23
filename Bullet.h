#pragma once
#include "main.h"

using namespace std;

class Bullet :public BaseCell
{

private:
	float bulletSpeed;   //弾の速さ
	vector2s moveDir;    //弾の向き
	vector2s movement;
	int damage = 1;

public:
	Bullet();
	Bullet(vector3s pos);
	~Bullet(){};


	int GetDamage() { return damage; }

	void DoUpdate(float deltatime);

	void SetDir(vector2s _dir);//弾の方向ベクトルを設定する
	void SetSpeed(float _speed);//弾のスピードを設定
};

class EnemyBullet :public Bullet
{

private:
	float bulletSpeed;   //弾の速さ
	vector2s moveDir;    //弾の向き
	vector2s movement;
	int damage = 1;

public:
	EnemyBullet();
	EnemyBullet(vector3s pos);
	~EnemyBullet() {};

	int GetDamage() { return damage; }

	void DoUpdate(float deltatime);

	void SetDir(vector2s _dir);//弾の方向ベクトルを設定する
	void SetSpeed(float _speed);//弾のスピードを設定
};
