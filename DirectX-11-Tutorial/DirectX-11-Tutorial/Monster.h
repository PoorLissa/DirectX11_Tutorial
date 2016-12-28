#pragma once
#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "gameObjectBase.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Monster : public gameObjectBase {

 public:
    Monster(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef interval, cuiRef anim_Qty)
        : gameObjectBase(x, y, scale, angle, speed, MONSTER_DEFAULT_HEALTH),
            animInterval0(interval),
            animInterval1(interval),
            animQty(anim_Qty),
            animPhase(0),
            _radius(20),
            _Cell(2e6)
	{}
   ~Monster() {}

    virtual void Move(cfRef, cfRef, void *);

    void threadMove(cfRef x, cfRef y, void *Param);

	virtual inline cuiRef getAnimPhase() const { return animPhase; }

    static inline void setFreeze(const bool &mode) { _freezeEffect = mode;  }   // Bonus - ��������� ��������

    inline const UINT& getCell  () const  { return _Cell;   }
    inline void        setCell  (UINT &c) {    _Cell = c;   }
    inline const UINT& getRadius() const  { return _radius; }

 private:
	 int            animInterval0, animInterval1;
	 unsigned int   animQty, animPhase;
     unsigned int   _radius;
     static bool    _freezeEffect;  // ����� ���������� � true, ��� ������� �������������� �� �����
     unsigned int   _Cell;          // ����� ������� ������, ����� ��� ������ �������� ��������� ������. ����� ������� � ������ ��������� ��� ������� - �� ���������
};

#endif
