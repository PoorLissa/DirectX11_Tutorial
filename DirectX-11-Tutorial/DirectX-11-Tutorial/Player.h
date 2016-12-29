#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "gameObjectBase.h"
#include "BonusEffects.h"
#include "BonusWeapons.h"
#include "Bullet.h"
#include "Bullet_Ion.h"
#include "Monster.h"

#define UNUSED_BULLET_TYPE 12345

// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Игрок
class Player : public gameObjectBase {

 public:
    Player(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef interval, cuiRef anim_Qty, HighPrecisionTimer *timer)
		: gameObjectBase(x, y, scale, angle, speed, PLAYER_DEFAULT_HEALTH),
		    _Angle0(angle),
            _appTimer(timer),
            _bulletsType(0),
            _bulletsType_old(UNUSED_BULLET_TYPE)
	{
        for (unsigned int i = 0; i < BonusEffects::Effects::_totalEffectsQty; i++)
            EffectsCounters[i] = 0;
    }

   ~Player() {}

    enum BulletsType { NORMAL, PIERCING, FIRE, ION, ION_EXPLOSION, FREEZE, _lastType };

    virtual void Move(cfRef = 0, cfRef = 0, void* = nullptr);

    virtual inline cuiRef getAnimPhase() const { return 0; }

    inline void setDirectionL(const bool &dir) { _Left  = dir; }
    inline void setDirectionR(const bool &dir) { _Right = dir; }
    inline void setDirectionU(const bool &dir) { _Up    = dir; }
    inline void setDirectionD(const bool &dir) { _Down  = dir; }

    void setEffect(const unsigned int &);

    inline void setShieldedMode    (const bool &mode)        { _Shielded     = mode;         }
    inline void setBulletsType_On  (const BulletsType &mode) { _bulletsType |=  (1 << mode); }
    inline void setBulletsType_Off (const BulletsType &mode) { _bulletsType &= ~(1 << mode); }
    inline void resetBulletsType   ()                        { _bulletsType  = 0;            }
    inline unsigned short getBulletsType() const             {          return _bulletsType; }
    inline const bool& isShielded() const                    {             return _Shielded; }

    inline const UINT& getWeaponDelay() const                {          return _weaponDelay; }
    inline const UINT& getBulletSpeed() const                {    return _weaponBulletSpeed; }
    inline const UINT& getWeaponBurst() const                {    return _weaponBurstQty;    }

    inline const bool& isWeaponReady()                       { if( !(_weaponReady % _weaponDelay) ) { _weaponReady = 0; return true; } return false; }

    void spawnBullet_Normal   (const int &, const int &, std::list<gameObjectBase*> *, unsigned int &, const int &, const int &);
    void spawnBullet_FourSides(std::list<gameObjectBase*> *, unsigned int &);

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// Угол, который передаем в конструктор. Позволяет довернуть спрайт, если он изначально расположен не под тем углом, как мы хотим

    HighPrecisionTimer *_appTimer;                                              // Указатель на общий таймер приложения
    unsigned int EffectsCounters[BonusEffects::Effects::_totalEffectsQty];      // Массив счетчиков длительности эффектов

    // Бонусные эффекты
    bool _Shielded, _SpedUp;

    // Эффекты пуль. Устанавливаются на игрока (навсегда или до отмены) и при генерации пуль устанавливаются на каждую пулю в отдельности
    unsigned short _bulletsType, _bulletsType_old;

    unsigned int _weaponDelay, _weaponBulletSpeed, _weaponBurstQty, _weaponReady, _weaponBulletSpread, _weaponBulletsQty;
};

// Включено здесь по причине перекрестных инклюдов. Если включить его перед классом, все поломается
#include "Bonus.h"

#endif
