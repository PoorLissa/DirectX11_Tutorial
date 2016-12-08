#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

// ------------------------------------------------------------------------------------------------------------------------



// Базовый класс для всякого игрового объекта
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

    // --- Базовые методы, которые не переопределяются в классах-наследниках ---
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

    // --- Виртуальные методы, уникальные для каждого класса-потомка ---
    virtual int Move(const float & = 0, const float & = 0, void* = 0) = 0;      // метод для перемещения объекта, вызывается в общем цикле
    inline virtual const unsigned int& getAnimPhase() const = 0;                // метод для получения текущей фазы анимации

    inline static void setThreadPool(ThreadPool *thPool) {
        _thPool = thPool;
    }

 protected:
    bool        _Alive;
    float       _X, _Y;
    float       _Speed;
    float       _Angle;
	float		_Scale;

    static ThreadPool *_thPool;		// указатель пул потоков, на котором будут запускаться многопоточные вычисления
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс со всеми возможными эффектами, которые могут накладываться на игрока/монстров.
// Пришлось вынести enum в отдельный класс, чтобы иметь возможность пользоваться конструкциями вида Bonus::Effects::totalQty.
// Без отдельного класса это было невозможно, т.к. предварительное объявление в случае с enum не работает, а просто вынести класс Bonus выше класса Player
// тоже не удалось, т.к. тогда в классе Bonus ломается указатель на Player, и опять же предварительное объявление Player не спасает 0_o

// ??? - короче, ошибка была в том, что нужно было определять методы вне класса, и тогда все ОК
class BonusEffects {
 public:
    // Последний элемент _totalQty всегда будет хранить актуальное количество эффектов
    static enum Effects { HEAL = 0, FREEZE, SHIELD, SLOW, _totalQty };
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Игрок
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

    // Пока что не используем возвращаемое значение, как предполагалось когда-то, а вместо этого пользуемся методом isAlive
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
	float	_Angle0;	// Угол, который передаем в конструктор. Позволяет довернуть спрайт, если он изначально расположен не под тем углом, как мы хотим

    HighPrecisionTimer *appTimer;                                       // Указатель на общий таймер приложения
    unsigned int EffectsCounters[BonusEffects::Effects::_totalQty];     // Массив счетчиков длительности эффектов
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Монстр
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



// Класс игрового объекта - Пуля
class Bullet : public gameObjectBase {

 public:
    Bullet(const float &, const float &, const float &, const float &, const float &, const float &);
   ~Bullet() {}

    // Метод для установки значений _scrWidth и _scrHeight, за пределами которых пули будут исчезать
    static void setScrSize(unsigned int width, unsigned int height) {
        unsigned int addedSize = 50;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline const unsigned int& getAnimPhase() const { return   0; }
            inline const float&        getX0()        const { return _X0; }
            inline const float&        getY0()        const { return _Y0; }

    // просчитываем движение пули, столкновение ее с монстром или конец траектории
    // возвращаем ноль, если столкновения не происходит, или счетчик анимации взрыва, если столкновение произошло
    virtual int Move(const float &, const float &, void *);

 private:

    // пересечение отрезка с окружностью
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

    void threadMove(std::vector< std::list<gameObjectBase*>* > *);

 private:
    float   _X0, _Y0;               // изначальная точка, из которой пуля летит
    float   _dX, _dY;               // смещения по x и по y для нахождения новой позиции пули

    float   dx, dy, a, b, c;        // переменные для вычисления пересечения пули с монстром, чтобы не объявлять их каждый раз в теле функции
                                    // ??? - надо бы потестить, может и пусть себе объявляются в теле функции?.. - вроде особой разницы нет...

    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide;

    static int _scrWidth;           // Значения координат, за пределами которых пуля считается ушедшей в молоко
    static int _scrHeight;          // Значения координат, за пределами которых пуля считается ушедшей в молоко

	// new test
	unsigned int hitCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Бонус
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

    // Для бонуса рассчитываем время жизни, поворот, масштаб и взаимодействие с Игроком
    virtual inline int Move(const float &x, const float &y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // Бонус взят (??? расстояние пока что выбрано методом научного тыка и считается не по окружности, а по квадрату)
        // При этом бонус умирает, а игрок получает новый эффект
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Effect);
            _Alive = false;
            return 1;
        }

        // Немного шевелим бонусный спрайт
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        _Angle = 0.5f * sin(_AngleCounter);
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
   
        if( --_LifeTime <= 0 )
		    _Alive = false;

        return 0;
    }

    // В качестве номера анимации просто отдаем номер эффекта и все время показываем одно и то же изображение
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
