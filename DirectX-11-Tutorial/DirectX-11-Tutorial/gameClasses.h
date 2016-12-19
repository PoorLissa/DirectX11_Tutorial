#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

#define cfRef  const float &
#define ciRef  const int   &
#define cuiRef const unsigned int &

// �������������� ��������� ������
class gameObjectBase;
class Player;
class Bullet;
class Monster;
class Bonus;
class Weapon;

#define olegMaxX 800
#define olegMaxY 600

typedef std::vector<gameObjectBase*> olegType;

struct OlegType {
    std::vector<gameObjectBase*> cellList;      // ������, � ������� ����� ���������� �������, �������� � ������
    std::mutex                   cellMutex;     // ������� ��� ������������ ������� cellList �� ������/��������
    unsigned int                 cellId;        // ���������� id ��� �����
};

class gameCells {

 public:
    gameCells() {

        if( Single ) {
            exit(EXIT_FAILURE);
        }
        else {
            // ��������� ���������
            _lowX = 0;
            _lowY = 0;
            _maxX = 800;
            _maxY = 600;

            _widthPixels  = abs(_maxX - _lowX);
            _heightPixels = abs(_maxY - _lowY);

            // ����� ����� ��������� ���������� ������
            _cellSide = 10;

            _widthCells  = _widthPixels  / _cellSide;
            _heightCells = _heightPixels / _cellSide;

            VEC = new std::vector<OlegType>(_widthCells * _heightCells);

            // �������� ���� ������� ���������� id
            for (unsigned int i = 0; i < _widthCells * _heightCells; i++)
                VEC->at(i).cellId = i;

            Single = true;
        }
    }

   ~gameCells() {
        delete VEC;
    }

    // ������������� �������� ���������� �� �������������� ������ � ���������� ��������������� ������
    inline OlegType& operator() (const int posx, const int posy) {

        unsigned short cellX = posx / _cellSide;
        unsigned short cellY = posy / _cellSide;

        return VEC->at( _widthCells * cellY + cellX );
    }

    // �������� �� ������ 2 ���������� � ���������� � ��� ���������� ������ � �����
    inline void getCellCoordinates(int &x, int &y) {
        x = x / _cellSide;
        y = y / _cellSide;
    }

    inline int getDist_inCells(const int &dist) {
        return dist / _cellSide;
    }

 private:
    gameCells(const gameCells &);
    gameCells& operator=(gameCells);

 private:
    short _lowX, _lowY, _maxX, _maxY;               // ����������, ��� ������� �������� �����
    unsigned short _cellSide;                       // ������ ������ �����
    unsigned short _widthPixels, _heightPixels;     // ������� ���� � ��������
    unsigned short _widthCells, _heightCells;       // ������� ���� � �������

    std::vector<OlegType> *VEC;

    static bool Single;

};
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

    // ����� ��� ��������� ������� � �������� ������ ������, ���������� � ����� �����. ����� void* ����� ��������� �� ���� ����� ��������� ���������
    virtual void Move(cfRef = 0, cfRef = 0, void* = nullptr) = 0;
    // ����� ��� ��������� ������� ���� �������� �������
    inline virtual cuiRef getAnimPhase() const = 0;

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
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, ION_GUN, _lastWeapon, _totalWeaponsQty = _lastWeapon - PISTOL };
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

    inline void spawnBullet_Normal   (const int &, const int &, std::list<gameObjectBase*> *, unsigned int &);
    inline void spawnBullet_FourSides(std::list<gameObjectBase*> *, unsigned int &);

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// ����, ������� �������� � �����������. ��������� ��������� ������, ���� �� ���������� ���������� �� ��� ��� �����, ��� �� �����

    HighPrecisionTimer *_appTimer;                                              // ��������� �� ����� ������ ����������
    unsigned int EffectsCounters[BonusEffects::Effects::_totalEffectsQty];      // ������ ��������� ������������ ��������

    // �������� �������
    bool _Shielded;

    // ������� ����. ��������������� �� ������ (�������� ��� �� ������) � ��� ��������� ���� ��������������� �� ������ ���� � �����������
    unsigned short _bulletsType, _bulletsType_old;

    unsigned int _weaponDelay, _weaponBulletSpeed, _weaponBurstQty, _weaponReady, _weaponBulletSpread;
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

    virtual void Move(cfRef, cfRef, void *);

    void threadMove(cfRef x, cfRef y, void *Param);

	virtual inline cuiRef getAnimPhase() const { return animPhase; }

    static inline void setFreeze(const bool &mode) { _freezeEffect = mode;  }   // Bonus - ��������� ��������

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
    virtual void Move(cfRef, cfRef, void *Param) {
        //_thPool->runAsync(&Bullet::threadMove_VECT, this, Param);
        threadMove_Oleg(Param);
    }

    inline       void          setBulletType(const unsigned int &type)  { _bulletType = type; }
    inline const unsigned int& getBulletType()                          { return _bulletType; }
    inline       void          setPiercing(const bool &mode)            {   _piercing = mode; }
    inline const bool        & isPiercing()                             {   return _piercing; }

 protected:
    // ����������� ������� � �����������
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

 private:
    // ��������� ������� ��������� ���� � ��������
    void threadMove_VECT(void *);
    void threadMove_Oleg(void *);

 protected:
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



