#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include <vector>
#include <list>
#include <iterator>

// ------------------------------------------------------------------------------------------------------------------------



// Базовый класс для всякого игрового объекта
class gameObjectBase {

 public:
    gameObjectBase(const float &x, const float &y, const float &angle, const float &speed) : _X(x), _Y(y), _Angle(angle), _Speed(speed) {}
    virtual ~gameObjectBase() {}

    // --- Базовые методы, которые не переопределяются в классах-наследниках ---
    inline float getPosX  () const           { return _X;     } 
    inline float getPosY  () const           { return _Y;     }
    inline float getAngle () const           { return _Angle; }
    inline bool  isAlive  () const           { return _Alive; }
    inline void  setPosX  (const float &x)   {        _X = x; }
    inline void  setPosY  (const float &y)   {        _Y = y; }
    inline void  setAngle (const float &a)   {    _Angle = a; }
    inline void  setAlive (const  bool &b)   {    _Alive = b; }

    // --- Виртуальные методы, уникальные для каждого класса-потомка ---
    virtual int Move(const float & = 0, const float & = 0, void* = 0) = 0;      // метод для перемещения объекта, вызывается в общем цикле
    inline virtual int getAnimPhase() const = 0;                                // метод для получения текущей фазы анимации

 protected:
    bool    _Alive;
    float   _X, _Y;
    float   _Speed;
    float   _Angle;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Игрок
class Player : public gameObjectBase {

 public:
    Player(const float &x, const float &y, const float &angle, const float &speed, const int &interval, const int &anim_Qty) : gameObjectBase(x, y, angle, speed) {}
   ~Player() {}

    // Пока что не используем возвращаемое значение, вместо него пользуемся методом isAlive
    virtual int Move(const float & = 0, const float & = 0, void* = nullptr);

    inline virtual int getAnimPhase() const  {    return 0; }

    void inline setDirectionL(const bool &b) { _Left  = b; }
    void inline setDirectionR(const bool &b) { _Right = b; }
    void inline setDirectionU(const bool &b) { _Up    = b; }
    void inline setDirectionD(const bool &b) { _Down  = b; }

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Монстр
class Monster : public gameObjectBase {

 public:
    Monster(const float &x, const float &y, const float &angle, const float &speed, const int &interval, const int &anim_Qty) : gameObjectBase(x, y, angle, speed),
        animInterval0(interval), animInterval1(interval), animQty(anim_Qty), animPhase(0) {
    }

   ~Monster() {}

    virtual int Move(const float &x, const float &y, void *Param);

	inline virtual int getAnimPhase() const { return animPhase; }

 private:
	 int animInterval0, animInterval1;
	 int animQty, animPhase;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Пуля
class Bullet : public gameObjectBase {

 public:
    Bullet(const int &, const int &, const int &, const int &, const float & = 1.0);

   ~Bullet() {}

    // Метод для установки значений _scrWidth и _scrHeight, за пределами которых пули будут исчезать
    static void setScrSize(unsigned int width, unsigned int height) {
        unsigned int addedSize = 50;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    inline virtual int   getAnimPhase() const { return 0;   }
    inline         float getX0()        const { return _X0; }
    inline         float getY0()        const { return _Y0; }

    // просчитываем движение пули, столкновение ее с монстром или конец траектории
    // возвращаем ноль, если столкновения не происходит, или счетчик анимации взрыва, если столкновение произошло
    virtual int Move(const float &, const float &, void *);

 private:

    // пересечение отрезка с окружностью ( http://www.cyberforum.ru/cpp-beginners/thread853799.html )
    bool commonSectionCircle(double, double, double, double, const double &, const double &, const double &);

 private:
    float   _X0, _Y0;               // изначальная точка, из которой пуля летит
    float   dX, dY;                 // смещения по x и по y для нахождения новой позиции пули

    float   dx, dy, a, b, c;        // переменные для вычисления пересечения пули с монстром, чтобы не объявлять их каждый раз в теле функции

    static int _scrWidth;           // Значения координат, за пределами которых пуля считается ушедшей в молоко
    static int _scrHeight;          // Значения координат, за пределами которых пуля считается ушедшей в молоко
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Бонус
class Bonus : public gameObjectBase {

 public:
	Bonus(const int &x, const int &y) : gameObjectBase(x, y, 0.0f, 0.0f), lifeTime(500) {}
   ~Bonus() {}

   // Для бонуса пока что просто рассчитываем время жизни
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
