#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

#define cfRef  const float &
#define ciRef  const int   &
#define cuiRef const unsigned int &

// ------------------------------------------------------------------------------------------------------------------------



// ������� ����� ��� ������� �������� �������
class gameObjectBase {

 public:
    gameObjectBase(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef health) :
		_X(x),
		_Y(y),
		_Angle(angle),
		_Speed(speed),
		_Scale(scale),
        _Health(health),
        _HealthMax(health)
	{}
    virtual ~gameObjectBase() {}

    // --- ������� ������, ������� �� ���������������� � �������-����������� ---
    inline const float& getPosX   () const    {    return _X;   }
	inline const float& getPosY   () const    {    return _Y;   } 
    inline const float& getAngle  () const    { return _Angle;  }
	inline const float& getScale  () const    { return _Scale;  }
    inline const int  & getHealth () const    { return _Health; }
    inline const bool & isAlive   () const    { return _Alive;  }

    inline void  setPosX   (const float &x)   {        _X = x;  }
    inline void  setPosY   (const float &y)   {        _Y = y;  }
    inline void  setAngle  (const float &a)   {    _Angle = a;  }
	inline void  setScale  (const float &s)   {    _Scale = s;  }
    inline void  setHealth (const int   &h)   {   _Health = h;  }
    inline void  setAlive  (const bool  &b)   {    _Alive = b;  }

    // ���������� �������� ��������� ��� ���������� ������ (��������). ����� ����� ����������� �������� ����������, ���� �� ����� ����������� ������ ����������
    // http://stackoverflow.com/questions/6404160/sort-a-stdlistmyclass-with-myclassoperatormyclass-other
    inline const bool operator < (const gameObjectBase &other) const {
        return this == &other ? false : _X < other._X;
    }

    inline static void setThreadPool(ThreadPool *thPool) { _thPool = thPool; }

    // --- ����������� ������, ���������� ��� ������� ������-������� ---
    virtual void Move(cfRef = 0, cfRef = 0, void* = 0) = 0;     // ����� ��� ����������� �������, ���������� � ����� �����
    inline virtual cuiRef getAnimPhase() const         = 0;     // ����� ��� ��������� ������� ���� �������� �������

 protected:
    bool            _Alive;
    float           _X, _Y;
    float           _Speed;
    float           _Angle;
	float		    _Scale;
    unsigned int    _Health, _HealthMax;