// ����� �������� ������� - ������ ����
// ������������ ���� ��������� �����, �.�. � �.�. ������� ������ ������� ��������, � �� ������� �������� ������� � ������� ������ Bullet::Move()
class BulletIon : public Bullet {

 public:
    BulletIon(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed)
        : Bullet(x, y, scale, x_to, y_to, speed, Player::BulletsType::ION)
    {}
   ~BulletIon() {}

    virtual void Move(cfRef, cfRef, void *Param) {
        _thPool->runAsync(&BulletIon::threadMove, this, Param);
    }

 private:
    void threadMove(void *);
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
    virtual void Move(cfRef x, cfRef y, void *Param) {

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
    virtual void Move(cfRef x, cfRef y, void *Param) {

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

void Player::spawnBullet_Normal(const int &mouseX, const int &mouseY, std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize)
{
    // �������� ���������� �� ������� �� ����� ������������ � ��������� ���������� � 500px, ��� �������� � ��������� ��������� ����

    short dx     = _X - mouseX;
    short dy     = _Y - mouseY;
    short dist   = sqrt(dx*dx + dy*dy);
    short Spread = dist * _weaponBulletSpread * 0.02;   // ����� �� ������ Spread � ����, ����� �� �� 500, � �� 50, � ��� ���� ����� �������� �������� ��� �� 10
    short halfSpread = Spread * 0.5f;

    // ��� ����������� ���� ��������� ��� ������ �������� &, �.�. � ������ ����� ���� ��������� �������� �����
    // � ����������� ���� �� �������� ���������� �������� �������

    if( _bulletsType & 1 << Player::BulletsType::ION )
    {
        if( _bulletsType & 1 << Player::BulletsType::FIRE ) {
            for (unsigned int i = 0; i < _weaponBurstQty; i++)
                bulletList->push_back( new Bullet(_X, _Y, 1.0f,
                    mouseX + 0.1 * int(rand() % Spread - halfSpread), mouseY + 0.1 * int(rand() % Spread - halfSpread),
                        _weaponBulletSpeed*5, Player::BulletsType::FIRE) );

            bulletListSize += _weaponBurstQty;
        }

        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new BulletIon(_X, _Y, 1.0f,
                mouseX + 0.1 * int(rand() % Spread - halfSpread), mouseY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::FIRE )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X, _Y, 1.0f,
                mouseX + 0.1 * int(rand() % Spread - halfSpread), mouseY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::FIRE) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::PIERCING )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X, _Y, 1.0f,
                mouseX + 0.1 * int(rand() % Spread - halfSpread), mouseY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::PIERCING) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    for (unsigned int i = 0; i < _weaponBurstQty; i++)
        bulletList->push_back( new Bullet(_X, _Y, 1.0f,
            mouseX + 0.1 * int(rand() % Spread - halfSpread), mouseY + 0.1 * int(rand() % Spread - halfSpread),
                _weaponBulletSpeed, Player::BulletsType::NORMAL) );

    bulletListSize += _weaponBurstQty;

    return;
}

void Player::spawnBullet_FourSides(std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize)
{
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X + 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X - 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y + 100, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y - 100, _weaponBulletSpeed, _bulletsType ) );

    bulletListSize += 4;
}

#include <vector>
#include <map>
#include <utility>
#include <string>
#if 0
class BulletHelper {

 public:
    BulletHelper(unsigned int numX, unsigned int numY) {

        numX = 2;
        numY = 2;
    
        for (int i = 0; i < numX * numY; i++)
            VEC.push_back(new std::vector<gameObjectBase*>());
    }

   ~BulletHelper() {
   
       if( VEC.size() ) {
           for (int i = 0; i < VEC.size(); i++)
               delete VEC[i];
       }
    }

