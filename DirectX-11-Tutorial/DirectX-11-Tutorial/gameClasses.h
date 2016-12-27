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

struct OlegCell {
    std::vector<gameObjectBase*> cellList;      // ������, � ������� ����� ���������� �������, �������� � ������
    std::mutex                   cellMutex;     // ������� ��� ������������ ������� cellList �� ������/��������
    unsigned int                 cellId;        // ���������� id ��� �����
};

class gameCells {

 public:
    gameCells() {

        if( _Single )
            exit(EXIT_FAILURE);
        else
            _Single = true;
    }

   ~gameCells() {
        delete VEC;
    }

    void Init(int bgrWidth, int bgrHeight, int cellSide = 10) {

        // ��������� ���������
        _lowX = 0;
        _lowY = 0;
        _maxX = bgrWidth;
        _maxY = bgrHeight;

        _widthPixels  = abs(_maxX - _lowX);
        _heightPixels = abs(_maxY - _lowY);

        // ����� ����� ��������� ���������� ������
        _cellSide = cellSide;
        _cellSideInverted = 1.0f / _cellSide;

        _widthCells  = _widthPixels  * _cellSideInverted;
        _heightCells = _heightPixels * _cellSideInverted;

        // �������� ������ � �������� � �������� ���� ������� ���������� id
        VEC = new std::vector<OlegCell>(_widthCells * _heightCells);

        for (unsigned int i = 0; i < _widthCells * _heightCells; i++)
            (*VEC)[i].cellId = i;   

        return;
    }

    // ������������� �������� ���������� �� �������������� ������ � ���������� ��������������� ������
    inline OlegCell& operator() (const int &posx, const int &posy) {
        return (*VEC)[_widthCells * posy + posx];
    }

    // �������� �� ������ 2 ���������� � ���������� � ��� ���������� ������ � �����
    inline void getCellCoordinates(int &x, int &y) {
        x *= _cellSideInverted;
        y *= _cellSideInverted;
    }

    inline int getDist_inCells(const int &dist) {
        return dist * _cellSideInverted;
    }

    inline int getCellId_withCoords(const int &x, const int &y) {
        return _widthCells * y + x;
    }

    // ���������, ����� ������������� ������ ���� �� ������� �� ������� ����� �����
    // ���� �� ����� ������� (���� ��������� �������� � ���� �����) - ���������� ���������� ���, ����� �� �� �������
    inline void checkCoordinates(int &x0, int &y0, int &x1, int &y1) {

        static int _lowX_Cells = getDist_inCells(_lowX);
        static int _lowY_Cells = getDist_inCells(_lowY);
        static int _maxX_Cells = getDist_inCells(_maxX);
        static int _maxY_Cells = getDist_inCells(_maxY);

        if( x0 < _lowX_Cells ) x0 = _lowX_Cells;
        if( y0 < _lowY_Cells ) y0 = _lowY_Cells;
        if( x1 > _maxX_Cells ) x1 = _maxX_Cells - 1;
        if( y1 > _maxY_Cells ) y1 = _maxY_Cells - 1;
    }

    // ����������� ������� � ������, ������� �� ����� ��������
    void UpdateGameCells(Monster *, const int &, const int &, const int &, const int &, float test);

 private:
    // ��������� ����������� � ������������
    gameCells(const gameCells &);
    gameCells& operator=(gameCells);

 private:
    short _lowX, _lowY, _maxX, _maxY;               // ����������, ��� ������� �������� �����
    unsigned short _cellSide;                       // ������ ������ �����
    float          _cellSideInverted;               // 1/_cellSide;
    unsigned short _widthPixels, _heightPixels;     // ������� ���� � ��������
    unsigned short _widthCells,  _heightCells;      // ������� ���� � �������
    static   bool  _Single;                         // ��� ����� ����� 1 ��������� ������, ��� ��� ������ �� ���� ������� ��������

    std::vector<OlegCell> *VEC;
};
// ------------------------------------------------------------------------------------------------------------------------



