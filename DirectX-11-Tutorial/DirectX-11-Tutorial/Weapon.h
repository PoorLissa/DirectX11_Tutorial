#pragma once
#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "gameObjectBase.h"
#include "BonusWeapons.h"
#include "Bonus.h"

// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Оружие
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
            _ScaleModifier(1.0f),
            _mouseHover(0)
	{}
   ~Weapon() {}

    // Для бонуса-оружия рассчитываем время жизни, поворот, масштаб и взаимодействие с Игроком
    virtual void Move(cfRef, cfRef, void *);

    // В качестве номера анимации просто отдаем номер эффекта и все время показываем одно и то же изображение
    // Здесь пришлось ввести дополнительное поле, т.к. все заточено под (weapon - Weapons::PISTOL), а по ссылке нельзя вернуть вычисленное значение
    virtual inline cuiRef getAnimPhase() const { return _WeaponReduced; }

 private:
	 unsigned int   _LifeTime;
     unsigned int   _Weapon, _WeaponReduced;
     float          _AngleCounter;
     float          _ScaleCounter, _ScaleModifier;
     unsigned int   _flashCounter;
     unsigned short _mouseHover;
};

#endif