    // ������ ��� ������ � ��������������� ������ � ���������� ����� �������
    void Push(gameObjectBase* obj) {

        int x = obj->getPosX();
        int y = obj->getPosY();

        // ���������, �������� �� ����� ������� ��� � Map-�. ���� �� ��� �� ��������, �������� 0.
        // ���� ������ ��� ��������, currentPair ����� ���������: 1. ����� �������, � ������� ��������� ������, 2. ����� ������� � �������
        std::pair<int, int> currentPair(-1, -1);
        int objFound = Map.count(obj);
        int vecNo;

        if( objFound )
            currentPair = Map[obj];

        if( x > 400 )
            if (y > 300)
                vecNo = 3;
            else
                vecNo = 1;
        else
            if( y > 300 )
                vecNo = 2;
            else
                vecNo = 0;

        // ����������� �������� ������� ������� �� ����� ����� �������� � ������, � ����� ������� ������ ��������� �������.
        // ���� ��� ���������� ������� ���������� �� �������, ������ ������ �� ��� ������� ������� � ������� ��� � ����� ������.
        // ������� �������� ��� �������� ������� � Map-�.
        if( currentPair.first != vecNo ) {

            if(currentPair.first >= 0) {

                std::vector<gameObjectBase*>::iterator
                    iter = VEC.at(currentPair.first)->begin(),
                    end  = VEC.at(currentPair.first)->end();

                for ( ; iter != end; ++iter ) {
                    if( *iter == obj ) {
                        VEC.at(currentPair.first)->erase(iter);
                        break;
                    }
                }
            }

            currentPair.first = vecNo;
            VEC.at(vecNo)->push_back(obj);
            currentPair.second = (VEC.at(vecNo))->size() - 1;
            Map[obj] = currentPair;
        }

        return;
    }

    // ������ ��������� �� ������, ���������� ��� �������, ����������� � ������, �� ������� ���������� ��������� ����� (x, y)
    // � �������� ��������� ������� �� ������ �������������� ������� �� ���� �� �������, ������� �� ����� �������� ��� ��������
    std::vector<gameObjectBase*>* getVec(int x, int y) {
    
        int X = x / (800 / _numX);
        int Y = y / (600 / _numY);

        X = X >= _numX ? _numX - 1 : X;
        Y = Y >= _numY ? _numY - 1 : Y;

        int num = X + Y * _numX;

        return VEC[num];
    }

    void logMsg(std::string str) {

	    FILE *f = NULL;

	    fopen_s(&f, "___msgLog__Helper.log", "a");
	    if (f != NULL) {
		    fputs(str.c_str(), f);
    		fputs("\n", f);
	    	fclose(f);
	    }
    }

 private:
    UINT _numX, _numY;                                      // ����� ��������� ������ �� ����������� � �� ���������
    std::vector< std::vector<gameObjectBase*>*    > VEC;    // ������ �������� ��������
    std::map<gameObjectBase*, std::pair<int, int> > Map;    // map, � ������� ��� ������� ������� �������� ����� �������, � ������� �� ��������� � ������ ������
};
#endif


#include <unordered_map>

class BulletHelper {

 public:
    BulletHelper(unsigned int numX, unsigned int numY) {

        numX = 2;
        numY = 2;
    
        for (int i = 0; i < numX * numY; i++)
            MAPS.push_back(new std::unordered_map<int, int>());
    }

   ~BulletHelper() {
       if( MAPS.size() ) {
           for (int i = 0; i < MAPS.size(); i++)
               delete MAPS[i];
       }
    }

    void setThreadPool(ThreadPool *thPool) {
        _thPool = thPool;
    }

    // ������ ��� ������ � ��������������� ������
    void Push(gameObjectBase* obj) {

        unsigned int Obj = (int)obj;

        int x = obj->getPosX();
        int y = obj->getPosY();

        // ���������, �������� �� ����� ������� ��� � Map-�. ���� �� ��� �� ��������, Map.count ���������� 0.
        // ���� ������ ��� ��������, currentPair ����� ���������: 1. ����� �������, � ������� ��������� ������, 2. ����� ������� � �������
        int oldMap = -1, currentMap = -1;

        if( Map.count(Obj) )
            oldMap = Map[Obj];

        if( x > 400 )
            if (y > 300)
                currentMap = 3;
            else
                currentMap = 1;
        else
            if( y > 300 )
                currentMap = 2;
            else
                currentMap = 0;

        // ����������� �������� ������� ������� �� ����� ����� �������� � ������, � ����� ������� ������ ��������� �������.
        // ���� ��� ���������� ������� ���������� �� �������, ������ ������ �� ��� ������� ������� � ������� ��� � ����� ������.
        // ������� �������� ��� �������� ������� � Map-�.
        if( oldMap != currentMap ) {

            if( oldMap >= 0 )
                MAPS.at(oldMap)->erase(Obj);

            (*MAPS.at(currentMap))[Obj] = 0;
            Map[Obj] = currentMap;

            //reinterpret_cast<gameObjectBase*>(Obj)->setAlive(false);
        }

        return;
    }

    void PushThreaded(gameObjectBase* obj) {

        _thPool->runAsync(&BulletHelper::threadMove, this, obj);
    }

