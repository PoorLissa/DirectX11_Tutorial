#include "stdafx.h"
#include "gameClasses.h"

// Инициализируем статические переменные в глобальной области
bool Monster   :: _freezeEffect = false;
int  Bullet    :: _scrWidth     = 0;
int  Bullet    :: _scrHeight    = 0;
bool gameCells :: Single        = false;

ThreadPool* gameObjectBase::_thPool = nullptr; // статический пул потоков семейства объектов gameObjectBase

gameCells GameCells;

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
        #define finalFactor     0.2f;
        #define timeRemaining   EffectsCounters[effect]

        static unsigned int finalPart = EFFECT_DEFAULT_LENGTH * finalFactor;

        for (unsigned int effect = 0; effect < Bonus::Effects::_totalEffectsQty; effect++) {

            // Уменьшаем оставшееся время эффекта на каждом новом кадре
            if( timeRemaining ) {

                if( timeRemaining > 1 ) {

                    timeRemaining--;

                    // Когда до истечения эффекта остается 20% от его базовой длительности, можем сделать плавный выход из него:

                    if( timeRemaining < finalPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // Плавно переинициализируем таймер со значения (appTimerInterval * SLOW_EFFECT_FACTOR) до (appTimerInterval * 1)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(timeRemaining) / (finalPart);  // [1 ... 0];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor);
                            }
                            break;
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
                        }
                        break;

                        case Bonus::Effects::FREEZE:
                        {
                            Monster::setFreeze(false);
                        }
                        break;

                        case Bonus::Effects::HEAL:
                        {
                            // Здесь ничего отключать не нужно
                        }
                        break;

                        case Bonus::Effects::SHIELD:
                        {
                            _Shielded = false;
                        }
                        break;

                        case Bonus::Effects::FIRE_BULLETS:
                        {
                            setBulletsType_Off(Player::BulletsType::FIRE);
                            setBulletsType_Off(Player::BulletsType::PIERCING);
                            setBulletsType_On (Player::BulletsType::NORMAL);

                            // Восстановим тип пуль, который был до установки эффекта
                            _bulletsType = _bulletsType_old;
                            _bulletsType_old = 12345;   // magic number, который, как мы полагаем, никогда не появится в виде комбинации существующих эффектов
                        }
                        break;
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

        if( effect == BonusEffects::Effects::SLOW)
            effectLength /= SLOW_EFFECT_FACTOR;

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
                    _appTimer->reInitialize(appTimerInterval * SLOW_EFFECT_FACTOR);
                }
                break;

                case Bonus::Effects::FREEZE:
                {
                    Monster::setFreeze(true);
                }
                break;

                case Bonus::Effects::HEAL:
                {
                    _Health = _Health < _HealthMax - 100 ? _Health + 100 : _HealthMax;
                }
                break;

                case Bonus::Effects::SHIELD:
                {
                    _Shielded = true;
                }
                break;

                case Bonus::Effects::FIRE_BULLETS:
                {
                    // Запомним текущий тип пуль
                    if( _bulletsType_old == 12345 )
                        _bulletsType_old = _bulletsType;

                    setBulletsType_Off(Player::BulletsType::NORMAL);
                    //setBulletsType_Off(Player::BulletsType::ION);
                    setBulletsType_On (Player::BulletsType::FIRE);
                    setBulletsType_On (Player::BulletsType::PIERCING);
                }
                break;
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
                //_weaponBulletSpeed  = 10;
                _weaponBulletSpeed  = 2;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::RIFLE:
            {
                _weaponDelay        = 10;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 20;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::SHOTGUN:
            {
                _weaponDelay        = 30;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 7;
                _weaponBulletSpread = 100;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::ION_GUN:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 7;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 5;

                setBulletsType_On (Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;
        }

        _weaponReady = _weaponDelay;
    }

    #undef ThisEffectLength

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// Перемещение Монстра
void Monster::Move(cfRef x, cfRef y, void *Param)
{
#if 0
    _thPool->runAsync(&Monster::threadMove, this, x, y, Param);
#else
    if( !_freezeEffect ) {

        int currX = _X;
        int currY = _Y;

        olegType **olegArray = static_cast<olegType**>(Param);

        // если монстр находится в пределах видимости, ищем в олегомассиве по координатам монстра указатель на него и удаляем,
        // т.к. собираемся передвинуться на другое место
        if( currX >= 0 && currX < 800 && currY >= 0 && currY < 600 ) {

            std::vector<gameObjectBase*> *vec = &olegArray[currX][currY];

            for (int i = 0; i < vec->size(); i++) {

                gameObjectBase *ptr = vec->at(i);

                if( ptr == this ) {
                
                    vec->erase(vec->begin() + i);
                    break;
                }
            }
        }

	    float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) * 0.01f;

        _X += dX;
        _Y += dY;

	    animInterval1--;

	    if( animInterval1 < 0 ) {
		    animInterval1 = animInterval0;

		    animPhase++;

		    if(animPhase > animQty)
			    animPhase = 0;
	    }

        // записываем новое местоположение монстра в олегомассив
        currX = _X;
        currY = _Y;

        if( currX >= 0 && currX < 800 && currY >= 0 && currY < 600 ) {

            std::vector<gameObjectBase*> *vec = &olegArray[currX][currY];
            vec->push_back(this);
        }
    }
#endif
    return;
}

// тестовое, пытался использовать с буллет-хелпером, но не задалось
void Monster::threadMove(cfRef x, cfRef y, void *Param)
{
    if( !_freezeEffect ) {

        BulletHelper *bltHelper = static_cast<BulletHelper*>(Param);

	    float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) * 0.01f;

        _X += dX;
        _Y += dY;

	    animInterval1--;

	    if( animInterval1 < 0 ) {
		    animInterval1 = animInterval0;

		    animPhase++;

		    if(animPhase > animQty)
			    animPhase = 0;
	    }

        //bltHelper->threadMove(this);
    }

    return;
}