// ������� ����� ��� ������� �������� �������
class gameObjectBase {

 public:
    gameObjectBase(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef health) :
		_X(x),
		_Y(y),
        _Alive(true),
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

    inline void spawnBullet_Normal   (const int &, const int &, std::list<gameObjectBase*> *, unsigned int &, const int &, const int &);
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
     static bool    _freezeEffect;              // ����� ���������� � true, ��� ������� �������������� �� �����
     unsigned int   _Cell;                      // ����� ������� ������, ����� ��� ������ �������� ��������� ������. ����� ������� � ������ ��������� ��� ������� - �� ���������
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ����
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef, UINT);
   ~Bullet() {}

    // ����� ��� ��������� �������� _scrWidth � _scrHeight, �� ��������� ������� ���� ����� ��������
    static void setScrSize(cuiRef width, cuiRef height) {
        unsigned int addedSize = 100;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline cuiRef getAnimPhase() const { return   0; }
            inline cfRef  getX0()        const { return _X0; }
            inline cfRef  getY0()        const { return _Y0; }

    // ������������ �������� ����, ������������ �� � �������� ��� ����� ����������
    // ���������� ����, ���� ������������ �� ����������, ��� ������� �������� ������, ���� ������������ ���������
    virtual void Move(cfRef wndPosX, cfRef wndPosY, void *Param) {
        //_thPool->runAsync(&Bullet::threadMove_VECT, this, Param);
        _thPool->runAsync(&Bullet::threadMove_Cells, this, wndPosX, wndPosY);
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
    void threadMove_Cells(cfRef, cfRef);

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
        _thPool->runAsync(&BulletIon::threadMove, this);
    }

 private:
    void threadMove();
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Bonus : public gameObjectBase, public BonusEffects {

 public:
	Bonus(cfRef x, cfRef y, const Effects &effect)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f, 1),
          BonusEffects(),
            _LifeTime(1500),    // 500
            _Effect(effect),
            _AngleCounter(rand()%10),
            _ScaleCounter(0)
	{}
   ~Bonus() {}

    // ��� ������ ������������ ���������� ����� �����, �������, ������� � �������������� � �������
    virtual void Move(cfRef wndPosX, cfRef wndPosY, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // ����� ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
        // ��� ���� ����� �������, � ����� �������� ����� ������
        if( abs(_X - player->getPosX() + wndPosX) < 33 && abs(_Y - player->getPosY() + wndPosY) < 30 ) {

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
            _WeaponReduced(weapon - Weapons::PISTOL),
            _AngleCounter(rand()%10),
            _ScaleCounter(0)
	{}
   ~Weapon() {}

    // ��� ������-������ ������������ ����� �����, �������, ������� � �������������� � �������
    virtual void Move(cfRef wndPosX, cfRef wndPosY, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // �����-������ ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
        // ��� ���� �����-������ �������, � ����� �������� ����� �����
        if( abs(_X - player->getPosX() + wndPosX) < 33 && abs(_Y - player->getPosY() + wndPosY) < 30 ) {

            player->setEffect(_Weapon);
            _Alive = false;
            return;
        }

        // ������������ �������� ������
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
    // ����� �������� ������ �������������� ����, �.�. ��� �������� ��� (weapon - Weapons::PISTOL), � �� ������ ������ ������� ����������� ��������
    virtual inline cuiRef getAnimPhase() const { return _WeaponReduced; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Weapon, _WeaponReduced;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



void Player::spawnBullet_Normal(const int &mouseX, const int &mouseY, std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize,
                                    const int &wndPosX, const int &wndPosY)
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
                bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                    mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                        _weaponBulletSpeed*5, Player::BulletsType::FIRE) );

            bulletListSize += _weaponBurstQty;
        }

        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new BulletIon(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::FIRE )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::FIRE) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::PIERCING )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::PIERCING) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    for (unsigned int i = 0; i < _weaponBurstQty; i++)
        bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
            mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
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

#endif