    // ������ ��������� �� �����, ���������� ��� �������, ����������� � ������, �� ������� ���������� ��������� ����� (x, y)
    // � �������� ������� ��������� ����� �� ������ �������������� ������� �� ��� �� �������, ������� �� ����� �������� ��� ��������
    std::unordered_map<int, int>* getMap(int x, int y) {
    
        int X = x / (800 / _numX);
        int Y = y / (600 / _numY);

        X = X >= _numX ? _numX - 1 : X;
        Y = Y >= _numY ? _numY - 1 : Y;

        return MAPS.at(X + Y * _numX);
    }

    void logMsg(std::string str) {

	    FILE *f = NULL;

	    fopen_s(&f, "___msgLog__Helper.log", "a");
	    if (f != NULL) {
		    fputs(str.c_str(), f);
    		fputs("\n", f);
	    	fclose(f);
	    }
    }

    // https://habrahabr.ru/post/182610/
    void threadMove(void *Param) {
    
        gameObjectBase* obj = static_cast<gameObjectBase*>(Param);

        unsigned int Obj = (int)obj;

        int x = obj->getPosX();
        int y = obj->getPosY();

        int oldMap = -1, currentMap = -1;

        if( Map.count(Obj) )
            oldMap = Map[Obj];

        if( x > 400 )
            if (y > 300)
                currentMap = 3;
            else
                currentMap = 1;
        else
            if( y > 300 )
                currentMap = 2;
            else
                currentMap = 0;

        if( oldMap != currentMap ) {

            if( oldMap >= 0 ) {

                mapLock[oldMap].lock();

                    MAPS.at(oldMap)->erase(Obj);

                mapLock[oldMap].unlock();
            }

            mapLock[currentMap].lock();

                (*MAPS.at(currentMap))[Obj] = 1;
                Map[Obj] = currentMap;

            mapLock[currentMap].unlock();
        }

        return;
    }

 private:

    std::mutex mapLock[4];

    ThreadPool *_thPool;

    UINT _numX, _numY;                                  // ����� ��������� ������ �� ����������� � �� ���������
    //std::map<int, int>                Map;            // �����, � ������� ��� ������� ������� �������� ����� �����, � ������� �� ��������� � ������ ������
    std::unordered_map<int, int>                Map;
    std::vector< std::unordered_map<int, int>*> MAPS;   // ������ ���� ��������
};


class ListedList {

 public:
    ListedList(unsigned int numX, unsigned int numY) : _numX(numX), _numY(numY) {
        _numX = 2;
        _numY = 2;

        for (int i = 0; i < _numX * _numY; i++)
            _VEC.push_back(new std::list<gameObjectBase*>);
    }
   ~ListedList() {
        for (int i = 0; i < _numX * _numY; i++)
            delete(_VEC[i]);
   }

    void setThreadPool(ThreadPool *thPool) {
        _thPool = thPool;
    }

    inline unsigned int getVecNo_withCoords(const int &x, const int &y) {
    
        if( x > 400 )
            if (y > 300)
                return 3;
            else
                return 1;
        else
            if( y > 300 )
                return 2;
            else
                return 0;
    }

    void runBullet(Bullet *bullet) {
    
    /*
        bullet->coords->getVecNo_withCoords

        get close lying lists

        _thPool->runAsync(...) {

            only for these lists

        }

    */
    }

    void MoveAll() {
    
        /*
            for every list:

                _thPool->runAsync(...) {

                    for every monster in the list: {
                        Monster->Move();

                        if( getVecNo_withCoords(const int &x, const int &y) != currentVecNo )
                            monster -> erase from this list,
                            AddObject(monster); <--- add to another list
                    }
                }
                    
        */
    }

    // ������ ��� ������ � ��������������� ������
    void AddObject(gameObjectBase* obj) {

        unsigned int Obj = (int)obj;

        int x = obj->getPosX();
        int y = obj->getPosY();

        // ���������, �������� �� ����� ������� ��� � Map-�. ���� �� ��� �� ��������, Map.count ���������� 0.
        // ���� ������ ��� ��������, value ����� ��������� ����� ������, � ������� ��������� ������
        int currentVec = getVecNo_withCoords(x, y);

        // ����������� �������� ������� ������� �� ����� ����� �������� � ������, � ����� ������� ������ ��������� �������.
        // ���� ��� ���������� ������� ���������� �� �������, ������ ������ �� ��� ������� ������� � ������� ��� � ����� ������.
        // ������� �������� ��� �������� ������� � Map-�.
        ( *_VEC.at(currentVec) ).push_back(obj);
        //_Map[Obj] = currentVec;

        return;
    }

private:
    //std::unordered_map<int, int>               _Map;
    std::vector<std::list<gameObjectBase*>*>   _VEC;
    UINT                                       _numX, _numY;    // ����� ��������� ������ �� ����������� � �� ���������
    ThreadPool                                *_thPool;
};

#endif