    static ThreadPool *_thPool;		// ��������� ��� �������, �� ������� ����� ����������� ������������� ����������
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �� ����� ���������� ���������, ������� ����� ������������� �� ������/��������.
// �������� ������� enum � ��������� �����, ����� ����� ����������� ������������ ������������� ���� Bonus::Effects::totalQty.
// ��� ���������� ������ ��� ���� ����������, �.�. ��������������� ���������� � ������ � enum �� ��������, � ������ ������� ����� Bonus ���� ������ Player
// ���� �� �������, �.�. ����� � ������ Bonus �������� ��������� �� Player, � ����� �� ��������������� ���������� Player �� ������� 0_o

// ??? - ������, ������ ���� � ���, ��� ����� ���� ���������� ������ ��� ������, � ����� ��� ��
class BonusEffects {
 public:
    // ��������� ��������� ������ ������ ���� _totalEffectsQty: ��� �� ������ ����� ������� ���������� ���������� ��������
    static enum Effects { HEAL = 0, FREEZE, SHIELD, FIRE_BULLETS, SLOW, _totalEffectsQty };
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �� ����� ������ ������, ���������� ������
class BonusWeapons {
 public:
    // PISTOL ������ ������ ���� ����� ������, �� ���� ��������� ������
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, _lastWeapon, _totalWeaponsQty = _lastWeapon - PISTOL };
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Player : public gameObjectBase {

 public:
    Player(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef interval, cuiRef anim_Qty, HighPrecisionTimer *timer)
		: gameObjectBase(x, y, scale, angle, speed, PLAYER_DEFAULT_HEALTH),
		    _Angle0(angle),
            _appTimer(timer),
            _bulletsType(0),
            _bulletsType_old(12345)
	{
        for (unsigned int i = 0; i < BonusEffects::Effects::_totalEffectsQty; i++)
            EffectsCounters[i] = 0;
    }

   ~Player() {}

    enum BulletsType { NORMAL, ION, FREEZE, FIRE, PIERCING };

    virtual void Move(cfRef = 0, cfRef = 0, void* = nullptr);

    virtual inline cuiRef getAnimPhase() const { return 0; }

    inline void setDirectionL(const bool &l) { _Left  = l; }
    inline void setDirectionR(const bool &r) { _Right = r; }
    inline void setDirectionU(const bool &u) { _Up    = u; }
    inline void setDirectionD(const bool &d) { _Down  = d; }

    void setEffect(const unsigned int &);

    inline void setShieldedMode    (const bool &mode)        { _isShielded   = mode;         }
    inline void setBulletsType_On  (const BulletsType &mode) { _bulletsType |=  (1 << mode); }
    inline void setBulletsType_Off (const BulletsType &mode) { _bulletsType &= ~(1 << mode); }
    inline void resetBulletsType   ()                        { _bulletsType  = 0;            }
    inline unsigned short getBulletsType() const             {          return _bulletsType; }
    inline const bool& isShielded() const                    {           return _isShielded; }

    inline const UINT& getWeaponDelay() const                {          return _weaponDelay; }
    inline const UINT& getBulletSpeed() const                {    return _weaponBulletSpeed; }
    inline const UINT& getWeaponBurst() const                {    return _weaponBurstMode;   }

    inline const bool& isWeaponReady()                       { if( !(_weaponReady % _weaponDelay) ) { _weaponReady = 0; return true; } return false; }

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// ����, ������� �������� � �����������. ��������� ��������� ������, ���� �� ���������� ���������� �� ��� ��� �����, ��� �� �����

    HighPrecisionTimer *_appTimer;                                              // ��������� �� ����� ������ ����������
    unsigned int EffectsCounters[BonusEffects::Effects::_totalEffectsQty];      // ������ ��������� ������������ ��������

    // �������� �������
    bool _isShielded;
    // ������� ����. ��������������� �� ������ (�������� ��� �� ������) � ��� ��������� ���� ��������������� �� ������ ���� � �����������
    unsigned short _bulletsType, _bulletsType_old;

    unsigned int _weaponDelay, _weaponBulletSpeed, _weaponBurstMode, _weaponReady;
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Monster : public gameObjectBase {

 public:
    Monster(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef interval, cuiRef anim_Qty)
        : gameObjectBase(x, y, scale, angle, speed, MONSTER_DEFAULT_HEALTH),
            animInterval0(interval),
            animInterval1(interval),
            animQty(anim_Qty),
            animPhase(0)
	{}
   ~Monster() {}

    virtual void Move(cfRef x, cfRef y, void *Param);

	virtual inline cuiRef getAnimPhase() const { return animPhase; }

    static inline void setFreeze(const bool &Freeze) { _freezeEffect = Freeze;  }   // Bonus - ��������� ��������

 private:
	 int            animInterval0, animInterval1;
	 unsigned int   animQty, animPhase;

     static bool    _freezeEffect;                  // ����� ���������� � true, ��� ������� �������������� �� �����
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ����
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef, UINT);
   ~Bullet() {}

    // ����� ��� ��������� �������� _scrWidth � _scrHeight, �� ��������� ������� ���� ����� ��������
    static void setScrSize(cuiRef width, cuiRef height) {
        unsigned int addedSize = 50;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline       cuiRef getAnimPhase() const { return   0; }
            inline const float& getX0()        const { return _X0; }
            inline const float& getY0()        const { return _Y0; }

    // ������������ �������� ����, ������������ �� � �������� ��� ����� ����������
    // ���������� ����, ���� ������������ �� ����������, ��� ������� �������� ������, ���� ������������ ���������
    virtual void Move(cfRef, cfRef, void *);

    inline       void          setBulletType(const unsigned int &type)  { _bulletType = type; }
    inline const unsigned int& getBulletType()                          { return _bulletType; }
    inline       void          setPiercing(const bool &mode)            {   _piercing = mode; }
    inline const bool        & isPiercing()                             {   return _piercing; }

 private:

    // ����������� ������� � �����������
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

    // ��������� ������� ��������� ���� � ��������
    void threadMove(void *);

 private:
    float   _X0, _Y0;               // ����������� �����, �� ������� ���� �����
    float   _dX, _dY;               // �������� �� x � �� y ��� ���������� ����� ������� ����

    float   dx, dy, a, b, c;        // ���������� ��� ���������� ����������� ���� � ��������, ����� �� ��������� �� ������ ��� � ���� �������
                                    // ??? - ���� �� ���������, ����� � ����� ���� ����������� � ���� �������?.. - ����� ������ ������� ���...

    int _squareX0, _squareY0,       // ���������� ��������, ���������� ������ ������� �������� ���� � ����� ��������
        _squareX1, _squareY1,
        _monsterX, _monsterY,
        _squareSide;

    static int _scrWidth;           // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������
    static int _scrHeight;          // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������

    UINT _bulletType;               // 0 - ������� ����, 1 - ��������, 2 - ������, 3 - ����������, 4 - ����������, 5 - ��������������
    bool _piercing;                 // false - ���� ��� ��������� � ������� ��������, true - ���� ����������� �������, ����� ����� ����� ����� (true ��� �������� ����� � ��������� ������)
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Bonus : public gameObjectBase, public BonusEffects {

 public:
	Bonus(cfRef x, cfRef y, const Effects &effect)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f, 1),
          BonusEffects(),
            _LifeTime(500),
            _Effect(effect),
            _AngleCounter(rand()%10),
            _ScaleCounter(0)
	{}
   ~Bonus() {}

    // ��� ������ ������������ ���������� ����� �����, �������, ������� � �������������� � �������
    virtual inline void Move(cfRef x, cfRef y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // ����� ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
        // ��� ���� ����� �������, � ����� �������� ����� ������
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Effect);
            _Alive = false;
            return;
        }

        // ������� ������� �������� ������
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        _Angle = 0.5f * sin(_AngleCounter);
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
   
        if( --_LifeTime <= 0 )
		    _Alive = false;

        return;
    }

