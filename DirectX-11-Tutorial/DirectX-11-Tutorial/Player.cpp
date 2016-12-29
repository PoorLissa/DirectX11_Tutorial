#include "stdafx.h"
#include "Player.h"

// ------------------------------------------------------------------------------------------------------------------------



// Перемещение Игрока
void Player::Move(cfRef x, cfRef y, void *Param)
{
    static const float     divPIby180 = D3DX_PI / 180.0f;
    static unsigned short  bitFld = 0;

    // --- Просчитываем перемещение и поворот спрайта Игрока на нужный угол в зависимости от нажатых клавиш ---
    {
	    _Step  = _Speed;
        bitFld = 0;

        // в случае нажатия двух клавиш уменьшаем шаг в sqrt(2) раз, чтобы компенсировать сложение двух векторов движения
        if( ( _Up && _Left ) || ( _Up && _Right ) || ( _Down && _Left ) || ( _Down && _Right ) )
            _Step *= 0.7071068f;

        // каждая нажатая клавиша изменяет свой разряд в bitFld, и потом по итоговому значению этого поля можно вычислить нужный угол поворота
        if (  _Down ) { _Y += _Step; bitFld |= 1 << 3; }
        if (  _Left ) { _X -= _Step; bitFld |= 1 << 0; }
        if (    _Up ) { _Y -= _Step; bitFld |= 1 << 1; }
        if ( _Right ) { _X += _Step; bitFld |= 1 << 2; }

        if ( bitFld ) {

            switch( bitFld ) {
    
                case 1:  _Angle =   0.0f;  break;    // left
                case 2:  _Angle = 270.0f;  break;    // up
                case 4:  _Angle = 180.0f;  break;    // right
                case 8:  _Angle =  90.0f;  break;    // down
                case 3:  _Angle = 315.0f;  break;    // up + left
                case 6:  _Angle = 225.0f;  break;    // up + right
                case 9:  _Angle =  45.0f;  break;    // down + left
                case 12: _Angle = 135.0f;  break;    // down + right
            }

		    // Вручную доворачиваем текстуру игрока на нужный нулевой угол и переводим в радианы
		    _Angle = (_Angle + _Angle0) * divPIby180;
        }
    }

    _weaponReady += _weaponReady < _weaponDelay ? 1 : 0;

    // --- Просчитываем бонусные эффекты ---
    {
        #define finalFactor     0.2f
        #define timeRemaining   EffectsCounters[effect]

        static unsigned int finalPart = EFFECT_DEFAULT_LENGTH * finalFactor;
        static unsigned int earlyPart = EFFECT_DEFAULT_LENGTH - finalPart;

        for (unsigned int effect = 0; effect < Bonus::Effects::_totalEffectsQty; effect++) {

            // Уменьшаем оставшееся время эффекта на каждом новом кадре
            if( timeRemaining ) {

                if( timeRemaining > 1 ) {

                    timeRemaining -= (effect == Bonus::Effects::SLOW) ? SLOW_EFFECT_FACTOR : 1;

                    // Пока с момента начала эффекта прошло менее 20% от его базовой длительности, можем сделать плавный вход в него:
                    // ??? пока что есть проблема - если взять бонус повторно, тут снова случится плавное затухание
                    if( timeRemaining > earlyPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // Плавно переинициализируем таймер со значения (appTimerInterval) до (appTimerInterval * SLOW_EFFECT_FACTOR)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(EFFECT_DEFAULT_LENGTH - timeRemaining) / (finalPart);  // [0 ... 1];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor + 1);

                                break;
                            }
                        }
                    }

                    // Когда до истечения эффекта остается 20% от его базовой длительности, можем сделать плавный выход из него:

                    if( timeRemaining < finalPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // Плавно переинициализируем таймер со значения (appTimerInterval * SLOW_EFFECT_FACTOR) до (appTimerInterval)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(timeRemaining) / (finalPart);  // [1 ... 0];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor);

                                break;
                            }
                        }
                    }

                }
                else {
        
                    // Отключаем эффекты, когда время их действия истекло
                    switch( effect )
                    {
                        case Bonus::Effects::SLOW:
                        {
                            _appTimer->reInitialize(appTimerInterval);
                            break;
                        }

                        case Bonus::Effects::FREEZE:
                        {
                            Monster::setFreeze(false);
                            break;
                        }

                        case Bonus::Effects::HEAL:
                        {
                            // Здесь ничего отключать не нужно
                            break;
                        }

                        case Bonus::Effects::SHIELD:
                        {
                            _Shielded = false;
                            break;
                        }

                        case Bonus::Effects::FIRE_BULLETS:
                        {
                            setBulletsType_Off(Player::BulletsType::FIRE);
                            setBulletsType_Off(Player::BulletsType::PIERCING);
                            setBulletsType_On (Player::BulletsType::NORMAL);

                            // Восстановим тип пуль, который был до установки эффекта
                            _bulletsType = _bulletsType_old;
                            _bulletsType_old = 12345;   // magic number, который, как мы полагаем, никогда не появится в виде комбинации существующих эффектов
                            break;
                        }
                    }

                    timeRemaining = 0;
                }
            }
        }
    }

    #undef finalFactor
    #undef timeRemaining

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// Устанавливаем бонусный эффект (длительность эффекта хранится и просчитывается в классе Игрока, но влиять эффект может не только на Игрока)
// Здесь же устанавливаем игроку новое оружие
void Player::setEffect(const unsigned int &effect)
{
    #define ThisEffectLength EffectsCounters[effect]

    // Некоторые бонусы могут длиться время, отличное от дефолтного. Например, SLOW. Поэтому пересчитаем время в случае надобности.
    float effectLength = EFFECT_DEFAULT_LENGTH + 1;

    // Получили бонус
    if( effect < BonusEffects::Effects::_totalEffectsQty ) {

        // Если эффект уже действует, просто продлеваем его длительность
        if( ThisEffectLength > 0 ) {

            ThisEffectLength += effectLength;

        }
        else {

            // Если эффект еще не действует, устанавливаем ему длительность и вызываем соответствующий метод
            ThisEffectLength = effectLength;

            switch( effect )
            {
                case Bonus::Effects::SLOW:
                {
                    //_appTimer->reInitialize(appTimerInterval * SLOW_EFFECT_FACTOR);
                    _appTimer->reInitialize(appTimerInterval + SLOW_EFFECT_FACTOR / 100);
                    break;
                }

                case Bonus::Effects::FREEZE:
                {
                    Monster::setFreeze(true);
                    break;
                }

                case Bonus::Effects::HEAL:
                {
                    _Health = _Health < _HealthMax - 100 ? _Health + 100 : _HealthMax;
                    break;
                }

                case Bonus::Effects::SHIELD:
                {
                    _Shielded = true;
                    break;
                }

                case Bonus::Effects::FIRE_BULLETS:
                {
                    // Запомним текущий тип пуль
                    if( _bulletsType_old == 12345 )
                        _bulletsType_old = _bulletsType;

                    setBulletsType_Off(Player::BulletsType::NORMAL);
                    //setBulletsType_Off(Player::BulletsType::ION);
                    setBulletsType_On (Player::BulletsType::FIRE);
                    setBulletsType_On (Player::BulletsType::PIERCING);
                    break;
                }
            }
        }
    }

    // Взяли новое оружие
    if( effect >= BonusWeapons::Weapons::PISTOL && effect < BonusWeapons::Weapons::_lastWeapon ) {
    
        switch( effect )
        {
            case BonusWeapons::Weapons::PISTOL:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 10;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);

                _weaponBulletSpeed  = 3;
#if 0
                _weaponDelay        = 3;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 50;

                //setBulletsType_On (Player::BulletsType::ION);
#endif
                break;
            }

            case BonusWeapons::Weapons::RIFLE:
            {
                _weaponDelay        = 5;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 20;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::SHOTGUN:
            {
                _weaponDelay        = 30;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 7;
                _weaponBulletSpread = 100;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::ION_GUN:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 7;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 5;

                setBulletsType_On (Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::PLASMA_GUN:
            {
                _weaponDelay        = 1;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::PLASMA);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                break;
            }
        }

        _weaponReady = _weaponDelay;
    }

    #undef ThisEffectLength

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



void Player::spawnBullet_Normal(const int &mouseX, const int &mouseY, std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize,
                                    const int &wndPosX, const int &wndPosY)
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

    if( _bulletsType & 1 << Player::BulletsType::PLASMA )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::PLASMA) );

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
// ------------------------------------------------------------------------------------------------------------------------



void Player::spawnBullet_FourSides(std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize)
{
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X + 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X - 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y + 100, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y - 100, _weaponBulletSpeed, _bulletsType ) );

    bulletListSize += 4;
}
// ------------------------------------------------------------------------------------------------------------------------
