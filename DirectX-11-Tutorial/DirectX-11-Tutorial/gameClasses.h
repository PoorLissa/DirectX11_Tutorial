#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

#define cfRef  const float &
#define ciRef  const int   &
#define cuiRef const unsigned int &

// ------------------------------------------------------------------------------------------------------------------------



// Базовый класс для всякого игрового объекта
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

    // --- Базовые методы, которые не переопределяются в классах-наследниках ---
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

    inline static void setThreadPool(ThreadPool *thPool) { _thPool = thPool; }

    // --- Виртуальные методы, уникальные для каждого класса-потомка ---
    virtual int Move(cfRef = 0, cfRef = 0, void* = 0) = 0;      // метод для перемещения объекта, вызывается в общем цикле
    inline virtual cuiRef getAnimPhase() const        = 0;      // метод для получения текущей фазы анимации объекта

 protected:
    bool            _Alive;
    float           _X, _Y;
    float           _Speed;
    float           _Angle;
	float		    _Scale;
    unsigned int    _Health, _HealthMax;

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
    // Последним элементом всегда должен быть _totalEffectsQty: так он всегда будет хранить актуальное количество эффектов
    static enum Effects { HEAL = 0, FREEZE, SHIELD, FIRE_BULLETS, SLOW, _totalEffectsQty };
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс со всеми видами оружия, доступными игроку
class BonusWeapons {
 public:
    // PISTOL должен всегда быть самым первым, от него считаются номера
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, _lastElement, _totalWeaponsQty = _lastElement - PISTOL };
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Игрок
class Player : public gameObjectBase {

 public:
    Player(cfRef x, cfRef y, cfRef scale, cfRef angle, cfRef speed, cuiRef interval, cuiRef anim_Qty, HighPrecisionTimer *timer)
		: gameObjectBase(x, y, scale, angle, speed, PLAYER_DEFAULT_HEALTH),
		    _Angle0(angle),
            appTimer(timer)
	{
        for (unsigned int i = 0; i < BonusEffects::Effects::_totalEffectsQty; i++)
            EffectsCounters[i] = 0;
    }

   ~Player() {}

    // Пока что не используем возвращаемое значение, как предполагалось когда-то, а вместо этого пользуемся методом isAlive
    virtual int Move(cfRef = 0, cfRef = 0, void* = nullptr);

    virtual inline cuiRef getAnimPhase() const { return 0; }

    inline void setDirectionL(const bool &l) { _Left  = l; }
    inline void setDirectionR(const bool &r) { _Right = r; }
    inline void setDirectionU(const bool &u) { _Up    = u; }
    inline void setDirectionD(const bool &d) { _Down  = d; }

    void setEffect(const unsigned int &);

  private:
    bool    _Left, _Right, _Up, _Down;
    float   _Step;
	float	_Angle0;	// Угол, который передаем в конструктор. Позволяет довернуть спрайт, если он изначально расположен не под тем углом, как мы хотим

    HighPrecisionTimer *appTimer;                                               // Указатель на общий таймер приложения
    unsigned int EffectsCounters[BonusEffects::Effects::_totalEffectsQty];      // Массив счетчиков длительности эффектов

    // Бонусные эффекты
    bool _Shielded;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Монстр
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

    virtual int Move(cfRef x, cfRef y, void *Param);

	virtual inline cuiRef getAnimPhase() const { return animPhase; }

    static inline void setFreeze(const bool &Freeze) { _freezeEffect = Freeze;  }   // Bonus - Заморозка монстров

 private:
	 int            animInterval0, animInterval1;
	 unsigned int   animQty, animPhase;

     static bool    _freezeEffect;                  // Когда выставлена в true, все монстры замораживаются на месте
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Пуля
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef);
   ~Bullet() {}

    // Метод для установки значений _scrWidth и _scrHeight, за пределами которых пули будут исчезать
    static void setScrSize(cuiRef width, cuiRef height) {
        unsigned int addedSize = 50;
        _scrWidth  = width  + addedSize;
        _scrHeight = height + addedSize;
    }

    virtual inline       cuiRef getAnimPhase() const { return   0; }
            inline const float& getX0()        const { return _X0; }
            inline const float& getY0()        const { return _Y0; }

    // просчитываем движение пули, столкновение ее с монстром или конец траектории
    // возвращаем ноль, если столкновения не происходит, или счетчик анимации взрыва, если столкновение произошло
    virtual int Move(cfRef, cfRef, void *);

    static inline       void          setBulletsType(const unsigned int &type) { _bulletsType = type; }
    static inline const unsigned int& getBulletsType()                         { return _bulletsType; }
    static inline       void          setPiercing(const bool &mode)            {    _piercing = mode; }
    static inline const bool        & getPiercing()                            {    return _piercing; }

 private:

    // пересечение отрезка с окружностью
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

    // Потоковый просчет попаданий пули в монстров
    void threadMove(std::vector< std::list<gameObjectBase*>* > *);

 private:
    float   _X0, _Y0;               // изначальная точка, из которой пуля летит
    float   _dX, _dY;               // смещения по x и по y для нахождения новой позиции пули

    float   dx, dy, a, b, c;        // переменные для вычисления пересечения пули с монстром, чтобы не объявлять их каждый раз в теле функции
                                    // ??? - надо бы потестить, может и пусть себе объявляются в теле функции?.. - вроде особой разницы нет...

    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide;

    static int _scrWidth;           // Значения координат, за пределами которых пуля считается ушедшей в молоко
    static int _scrHeight;          // Значения координат, за пределами которых пуля считается ушедшей в молоко

    static UINT _bulletsType;       // 0 - обычная пуля, 1 - огненная, 2 - ионная, 3 - плазменная, 4 - импульсная, 5 - замораживающая
    static bool _piercing;          // false - пуля при попадании в монстра исчезает, true - пуля пронизывает монстра, теряя часть своей жизни (true при огненных пулях и гауссовом оружии)
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Бонус
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

    // Для бонуса рассчитываем оставшееся время жизни, поворот, масштаб и взаимодействие с Игроком
    virtual inline int Move(cfRef x, cfRef y, void *Param) {

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
    virtual inline cuiRef getAnimPhase() const { return _Effect; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Effect;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Оружие
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

    // Для бонуса-оружия рассчитываем время жизни, поворот, масштаб и взаимодействие с Игроком
    virtual inline int Move(cfRef x, cfRef y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // Бонус-оружие взят (??? расстояние пока что выбрано методом научного тыка и считается не по окружности, а по квадрату)
        // При этом бонус-оружие умирает, а игрок получает новую пушку
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Weapon);
            _Alive = false;
            return 1;
        }

        // Немного шевелим бонусный спрайт
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        //_Angle = 0.5f * sin(_AngleCounter);

        _Angle = 0.5f * sin(_AngleCounter) + 45.0f * 3.14f / 180;

        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f : 2.0f + 0.1f * sin(_ScaleCounter);

        if( --_LifeTime <= 0 )
		    _Alive = false;

        return 0;
    }

    // В качестве номера анимации просто отдаем номер эффекта и все время показываем одно и то же изображение
    virtual inline cuiRef getAnimPhase() const { return _Weapon; }

 private:
	 unsigned int _LifeTime;
     unsigned int _Weapon;
     float        _AngleCounter;
     float        _ScaleCounter;
};
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

#endif
