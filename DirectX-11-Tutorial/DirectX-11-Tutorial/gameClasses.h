#pragma once
#ifndef _GAME_CLASSES_H_
#define _GAME_CLASSES_H_

#include "__threadPool.h"
#include "__highPrecTimer.h"

#define cfRef  const float &
#define ciRef  const int   &
#define cuiRef const unsigned int &

// Предварительно объявляем классы
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
    std::vector<gameObjectBase*> cellList;      // вектор, в котором будут отмечаться монстры, зашедшие в ячейку
    std::mutex                   cellMutex;     // мьютекс для блокирования вектора cellList на запись/удаление
    unsigned int                 cellId;        // уникальный id для ячеек
};

class gameCells {

 public:
    gameCells() {

        if( Single ) {
            exit(EXIT_FAILURE);
        }
        else {
            // временные константы
            _lowX = 0;
            _lowY = 0;
            _maxX = 800;
            _maxY = 600;

            _widthPixels  = abs(_maxX - _lowX);
            _heightPixels = abs(_maxY - _lowY);

            // потом нужно подобрать подходящий размер
            _cellSide = 10;

            _widthCells  = _widthPixels  / _cellSide;
            _heightCells = _heightPixels / _cellSide;

            VEC = new std::vector<OlegType>(_widthCells * _heightCells);

            // раздадим всем ячейкам уникальные id
            for (unsigned int i = 0; i < _widthCells * _heightCells; i++)
                VEC->at(i).cellId = i;

            Single = true;
        }
    }

   ~gameCells() {
        delete VEC;
    }

    // пересчитываем экранные координаты во внутрисеточный индекс и возвращаем соответствующий вектор
    inline OlegType& operator() (const int posx, const int posy) {

        unsigned short cellX = posx / _cellSide;
        unsigned short cellY = posy / _cellSide;

        return VEC->at( _widthCells * cellY + cellX );
    }

    // передаем по ссылке 2 координаты и записываем в них координаты ячейки в сетке
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
    short _lowX, _lowY, _maxX, _maxY;               // координаты, для которых строится сетка
    unsigned short _cellSide;                       // размер ячейки сетки
    unsigned short _widthPixels, _heightPixels;     // размеры поля в пикселах
    unsigned short _widthCells, _heightCells;       // размеры поля в ячейках

    std::vector<OlegType> *VEC;

    static bool Single;

};
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

    // Перегрузим оператор сравнения для сортировки списка (монстров). Кроме этого потребуется предикат сортировки, если мы хотим сортировать список указателей
    // http://stackoverflow.com/questions/6404160/sort-a-stdlistmyclass-with-myclassoperatormyclass-other
    inline const bool operator < (const gameObjectBase &other) const {
        return this == &other ? false : _X < other._X;
    }

    inline static void setThreadPool(ThreadPool *thPool) { _thPool = thPool; }

    // --- Виртуальные методы, уникальные для каждого класса-потомка ---

    // метод для обработки объекта в пределах одного фрейма, вызывается в общем цикле. через void* может принимать на вход любые требуемые параметры
    virtual void Move(cfRef = 0, cfRef = 0, void* = nullptr) = 0;
    // метод для получения текущей фазы анимации объекта
    inline virtual cuiRef getAnimPhase() const = 0;

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
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, ION_GUN, _lastWeapon, _totalWeaponsQty = _lastWeapon - PISTOL };
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Игрок
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
	float	_Angle0;	// Угол, который передаем в конструктор. Позволяет довернуть спрайт, если он изначально расположен не под тем углом, как мы хотим

    HighPrecisionTimer *_appTimer;                                              // Указатель на общий таймер приложения
    unsigned int EffectsCounters[BonusEffects::Effects::_totalEffectsQty];      // Массив счетчиков длительности эффектов

    // Бонусные эффекты
    bool _Shielded;

    // Эффекты пуль. Устанавливаются на игрока (навсегда или до отмены) и при генерации пуль устанавливаются на каждую пулю в отдельности
    unsigned short _bulletsType, _bulletsType_old;

    unsigned int _weaponDelay, _weaponBulletSpeed, _weaponBurstQty, _weaponReady, _weaponBulletSpread;
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

    virtual void Move(cfRef, cfRef, void *);

    void threadMove(cfRef x, cfRef y, void *Param);

	virtual inline cuiRef getAnimPhase() const { return animPhase; }

    static inline void setFreeze(const bool &mode) { _freezeEffect = mode;  }   // Bonus - Заморозка монстров

 private:
	 int            animInterval0, animInterval1;
	 unsigned int   animQty, animPhase;

     static bool    _freezeEffect;                  // Когда выставлена в true, все монстры замораживаются на месте
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Пуля
class Bullet : public gameObjectBase {

