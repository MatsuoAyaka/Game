#pragma once

#include "GameObject.h"

enum E_Effect;

class Effect :public BaseCell {
protected:
	E_Effect m_effectType;
public:
	Effect();
	~Effect();

	void StartEffect();
	E_Effect GetEffectType();
	//virtual void DeleteCell();
	virtual void OnCellRecycle();
	virtual void OnCellInit();
};

class EffectHit : public Effect {

public:
	EffectHit();
	~EffectHit();

};

class EffectJump :public Effect {

public:
	EffectJump();
	~EffectJump();

};

class EffectRush : public Effect {
public:
	EffectRush();
	~EffectRush();
};

class EffectSweat :public Effect {
public:
	EffectSweat();
	~EffectSweat();
};

class EffectRecover :public Effect {
public:
	EffectRecover();
	~EffectRecover();
};

class EffectSakura : public Effect
{
public:
	EffectSakura();
	~EffectSakura() {}
};

class EffectButton :public Effect {
public:
	EffectButton();
	~EffectButton();
};