#pragma once
#include "main.h"
#define ONIGIRI_TEX_WIDTH 860
#define ONIGIRI_TEX_HEIGHT 780

using namespace std;



class Onigiri:public BaseCell
{
public:
	Onigiri();
	Onigiri(vector3s,vector2s);
	~Onigiri();
	int  GetHealth() { return m_health; }
	bool GetOnigiriUse() { return m_use; }
	void DelOnigiri();
	void Reset();
private:
	bool  m_use = true;
	const int m_health = 5;
	const D3DXCOLOR onigiri_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
};


class Sugar :public BaseCell
{
public:
	Sugar();
	~Sugar();
	void DelSugar();
	void Reset();
private:
};

class Pepper :public BaseCell
{
public:
	Pepper();
	~Pepper();
	void DelPepper();
	void Reset();
private:
};

class PickledPlums :public BaseCell
{
public:
	PickledPlums();
	~PickledPlums();
	void DelPickledPlums();
private:
};

class MisoSoup :public BaseCell
{
public:
	MisoSoup();
	~MisoSoup();
	void DelMisoSoup();
	void Reset();
private:
};