#include "stdafx.h"
#include "gameClasses.h"

// ------------------------------------------------------------------------------------------------------------------------



// Перемещение Игрока
int Player::Move(const float &x, const float &y, void *Param)
{
    static const float     PI = 3.14159265f;
    static const float     divPIby180 = PI / 180.0f;
    static unsigned short  bitFld = 0;

	_Step  = _Speed;
    bitFld = 0;

    // в случае нажатия двух клавиш уменьшаем шаг в sqrt(2) раз, чтобы компенсировать сложение двух векторов движения
    if( ( _Up && _Left ) || ( _Up && _Right ) || ( _Down && _Left ) || ( _Down && _Right ) )
        _Step *= 0.7071068f;

    if (  _Down ) { _Y += _Step; bitFld |= 1 << 3; }
    if (  _Left ) { _X -= _Step; bitFld |= 1 << 0; }
    if (    _Up ) { _Y -= _Step; bitFld |= 1 << 1; }
    if ( _Right ) { _X += _Step; bitFld |= 1 << 2; }

    if ( bitFld ) {

        switch (bitFld) {
    
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

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// Перемещение Монстра
int Monster::Move(const float &x, const float &y, void *Param)
{
	float dX = x - _X;
    float dY = y - _Y;
    float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

#if 0
        dX = div_Speed_by_Dist * dX * 0.1f;
        dY = div_Speed_by_Dist * dY * 0.1f;
#else
        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) * 0.01f;
#endif

    _X += dX;
    _Y += dY;

	animInterval1--;

	if( animInterval1 < 0 ) {
		animInterval1 = animInterval0;

		animPhase++;

		if(animPhase > animQty)
			animPhase = 0;
	}

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// Конструктор для Пули
Bullet::Bullet(const float &x, const float &y, const float &scale, const float &x_to, const float &y_to, const float &speed)
				: gameObjectBase(x, y, scale, 0.0f, speed), hitCounter(0)
{
	// Вычислим поворот пули на такой угол, чтобы она всегда была повернута от точки выстрела в точку прицеливания
    // Будем затем передавать этот угол в шейдер, одновременно запрещая все другие повороты спрайта
    static const float PI = 3.14159265f;
    static const float divPIby180 = PI / 180.0f;
    static const float div180byPI = 180.0f / PI;

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

    // смещаем пулю так, чтобы она летела не от центра Игрока, а из среза ствола его пушки
    static const int gunRadius = 35*2;
    _X0 = _X = _X - gunRadius * cos(_Angle);
    _Y0 = _Y = _Y + gunRadius * sin(_Angle);
}
// ------------------------------------------------------------------------------------------------------------------------


#define useThread
//#undef  useThread
#define piercingBullets
//#undef  piercingBullets

// На вход получаем вектор списков с монстрами. Рассчитываем столкновения пуль с монстрами, и кто из них умирает.
// Координаты пули передаются только для совместимости с сигнатурой базового метода и нигде не используются
int Bullet::Move(const float &x, const float &y, void *Param)
{
    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

#if defined useThread
    _thPool->runAsync(&Bullet::threadMove, this, VEC);
    return 0;
#endif

	// Сначала смотрим в первом приближении, находится ли пуля рядом с данным монсторм
	// ??? - нужно проверить, эффективно ли это, и подобрать размер квадрата
    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide = 100;

    if( _dX > 0 ) {
        squareX0 = int(_X);
        squareX1 = int(_X + _dX);
    }
    else {
        squareX1 = int(_X);
        squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        squareY0 = int(_Y);
        squareY1 = int(_Y + _dY);
    }
    else {
        squareY1 = int(_Y);
        squareY0 = int(_Y + _dY);
    }

    squareX0 += squareSide;
    squareX1 -= squareSide;
    squareY0 += squareSide;
    squareY1 -= squareSide;

    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            if( (*iter)->isAlive() ) {

                monsterX = (int)(*iter)->getPosX();
                monsterY = (int)(*iter)->getPosY();

                // сначала проверим, находится ли пуля в грубом приближении к монстру, чтобы не считать пересечение с окружностью для каждого монстра на карте
                if( squareX0 > monsterX && squareX1 < monsterX && squareY0 > monsterY && squareY1 < monsterY ) {

                    if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, monsterX, monsterY, 20) ) {

                        (*iter)->setAlive(false);			// монстр убит

#ifndef piercingBullets

						    this->_Alive = false;           // пуля истрачена

						    _dX = _dY = 0.0;                // Останавливаем пулю
						    _X = (float)monsterX;           // Переносим пулю в 
						    _Y = (float)monsterY;           // центр монстра

						    return 1;
#endif
                    }
                }
            }

            ++iter;
        }
    }

    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // пуля ушла в молоко
        return 1;
    }

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// Просчет движения пуль и попадания их в монстров, потоковая версия
void Bullet::threadMove(std::vector< std::list<gameObjectBase*>* > *VEC)
{
	// Сначала смотрим в первом приближении, находится ли пуля рядом с данным монсторм
	// ??? - нужно проверить, эффективно ли это, и подобрать размер квадрата
    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide = 100;

    if( _dX > 0 ) {
        squareX0 = int(_X);
        squareX1 = int(_X + _dX);
    }
    else {
        squareX1 = int(_X);
        squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        squareY0 = int(_Y);
        squareY1 = int(_Y + _dY);
    }
    else {
        squareY1 = int(_Y);
        squareY0 = int(_Y + _dY);
    }

    squareX0 += squareSide;
    squareX1 -= squareSide;
    squareY0 += squareSide;
    squareY1 -= squareSide;

    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            monsterX = (int)(*iter)->getPosX();
            monsterY = (int)(*iter)->getPosY();

            // сначала проверим, находится ли пуля в грубом приближении к монстру, чтобы не считать пересечение с окружностью для каждого монстра на карте
            if( squareX0 > monsterX && squareX1 < monsterX && squareY0 > monsterY && squareY1 < monsterY ) {

                if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, monsterX, monsterY, 20) ) {

                    (*iter)->setAlive(false);			// монстр убит


					// new test
					hitCounter++;
					if(hitCounter > 50) {

						this->_Alive = false;           // пуля истрачена

						_dX = _dY = 0.0;                // Останавливаем пулю
						_X = (float)monsterX;           // Переносим пулю в 
						_Y = (float)monsterY;           // центр монстра

						return;
					}

#ifndef piercingBullets

						this->_Alive = false;           // пуля истрачена

						_dX = _dY = 0.0;                // Останавливаем пулю
						_X = (float)monsterX;           // Переносим пулю в 
						_Y = (float)monsterY;           // центр монстра

						return;
#endif
                }
            }

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