// ------------------------------------------------------------------------------------------------------------------------

#define BULLET_BONUS_LIFE 0 // для гауссового оружия

// Конструктор для Пули
Bullet::Bullet(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed, UINT bulletType)
				: gameObjectBase(x, y, scale, 0.0f, speed, BULLET_DEFAULT_HEALTH + BULLET_BONUS_LIFE),
                    _squareSide(20),
                    _bulletType(bulletType)
{
	// Вычислим поворот пули на такой угол, чтобы она всегда была повернута от точки выстрела в точку прицеливания
    // Будем затем передавать этот угол в шейдер, одновременно запрещая все другие повороты спрайта
    static const float divPIby180 = D3DX_PI / 180.0f;
    static const float div180byPI = 180.0f / D3DX_PI;

    _dX = x_to - x;
    _dY = y_to - y;

    if (_dX == 0.0f) {

        _Angle = _dY > 0.0f ? 180.0f : 0.0f;

    }
    else {

        _Angle = atan(_dY / _dX) * div180byPI;
        _Angle = _dX > 0.0f ? _Angle + 90.0f : _Angle + 270.0f;
    }

    _Angle = (270.0f - _Angle) * divPIby180;

    // Вычислим dX и dY
	// Классически делается так:
	//	double Dist = sqrt((_x-X)*(_x-X) + (_y-Y)*(_y-Y));
	//	dX = Speed * (X-_x)/Dist;
	//	dY = Speed * (Y-_y)/Dist;
    // Но мы немного ускоряем вычисление:
    float Speed_Divided_By_Dist = _Speed / sqrt(_dX*_dX + _dY*_dY);
    _dX = Speed_Divided_By_Dist * _dX;
    _dY = Speed_Divided_By_Dist * _dY;

    // Зададим пулям разброс по начальной точке, чтобы они не шли ровными рядами
	float initialMult = float( 1.0 + rand()%10 ) * 0.1f;
	_X += _dX * initialMult;
	_Y += _dY * initialMult;

    // проставим свойство Piercing
    _piercing = bulletType & 1 << Player::BulletsType::PIERCING;

    // смещаем пулю так, чтобы она летела не от центра Игрока, а из среза ствола его пушки
    //static const int gunRadius = 35*2;
    static const int gunRadius = 20;
    _X0 = _X = _X - gunRadius * cos(_Angle);
    _Y0 = _Y = _Y + gunRadius * sin(_Angle);
}
// ------------------------------------------------------------------------------------------------------------------------



// Просчет движения пуль и попадания их в монстров (потоковая версия)
// На вход получаем вектор списков с монстрами. Рассчитываем столкновения пуль с монстрами, и кто из них умирает.
// Координаты пули передаются только для совместимости с сигнатурой базового метода и нигде не используются
void Bullet::threadMove_VECT(void *Param)
{
    float Rad = 20.0f;

    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// Сначала смотрим в первом приближении, находится ли пуля рядом с данным монстром
	// При (2 * 20000 монстров) и порядка 50-100 пулях дает разницу примерно в 10 фпс (~33 против ~23)
    // ??? Нужно еще поэкспериментировать с размером квадрата

    // ??? - Можно еще сделать проверку на нулевую точку: если монстр левее нулевой точки. а пуля летит вправо, то она никогда в него не попадет
    if( _dX > 0 ) {
        _squareX0 = int(_X);
        _squareX1 = int(_X + _dX);
    }
    else {
        _squareX1 = int(_X);
        _squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y);
        _squareY1 = int(_Y + _dY);
    }
    else {
        _squareY1 = int(_Y);
        _squareY0 = int(_Y + _dY);
    }

    _squareX0 -= _squareSide;
    _squareX1 += _squareSide;
    _squareY0 -= _squareSide;
    _squareY1 += _squareSide;


    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

