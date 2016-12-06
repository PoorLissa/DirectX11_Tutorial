#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
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
    inline float getPosX  () const           { return _X;     } 
    inline float getPosY  () const           { return _Y;     }
    inline float getAngle () const           { return _Angle; }
	inline float getScale () const           { return _Scale; }
    inline bool  isAlive  () const           { return _Alive; }

    inline void  setPosX  (const float &x)   {        _X = x; }
    inline void  setPosY  (const float &y)   {        _Y = y; }
    inline void  setAngle (const float &a)   {    _Angle = a; }
	inline void  setScale (const float &s)   {    _Scale = s; }
    inline void  setAlive (const  bool &b)   {    _Alive = b; }

    // --- ����������� ������, ���������� ��� ������� ������-������� ---
    virtual int Move(const float & = 0, const float & = 0, void* = 0) = 0;      // ����� ��� ����������� �������, ���������� � ����� �����
    inline virtual int getAnimPhase() const = 0;                                // ����� ��� ��������� ������� ���� ��������

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



// ����� �������� ������� - �����
class Player : public gameObjectBase {

 public:
    Player(const float &x, const float &y, const float &scale, const float &angle, const float &speed, const int &interval, const int &anim_Qty)
		: gameObjectBase(x, y, scale, angle, speed),
		_Angle0(angle)
	{}
   ~Player() {}

    // ���� ��� �� ���������� ������������ ��������, ������ ���� ���������� ������� isAlive
    virtual int Move(const float & = 0, const float & = 0, void* = nullptr);

    inline virtual int getAnimPhase() const  {    return 0; }

    void inline setDirectionL(const bool &b) { _Left  = b; }
    void inline setDirectionR(const bool &b) { _Right = b; }
    void inline setDirectionU(const bool &b) { _Up    = b; }
    void inline setDirectionD(const bool &b) { _Down  = b; }

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// ����, ������� �������� � �����������. ��������� ��������� ������, ���� �� ���������� ���������� �� ��� ��� �����, ��� �� �����
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - ������
class Monster : public gameObjectBase {

 public:
    Monster(const float &x, const float &y, const float &scale, const float &angle, const float &speed, const int &interval, const int &anim_Qty)
        : gameObjectBase(x, y, scale, angle, speed),
            animInterval0(interval),
            animInterval1(interval),
            animQty(anim_Qty),
            animPhase(0)
	{}
   ~Monster() {}

    virtual int Move(const float &x, const float &y, void *Param);

	inline virtual int getAnimPhase() const { return animPhase; }

 private:
	 int animInterval0, animInterval1;
	 int animQty, animPhase;
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

    inline virtual int   getAnimPhase() const { return 0;   }
    inline         float getX0()        const { return _X0; }
    inline         float getY0()        const { return _Y0; }

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
                                    // ??? - ���� �� ���������, ����� � ����� ���� ����������� � ���� �������?..

    static int _scrWidth;           // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������
    static int _scrHeight;          // �������� ���������, �� ��������� ������� ���� ��������� ������� � ������

	// new test
	unsigned int hitCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



// ����� �������� ������� - �����
class Bonus : public gameObjectBase {

 public:
	Bonus(const float &x, const float &y, const float &scale)
		: gameObjectBase(x, y, scale, 0.0f, 0.0f), lifeTime(500)
	{}
   ~Bonus() {}

    // ��� ������ ���� ��� ������ ������������ ����� �����
    virtual inline int Move(const float &, const float &, void *) {
   
        if( !--lifeTime )
		    _Alive = false;
    }

 private:

 private:
	 unsigned int lifeTime;
};
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

#endif
