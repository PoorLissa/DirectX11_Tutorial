#include "stdafx.h"
#include "Bullet.h"
#include "Player.h"
#include "gameCells.h"

// Инициализируем статические переменные в глобальной области
int  Bullet :: _scrWidth  = 0;
int  Bullet :: _scrHeight = 0;

extern gameCells GameCells;
// ------------------------------------------------------------------------------------------------------------------------



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
        
        std::list<gameObjectBase*> *list = (*VEC)[lst];

        // выбираем вектор с монстрами из вектора векторов
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

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



// Потоковый просчет движения пули и попаданий при помощи ячеек
void Bullet::threadMove_Cells(cfRef wndPosX, cfRef wndPosY)
{
    float       Rad      = 20.0f;
    int         RadCells = GameCells.getDist_inCells(Rad);
    int        _monsterX, _monsterY, i, j, mon;
    OlegCell   *Cell;
    std::vector<gameObjectBase*> *vec;

    // берем координаты пули в начале и конце этой итерации
    int bulletX0 = _X,
        bulletY0 = _Y,
        bulletX1 = _X + _dX,
        bulletY1 = _Y + _dY;

    // переводим координаты пули из общеэкранной в ячеечную систему координат
    GameCells.getCellCoordinates(bulletX0, bulletY0);
    GameCells.getCellCoordinates(bulletX1, bulletY1);

    // расширяем прямоугольник вокруг вектора движения пули на величину радиуса монстра
    if( _dX > 0 ) {
        _squareX0 = int(bulletX0) - RadCells;
        _squareX1 = int(bulletX1) + RadCells;
    }
    else {
        _squareX1 = int(bulletX0) + RadCells;
        _squareX0 = int(bulletX1) - RadCells;
    }

    if( _dY > 0 ) {
        _squareY0 = int(bulletY0) - RadCells;
        _squareY1 = int(bulletY1) + RadCells;
    }
    else {
        _squareY1 = int(bulletY0) + RadCells;
        _squareY0 = int(bulletY1) - RadCells;
    }

    // убедимся, что координаты квадрата вокруг пули не выходят за пределы сетки ячеек
    GameCells.checkCoordinates(_squareX0, _squareY0, _squareX1, _squareY1);

    // пробежимся по всем ячейкам из получившегося прямоугольника и для них проверим, с кем из монстров пересекается пуля
    for (i = _squareX0; i < _squareX1; i++) {
    
        for (j = _squareY0; j < _squareY1; j++) {

            // получаем указатель на ячейку и на вектор монстров, прописанный в ней
            Cell = &( GameCells(i, j) );
            vec  = &( Cell->cellList  );

            // лочим мьютекс, чтобы никто другой не зашел в эту ячейку, пока мы здесь:
            // если мы убьем монстра, то нужно удалить запись о нем из вектора, и если мы удалим ее, пока другой поток будет проходить через этот же вектор,
            // получится очень нехорошо
            Cell->cellMutex.lock();

                for (mon = 0; mon < vec->size(); mon++) {

                    _monsterX = (*vec)[mon]->getPosX();
                    _monsterY = (*vec)[mon]->getPosY();

                    if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                        (*vec)[mon]->setAlive(false);  // монстр убит

                        // копируем последний элемент вектора в текущую позицию, а последний элемент удаляем
                        if( (*vec)[mon] != (*vec).back() )
                            (*vec)[mon] = (*vec).back();
                        (*vec).pop_back();

                        // Если включен бонус Piercing, понижаем время жизни пули на единицу. Если нет, то пуля умирает после первого же попадания.
                        _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                        // Если время жизни пули тем или иным образом истекло, то пуля истрачена:
			            if( !_Health ) {

			                this->_Alive = false;

				            _dX = _dY = 0.0;            // Останавливаем пулю
				            _X = (float)_monsterX;      // Переносим пулю в центр монстра
				            _Y = (float)_monsterY;

                            // дублируем mutex.unlock, т.к. в противном случае он никогда не будет разлочен
                            Cell->cellMutex.unlock();
				            return;
			            }
                    }
                }

            // разлочиваем мьютекс
            Cell->cellMutex.unlock();
        }
    }

    // ----------------------------------------------------------------

    _X += _dX;
    _Y += _dY;

    if ( _X < -100 || _X > _scrWidth || _Y < -100 || _Y > _scrHeight )
        this->_Alive = false;   // пуля ушла в молоко

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