 public:
    Bullet(cfRef, cfRef, cfRef, cfRef, cfRef, cfRef, UINT);
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
    virtual void Move(cfRef, cfRef, void *Param) {
        //_thPool->runAsync(&Bullet::threadMove_VECT, this, Param);
        threadMove_Oleg(Param);
    }

    inline       void          setBulletType(const unsigned int &type)  { _bulletType = type; }
    inline const unsigned int& getBulletType()                          { return _bulletType; }
    inline       void          setPiercing(const bool &mode)            {   _piercing = mode; }
    inline const bool        & isPiercing()                             {   return _piercing; }

 protected:
    // пересечение отрезка с окружностью
    // http://www.cyberforum.ru/cpp-beginners/thread853799.html
    bool commonSectionCircle(float, float, float, float, const int &, const int &, const float &);

 private:
    // Потоковый просчет попаданий пули в монстров
    void threadMove_VECT(void *);
    void threadMove_Oleg(void *);

 protected:
    float   _X0, _Y0;               // изначальная точка, из которой пуля летит
    float   _dX, _dY;               // смещения по x и по y для нахождения новой позиции пули

    float   dx, dy, a, b, c;        // переменные для вычисления пересечения пули с монстром, чтобы не объявлять их каждый раз в теле функции
                                    // ??? - надо бы потестить, может и пусть себе объявляются в теле функции?.. - вроде особой разницы нет...

    int _squareX0, _squareY0,       // координаты квадрата, описанного вокруг вектора смещения пули в одной итерации
        _squareX1, _squareY1,
        _monsterX, _monsterY,
        _squareSide;

    static int _scrWidth;           // Значения координат, за пределами которых пуля считается ушедшей в молоко
    static int _scrHeight;          // Значения координат, за пределами которых пуля считается ушедшей в молоко

