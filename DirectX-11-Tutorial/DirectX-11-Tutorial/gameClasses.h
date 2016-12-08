#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

// ------------------------------------------------------------------------------------------------------------------------



// ������� ����� ��� ������� �������� �������
class gameObjectBase {

 public:
    gameObjectBase(const float &x, const float &y, const float &scale, const float &angle, const float &speed) :
		_X(x),
		_Y(y),
		_Angle(angle),
		_Speed(speed),
		_Scale(scale)
	{}
    virtual ~gameObjectBase() {}

    // --- ������� ������, ������� �� ���������������� � �������-����������� ---
    inline const float& getPosX  () const    {    return _X;  }
	inline const float& getPosY  () const    {    return _Y;  }
    inline const float& getAngle () const    { return _Angle; }
	inline const float& getScale () const    { return _Scale; }
    inline const bool & isAlive  () const    { return _Alive; }

    inline void  setPosX  (const float &x)   {        _X = x; }
    inline void  setPosY  (const float &y)   {        _Y = y; }
    inline void  setAngle (const float &a)   {    _Angle = a; }
	inline void  setScale (const float &s)   {    _Scale = s; }
    inline void  setAlive (const  bool &b)   {    _Alive = b; }

    // --- ����������� ������, ���������� ��� ������� ������-������� ---
    virtual int Move(const float & = 0, const float & = 0, void* = 0) = 0;      // ����� ��� ����������� �������, ���������� � ����� �����
    inline virtual const unsigned int& getAnimPhase() const = 0;                // ����� ��� ��������� ������� ���� ��������

    inline static void setThreadPool(ThreadPool *thPool) {
        _thPool = thPool;
    }

 protected:
    bool        _Alive;
    float       _X, _Y;
    float       _Speed;
    float       _Angle;
	float		_Scale;

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
    // ��������� ������� _totalQty ������ ����� ������� ���������� ���������� ��������
    static enum Effects { HEAL = 0, FREEZE, SHIELD, SLOW, _totalQty };
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Player : public gameObjectBase {

 public:
    Player(const float &x, const float &y, const float &scale, const float &angle, const float &speed,
            const unsigned int &interval, const unsigned int &anim_Qty, HighPrecisionTimer *timer)
		: gameObjectBase(x, y, scale, angle, speed),
		    _Angle0(angle),
            appTimer(timer)
	{
        for (unsigned int i = 0; i < BonusEffects::Effects::_totalQty; i++)
            EffectsCounters[i] = 0;
    }

   ~Player() {}

    // ���� ��� �� ���������� ������������ ��������, ��� �������������� �����-��, � ������ ����� ���������� ������� isAlive
    virtual int Move(const float & = 0, const float & = 0, void* = nullptr);

    virtual inline const unsigned int& getAnimPhase() const { return 0; }

    inline void setDirectionL(const bool &l) { _Left  = l; }
    inline void setDirectionR(const bool &r) { _Right = r; }
    inline void setDirectionU(const bool &u) { _Up    = u; }
    inline void setDirectionD(const bool &d) { _Down  = d; }

    void setEffect(const unsigned int &);

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// ����, ������� �������� � �����������. ��������� ��������� ������, ���� �� ���������� ���������� �� ��� ��� �����, ��� �� �����

    HighPrecisionTimer *appTimer;                                       // ��������� �� ����� ������ ����������
    unsigned int EffectsCounters[BonusEffects::Effects::_totalQty];     // ������ ��������� ������������ ��������
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Monster : public gameObjectBase {

 public:
    Monster(const float &x, const float &y, const float &scale, const float &angle, const float &speed, const unsigned int &interval, const unsigned int &anim_Qty)
        : gameObjectBase(x, y, scale, angle, speed),
            animInterval0(interval),
            animInterval1(interval),
            animQty(anim_Qty),
            animPhase(0)
	{}
   ~Monster() {}

    virtual int Move(const float &x, const float &y, void *Param);

	virtual inline const unsigned int& getAnimPhase() const { return animPhase; }

 private:
	 int            animInterval0, animInterval1;
	 unsigned int   animQty, animPhase;
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ����
class Bullet : public gameObjectBase {

 public:
    Bullet(const float &, const float &, const float &, const float &, const float &, const float &);
   ~Bullet() {}

    // ����� ��� ��������� �������� _scrWidth � _scrHeight, �� ��������� ������� ���� ����� ��������
    static void setScrSize(unsigned int width, unsigned int height) {
        unsigned int addedSize = 50;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline const unsigned int& getAnimPhase() const { return   0; }
            inline const float&        getX0()        const { return _X0; }
            inline const float&        getY0()        const { return _Y0; }

    // ������������ �������� ����, ������������ �� � �������� ��� ����� ����������
    // ���������� ����, ���� ������������ �� ����������, ��� ������� �������� ������, ���� ������������ ���������
    virtual int Move(const float &, const float &, void *);

 private:

    // ����������� ������� � �����������
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

    void threadMove(std::vector< std::list<gameObjectBase*>* > *);

 private:
    float   _X0, _Y0;               // ����������� �����, �� ������� ���� �����
    float   _dX, _dY;               // �������� �� x � �� y ��� ���������� ����� ������� ����

    float   dx, dy, a, b, c;        // ���������� ��� ���������� ����������� ���� � ��������, ����� �� ��������� �� ������ ��� � ���� �������
                                    // ??? - ���� �� ���������, ����� � ����� ���� ����������� � ���� �������?.. - ����� ������ ������� ���...

    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide;

    static int _scrWidth;           // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������
    static int _scrHeight;          // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������

	// new test
	unsigned int hitCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Bonus : public gameObjectBase, public BonusEffects {

 public:
	Bonus(const float &x, const float &y, const Effects &effect)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f),
          BonusEffects(),
            _LifeTime(500),
            _Effect(effect),
            _AngleCounter(rand()%10),
            _ScaleCounter(0)
	{}
   ~Bonus() {}

    // ��� ������ ������������ ����� �����, �������, ������� � �������������� � �������
    virtual inline int Move(const float &x, const float &y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // ����� ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
        // ��� ���� ����� �������, � ����� �������� ����� ������
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Effect);
            _Alive = false;
            return 1;
        }

        // ������� ������� �������� ������
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        _Angle = 0.5f * sin(_AngleCounter);
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
   
        if( --_LifeTime <= 0 )
		    _Alive = false;

        return 0;
    }

    // � �������� ������ �������� ������ ������ ����� ������� � ��� ����� ���������� ���� � �� �� �����������
    virtual inline const unsigned int& getAnimPhase() const { return _Effect; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Effect;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

#endif
