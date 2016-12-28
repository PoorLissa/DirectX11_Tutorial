#pragma once
#ifndef _BONUS_H_
#define _BONUS_H_

#include "gameObjectBase.h"
#include "BonusEffects.h"

// ------------------------------------------------------------------------------------------------------------------------



// Класс игрового объекта - Бонус
class Bonus : public gameObjectBase, public BonusEffects {

 public:
	Bonus(cfRef x, cfRef y, const Effects &effect)
		: gameObjectBase(x, y, 1.0f, 0.0f, 0.0f, 1),
          BonusEffects(),
            _LifeTime(BONUS_DEFAULT_LIFESPAN + rand() % 100),
            _Effect(effect),
            _AngleCounter(rand()%10),
            _ScaleCounter(0),
            _flashCounter(0),
            _ScaleModifier(1.0f),
            _mouseHover(0)
	{}
   ~Bonus() {}

    // Для бонуса рассчитываем оставшееся время жизни, поворот, масштаб и взаимодействие с Игроком
    virtual void Move(cfRef, cfRef, void *);

    // В качестве номера анимации просто отдаем номер эффекта и все время показываем одно и то же изображение
    virtual inline cuiRef getAnimPhase() const { return _Effect; }

 private:
	 unsigned int   _LifeTime;
     unsigned int   _Effect;
     float          _AngleCounter;
     float          _ScaleCounter, _ScaleModifier;
     unsigned int   _flashCounter;
     unsigned short _mouseHover;
};

#include "Player.h"

// Вспомогательная структура для передачи параметров в Bonus::Move()
struct BonusParams {
    Player *player;
    int    *mouseX;
    int    *mouseY;
};

#endif