#if defined useSorting

            // Сортировка в списках монстров сделана по иксовой координате
            // В таком случае, пока мы не приблизились к левой стороне грубого квадрата, мы пропускаем всех монстров
            if( _monsterX < _squareX0 ) {
                ++iter;
                continue;
            }
            // ... а как только мы прошли правую его сторону, то прерываем цикл, т.к. все остальные монстры будут еще правее
            if( _monsterX > _squareX1 )
                break;

#endif

            // сначала проверим, находится ли пуля в грубом приближении к монстру, чтобы не считать пересечение с окружностью для каждого монстра на карте
            if( _squareX0 < _monsterX && _squareX1 > _monsterX && _squareY0 < _monsterY && _squareY1 > _monsterY )
            {
                // Если грубая проверка положительна, просчитываем точное столкновение
                if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                    (*iter)->setAlive(false);			// монстр убит

                    // Если включен бонус Piercing, понижаем время жизни пули на единицу. Если нет, то пуля умирает после первого же попадания.

                    _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                    // Если время жизни пули истекло, то пуля истрачена:
					if( !_Health ) {

						this->_Alive = false;

						_dX = _dY = 0.0;            // Останавливаем пулю
						_X = (float)_monsterX;      // Переносим пулю в центр монстра
						_Y = (float)_monsterY;

						return;
					}
                }

            } // if-square...

            ++iter;
        }
    }

    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // пуля ушла в молоко
        return;
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



void Bullet::threadMove_Oleg(void *Param)
{
    float Rad = 20.0f;

    olegType **olegArray = static_cast<olegType **>(Param);
/*
    if( _dX > 0 ) {
        _squareX0 = int(_X) - 2;
        _squareX1 = int(_X + _dX + _squareSide);
    }
    else {
        _squareX1 = int(_X) + 2;
        _squareX0 = int(_X + _dX - _squareSide);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y) - 2;
        _squareY1 = int(_Y + _dY + _squareSide);
    }
    else {
        _squareY1 = int(_Y) + 2;
        _squareY0 = int(_Y + _dY - _squareSide);
    }
*/

    if( _dX > 0 ) {
        _squareX0 = int(_X) - 2;
        _squareX1 = int(_X + _dX + _squareSide);
    }
    else {
        _squareX1 = int(_X) + 2;
        _squareX0 = int(_X + _dX - _squareSide);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y) - 2;
        _squareY1 = int(_Y + _dY + _squareSide);
    }
    else {
        _squareY1 = int(_Y) + 2;
        _squareY0 = int(_Y + _dY - _squareSide);
    }

    if( _squareX0 >= 0 && _squareX1 < olegMaxX && _squareY0 >= 0 && _squareY1 < olegMaxY )
