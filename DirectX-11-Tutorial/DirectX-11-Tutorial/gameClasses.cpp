#include "gameClasses.h"

// ------------------------------------------------------------------------------------------------------------------------



// Перемещение Игрока
int Player::Move(const float &x, const float &y, void *Param)
{
	_Step = _Speed;

    // в случае нажатия двух клавиш уменьшаем шаг в sqrt(2) раз, чтобы компенсировать сложение двух векторов движения
    if( ( _Up && _Left ) || ( _Up && _Right ) || ( _Down && _Left ) || ( _Down && _Right ) )
        _Step *= 0.7071068f;

    if (  _Down ) _Y += _Step;
    if (  _Left ) _X -= _Step;
    if (    _Up ) _Y -= _Step;
    if ( _Right ) _X += _Step;

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
Bullet::Bullet(const int &x, const int &y, const int &x_to, const int &y_to, const float &speed)
				: gameObjectBase(x, y, 0.0f, speed), _X0(x), _Y0(y)
{
	// Вычислим поворот пули на такой угол, чтобы она всегда была повернута от точки выстрела в точку прицеливания
    // Будем затем передавать этот угол в шейдер, одновременно запрещая все другие повороты спрайта
    static const float PI = 3.14159265f;
    static const float divPIby180 = PI / 180.0f;
    static const float div180byPI = 180.0f / PI;

    dX = x_to - x;
    dY = y_to - y;

    if (dX == 0.0f) {

        _Angle = dY > 0.0f ? 180.0f : 0.0f;
    }
    else {

        _Angle = atan(dY / dX) * div180byPI;
        _Angle = dX > 0.0f ? _Angle + 90.0f : _Angle + 270.0f;
    }

    _Angle = (270.0f - _Angle) * divPIby180;

    // Вычислим dX и dY
	// Классически делается так:
	//	double Dist = sqrt((_x-X)*(_x-X) + (_y-Y)*(_y-Y));
	//	dX = Speed * (X-_x)/Dist;
	//	dY = Speed * (Y-_y)/Dist;
    // Но мы немного ускоряем вычисление:
    float Speed_Divided_By_Dist = _Speed / sqrt(dX*dX + dY*dY);
    dX = Speed_Divided_By_Dist * dX;
    dY = Speed_Divided_By_Dist * dY;

	float initialMult = float( 1.0 + rand()%10 ) * 0.1f;

	_X += dX * initialMult;
	_Y += dY * initialMult;
}
// ------------------------------------------------------------------------------------------------------------------------



// На вход получаем координаты пули и вектор списков с монстрами. Рассчитываем столкновения пуль с монстрами, и кто из них умирает
int Bullet::Move(const float &x, const float &y, void *Param)
{
    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// Сначала смотрим в первом приближении, находится ли пуля рядом с данным монсторм
	// ??? - нужно проверить, эффективно ли это, и подобрать размер квадрата
    int squareX0, squareY0, squareX1, squareY1, monsterX, monsterY, squareSide = 100;

    if (dX > 0) {
        squareX0 = _X;
        squareX1 = _X + dX;
    }
    else {
        squareX1 = _X;
        squareX0 = _X + dX;
    }

    if (dY > 0) {
        squareY0 = _Y;
        squareY1 = _Y + dY;
    }
    else {
        squareY1 = _Y;
        squareY0 = _Y + dY;
    }

    squareX0 += squareSide;
    squareX1 -= squareSide;
    squareY0 += squareSide;
    squareY1 -= squareSide;

    for (int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            monsterX = (*iter)->getPosX();
            monsterY = (*iter)->getPosY();

            // сначала проверим, находится ли пуля в грубом приближении к монстру, чтобы не считать пересечение с окружностью для каждого монстра на карте
            if( squareX0 > monsterX && squareX1 < monsterX && squareY0 > monsterY && squareY1 < monsterY ) {

                if( commonSectionCircle(_X, _Y, _X + dX, _Y + dY, monsterX, monsterY, 20) ) {

                    (*iter)->setAlive(false);			// монстр убит

					if( rand() % 5 == 1 ) {
#if 1
						this->_Alive = false;           // пуля истрачена

						dX = dY = 0.0;                  // Останавливаем пулю
						_X = monsterX;                  // Переносим пулю в 
						_Y = monsterY;                  // центр монстра

						return 1;
					}
#endif
                }
            }

            ++iter;
        }
    }

    _X += dX;
    _Y += dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        dX = dY = 0.0;
        this->_Alive = false;   // пуля ушла в молоко
        return 1;
    }

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// пересечение отрезка с окружностью ( http://www.cyberforum.ru/cpp-beginners/thread853799.html )
bool Bullet::commonSectionCircle(double x1, double y1, double x2, double y2, const double &xCirc, const double &yCirc, const double &Rad)
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
    b = 2.0 * (x1*dx + y1*dy);
    c = x1*x1 + y1*y1 - Rad*Rad;

    // а теперь проверяем, есть ли на отрезке [0..1] решения
    if (-b < 0)
        return c < 0;

    if (-b < (2.0 * a))
        return (4.0 * a*c - b*b) < 0;

    return (a + b + c) < 0;
}
// ------------------------------------------------------------------------------------------------------------------------