    UINT _bulletType;               // 0 - обычная пуля, 1 - огненная, 2 - ионная, 3 - плазменная, 4 - импульсная, 5 - замораживающая
    bool _piercing;                 // false - пуля при попадании в монстра исчезает, true - пуля пронизывает монстра, теряя часть своей жизни (true при огненных пулях и гауссовом оружии)
};
// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Ионная пуля
// Унаследовали свой отдельный класс, т.к. у И.П. немного другой просчет движения, и не хочется городить условия в базовом методе Bullet::Move()
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
    virtual void Move(cfRef x, cfRef y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // Бонус взят (??? расстояние пока что выбрано методом научного тыка и считается не по окружности, а по квадрату)
        // При этом бонус умирает, а игрок получает новый эффект
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Effect);
            _Alive = false;
            return;
        }

        // Немного шевелим бонусный спрайт
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        _Angle = 0.5f * sin(_AngleCounter);
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
   
        if( --_LifeTime <= 0 )
		    _Alive = false;

        return;
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
    virtual void Move(cfRef x, cfRef y, void *Param) {

        Player *player = static_cast<Player*>(Param);

        // Бонус-оружие взят (??? расстояние пока что выбрано методом научного тыка и считается не по окружности, а по квадрату)
        // При этом бонус-оружие умирает, а игрок получает новую пушку
        if( abs(_X - player->getPosX()) < 33 && abs(_Y - player->getPosY()) < 33 ) {

            player->setEffect(_Weapon);
            _Alive = false;
            return;
        }

        // Немного шевелим бонусный спрайт
        _AngleCounter += 0.01f;
        _ScaleCounter += 0.01f;

        //_Angle = 0.5f * sin(_AngleCounter);

        _Angle = 0.5f * sin(_AngleCounter) + 45.0f * 3.14f / 180;

        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f : 2.0f + 0.1f * sin(_ScaleCounter);

        if( --_LifeTime <= 0 )
		    _Alive = false;

        return;
    }

    // В качестве номера анимации просто отдаем номер эффекта и все время показываем одно и то же изображение
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
    // приводим расстояние от стрелка до точки прицеливания к условному расстоянию в 500px, для которого и применяем рассеяние пуль

    short dx     = _X - mouseX;
    short dy     = _Y - mouseY;
    short dist   = sqrt(dx*dx + dy*dy);
    short Spread = dist * _weaponBulletSpread * 0.02;   // чтобы не свести Spread к нулю, делим не на 500, а на 50, и уже ниже делим итоговое значение еще на 10
    short halfSpread = Spread * 0.5f;

    // Тип создаваемой пули проверяем при помощи операции &, т.к. у Игрока может быть несколько эффектов сразу
    // В конструктор пули же передаем конкретное значение эффекта

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

    // кладем наш объект в соответствующий вектор и возвращаем номер вектора
    void Push(gameObjectBase* obj) {

        int x = obj->getPosX();
        int y = obj->getPosY();

        // Проверяем, хранится ли такой элемент уже в Map-е. Если он еще не хранится, получаем 0.
        // Если объект уже хранится, currentPair будет содержать: 1. Номер вектора, в котором находится объект, 2. Номер объекта в векторе
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

        // Спроецируем экранную позицию объекта на карту наших векторов и найдем, в каком векторе сейчас находится элемент.
        // Если его предыдущая позиция отличается от текущей, удалим объект из его старого вектора и добавим его в новый вектор.
        // Заменим значение для текущего объекта в Map-е.
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

    // Вернем указатель на вектор, содержащий все объекты, находящиеся в ячейке, на которую приходится указанная точка (x, y)
    // В процессе обработки вектора мы должны самостоятельно удалить из него те объекты, которые мы решим отметить как погибшие
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
    UINT _numX, _numY;                                      // число разбиений экрана по горизонтали и по вертикали
    std::vector< std::vector<gameObjectBase*>*    > VEC;    // вектор векторов монстров
    std::map<gameObjectBase*, std::pair<int, int> > Map;    // map, в котором для каждого монстра хранится номер вектора, в котором он находится в данный момент
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

    // кладем наш объект в соответствующий вектор
    void Push(gameObjectBase* obj) {

        unsigned int Obj = (int)obj;

        int x = obj->getPosX();
        int y = obj->getPosY();

        // Проверяем, хранится ли такой элемент уже в Map-е. Если он еще не хранится, Map.count возвращает 0.
        // Если объект уже хранится, currentPair будет содержать: 1. Номер вектора, в котором находится объект, 2. Номер объекта в векторе
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

        // Спроецируем экранную позицию объекта на карту наших векторов и найдем, в каком векторе сейчас находится элемент.
        // Если его предыдущая позиция отличается от текущей, удалим объект из его старого вектора и добавим его в новый вектор.
        // Заменим значение для текущего объекта в Map-е.
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

    // Вернем указатель на карту, содержащую все объекты, находящиеся в ячейке, на которую приходится указанная точка (x, y)
    // В процессе внешней обработки карты мы должны самостоятельно удалить из нее те объекты, которые мы решим отметить как погибшие
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

    UINT _numX, _numY;                                  // число разбиений экрана по горизонтали и по вертикали
    //std::map<int, int>                Map;            // карта, в которой для каждого монстра хранится номер карты, в которой он находится в данный момент
    std::unordered_map<int, int>                Map;
    std::vector< std::unordered_map<int, int>*> MAPS;   // вектор карт монстров
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

    // кладем наш объект в соответствующий список
    void AddObject(gameObjectBase* obj) {

        unsigned int Obj = (int)obj;

        int x = obj->getPosX();
        int y = obj->getPosY();

        // Проверяем, хранится ли такой элемент уже в Map-е. Если он еще не хранится, Map.count возвращает 0.
        // Если объект уже хранится, value будет содержать номер списка, в котором находится объект
        int currentVec = getVecNo_withCoords(x, y);

        // Спроецируем экранную позицию объекта на карту наших векторов и найдем, в каком векторе сейчас находится элемент.
        // Если его предыдущая позиция отличается от текущей, удалим объект из его старого вектора и добавим его в новый вектор.
        // Заменим значение для текущего объекта в Map-е.
        ( *_VEC.at(currentVec) ).push_back(obj);
        //_Map[Obj] = currentVec;

        return;
    }

private:
    //std::unordered_map<int, int>               _Map;
    std::vector<std::list<gameObjectBase*>*>   _VEC;
    UINT                                       _numX, _numY;    // число разбиений экрана по горизонтали и по вертикали
    ThreadPool                                *_thPool;
};

#endif