//lalala
    // пройдем по всем ячейкам олегомассива, которые находятся в прямоугольнике вокруг пули и проверим попадания в монстров
    for (int i = _squareX0; i < _squareX1; i++) {
    
        for (int j = _squareY0; j < _squareY1; j++) {

            std::vector<gameObjectBase*> *vec = &olegArray[i][j];

            for (int monster = 0; monster < vec->size(); monster++) {
            
                _monsterX = vec->at(monster)->getPosX();
                _monsterY = vec->at(monster)->getPosY();

                float dx = _X - _monsterX;
                float dy = _Y - _monsterY;
                float dist = sqrt(dx*dx + dy*dy);

                if( dist < Rad || commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                    vec->at(monster)->setAlive(false);  // монстр убит

                    // Если включен бонус Piercing, понижаем время жизни пули на единицу. Если нет, то пуля умирает после первого же попадания.

                    _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                    // Если время жизни пули истекло, то пуля истрачена:
					if( !_Health ) {

						this->_Alive = false;

						_dX = _dY = 0.0;            // Останавливаем пулю
						_X = (float)_monsterX;      // Переносим пулю в центр монстра
						_Y = (float)_monsterY;

						return;
					}
                }
            }
        }
    }

    // big if

    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight )
        this->_Alive = false;   // пуля ушла в молоко

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// Ионные пули просчитываем иначе, потому что у них есть а) радиус, б) взрыв при попадании
void BulletIon::threadMove(void *Param)
{
    float Rad = 20.0f;

    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// Сначала смотрим в первом приближении, находится ли пуля рядом с данным монстром

    if( _dX > 0 ) {
        _squareX0 = int(_X);
        _squareX1 = int(_X + _dX);
    }
    else {
        _squareX1 = int(_X);
        _squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y);
        _squareY1 = int(_Y + _dY);
    }
    else {
        _squareY1 = int(_Y);
        _squareY0 = int(_Y + _dY);
    }

    // рассчитаем радиус ионного взрыва
    if( _bulletType == Player::BulletsType::ION_EXPLOSION ) {

        _Health++;

        // 15 здесь - ограничивающий фактор для окружности взрыва, должен зависеть от типа оружия (типа, у винтовки больше, у минигана меньше)
        if( _Health <= 15 ) {   
            _Scale = _Health;
            Rad    = _Health * 5;  // bullet sprite texture size / 2 (as in the _gameShader_Bullet.vs file)

            // для ионного взрыва рассчитываем свой собственный грубый квадрат
            int SquareSize = Rad + 10;

            _squareX0 -= SquareSize;
            _squareX1 += SquareSize;
            _squareY0 -= SquareSize;
            _squareY1 += SquareSize;
        }
        else
            this->_Alive = false;

    }
    else {

        _squareX0 -= _squareSide;
        _squareX1 += _squareSide;
        _squareY0 -= _squareSide;
        _squareY1 += _squareSide;

    }

    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

            // сначала проверим, находится ли пуля в грубом приближении к монстру, чтобы не считать пересечение с окружностью для каждого монстра на карте
            if( _squareX0 < _monsterX && _squareX1 > _monsterX && _squareY0 < _monsterY && _squareY1 > _monsterY )
            {
                switch( _bulletType )
                {
                    // Для ионной пули (у которой есть радиус): считаем, что пуля попадает в монстра, если ее центральная линия пересекает окружность,
                    // радиус которой равен сумме радиусов монстра и пули
                    case Player::BulletsType::ION:
                    {
                        if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad + 25) )
                        {
                            // При попадании: останавливаем пулю, переносим ее в центр монстра и изменяем ей тип на ION_EXPLOSION.
                            // Далее её здоровье будет расти (а с ним и радиус взрыва), пока не достигнет некоторой пороговой величины, после которой только смерть
                            _bulletType = Player::BulletsType::ION_EXPLOSION;
                            
					        _dX = _dY = 0.0;
					        _X = (float)_monsterX;
					        _Y = (float)_monsterY;
                            _Health = 3;

                            return;
                        }
                    }
                    break;

                    // Проверка для ионного взрыва. Всё наоборот - проверяем пересечение короткого отрезка, проходящего сквозь монстра, и окружности взрыва
                    case Player::BulletsType::ION_EXPLOSION:
                    {
                        // Если монстра зацепит взрывом, прописываем ему дамаг (а пока что просто убиваем с одного удара)
                        // Имеем ввиду, что радиус взрыва увеличивается на протяжении нескольких фреймов, поэтому дамаг прописываем небольшой:
                        // те монстры, которые ближе к центру взрыва, пробудут под дамагом больше циклов и получат больший суммарный урон, чем те, что на периферии
                        if( commonSectionCircle(_monsterX-10, _monsterY-10, _monsterX+10, _monsterY+10, _X, _Y, Rad) )
                            (*iter)->setAlive(false);
                    }
                    break;
                }

            }

            ++iter;
        }
    }

    // Продвигаем пулю вперед, если ни в кого не попали
    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // пуля ушла в молоко
        return;
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// пересечение отрезка с окружностью для определения попадания пули в монстра
// http://www.cyberforum.ru/cpp-beginners/thread853799.html
bool Bullet::commonSectionCircle(float x1, float y1, float x2, float y2, const int &xCirc, const int &yCirc, const float &Rad)
{
    x1 -= xCirc;
    y1 -= yCirc;
    x2 -= xCirc;
    y2 -= yCirc;

    dx = x2 - x1;
    dy = y2 - y1;

    // составляем коэффициенты квадратного уравнения на пересечение прямой и окружности.
    // если на отрезке [0..1] есть отрицательные значения, значит отрезок пересекает окружность
    a = dx*dx + dy*dy;
    b = 2.0f * (x1*dx + y1*dy);
    c = x1*x1 + y1*y1 - Rad*Rad;

    // а теперь проверяем, есть ли на отрезке [0..1] решения
    if (-b < 0)
        return c < 0;

    if (-b < (2.0 * a))
        return (4.0 * a*c - b*b) < 0;

    return (a + b + c) < 0;
}
// ------------------------------------------------------------------------------------------------------------------------
