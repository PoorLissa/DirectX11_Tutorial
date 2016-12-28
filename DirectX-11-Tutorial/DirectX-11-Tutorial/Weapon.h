#pragma once
#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "gameObjectBase.h"
#include "BonusWeapons.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Weapon : public gameObjectBase, public BonusWeapons {

 public:
	Weapon(cfRef x, cfRef y, const Weapons &weapon)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f, 1),
          BonusWeapons(),
            _LifeTime(3000 + rand()%2000),
            _Weapon(weapon),
            _WeaponReduced(weapon - Weapons::PISTOL),
            _AngleCounter(rand()%10),
            _ScaleCounter(0),
            _flashCounter(0),
            _ScaleModifier(1.0f)
	{}
   ~Weapon() {}

    // ��� ������-������ ������������ ����� �����, �������, ������� � �������������� � �������
    virtual void Move(cfRef, cfRef, void *);

    // � �������� ������ �������� ������ ������ ����� ������� � ��� ����� ���������� ���� � �� �� �����������
    // ����� �������� ������ �������������� ����, �.�. ��� �������� ��� (weapon - Weapons::PISTOL), � �� ������ ������ ������� ����������� ��������
    virtual inline cuiRef getAnimPhase() const { return _WeaponReduced; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Weapon, _WeaponReduced;
     float        _AngleCounter;
     float        _ScaleCounter, _ScaleModifier;
     unsigned int _flashCounter;
};

#endif