    // � �������� ������ �������� ������ ������ ����� ������� � ��� ����� ���������� ���� � �� �� �����������
    virtual inline cuiRef getAnimPhase() const { return _Effect; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Effect;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Weapon : public gameObjectBase, public BonusWeapons {

 public:
	Weapon(cfRef x, cfRef y, const Weapons &weapon)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f, 1),
          BonusWeapons(),
            _LifeTime(5500),
            _Weapon(weapon),
            _AngleCounter(rand()%10),
            _ScaleCounter(0)
	{}
   ~Weapon() {}

    // ��� ������-������ ������������ ����� �����, �������, ������� � �������������� � �������
    virtual inline void Move(cfRef x, cfRef y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // �����-������ ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
        // ��� ���� �����-������ �������, � ����� �������� ����� �����
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Weapon);
            _Alive = false;
            return;
        }

        // ������� ������� �������� ������
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        //_Angle = 0.5f * sin(_AngleCounter);

        _Angle = 0.5f * sin(_AngleCounter) + 45.0f * 3.14f / 180;

        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f : 2.0f + 0.1f * sin(_ScaleCounter);

        if( --_LifeTime <= 0 )
		    _Alive = false;

        return;
    }

    // � �������� ������ �������� ������ ������ ����� ������� � ��� ����� ���������� ���� � �� �� �����������
    virtual inline cuiRef getAnimPhase() const { return _Weapon - Weapons::PISTOL; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Weapon;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

#endif
