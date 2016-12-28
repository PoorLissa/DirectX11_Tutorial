#include "stdafx.h"
#include "Bullet_Ion.h"
#include "Player.h"
#include "gameCells.h"

extern gameCells GameCells;
// ------------------------------------------------------------------------------------------------------------------------



BulletIon::BulletIon(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed)
    : Bullet(x, y, scale, x_to, y_to, speed, Player::BulletsType::ION)
{
}
// ------------------------------------------------------------------------------------------------------------------------



// Ионные пули просчитываем иначе, потому что у них есть а) радиус, б) взрыв при попадании
void BulletIon::threadMove()
{
    float       Rad      = 20.0f;
    int         RadCells = GameCells.getDist_inCells(Rad);
    int        _monsterX, _monsterY, i, j, mon;
    int        _squareX0, _squareY0, _squareX1, _squareY1;      // координаты квадрата, описанного вокруг вектора смещения пули в одной итерации
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

    // просчитаем прямоугольник вокруг вектора движения пули
    if( _dX > 0 ) {
        _squareX0 = int(bulletX0);
        _squareX1 = int(bulletX1);
    }
    else {
        _squareX1 = int(bulletX0);
        _squareX0 = int(bulletX1);
    }

    if( _dY > 0 ) {
        _squareY0 = int(bulletY0);
        _squareY1 = int(bulletY1);
    }
    else {
        _squareY1 = int(bulletY0);
        _squareY0 = int(bulletY1);
    }



    // рассчитаем радиус ионного взрыва (при столкновении с монстром пуля превращается во взрыв)
    if( _bulletType == Player::BulletsType::ION_EXPLOSION ) {

        _Health++;

        // 15 здесь - ограничивающий фактор для окружности взрыва, должен зависеть от типа оружия (типа, у винтовки больше, у минигана меньше)
        // ??? - change 15 to method call
        if( _Health <= 15 ) {   
            _Scale   = _Health;         // визуальный радиус взрыва для шейдера
            Rad      = _Health * 5;     // bullet sprite texture size / 2 (as in the _gameShader_Bullet.vs file)
            RadCells = GameCells.getDist_inCells(Rad);

            // для ионного взрыва рассчитываем свой собственный грубый квадрат
            int SquareSize = RadCells + 1;

            _squareX0 -= SquareSize;
            _squareX1 += SquareSize;
            _squareY0 -= SquareSize;
            _squareY1 += SquareSize;
        }
        else
            this->_Alive = false;
    }
    else {

        _squareX0 -= RadCells;
        _squareX1 += RadCells;
        _squareY0 -= RadCells;
        _squareY1 += RadCells;

    }

    // убедимся, что координаты квадрата не выходят за пределы сетки ячеек
    GameCells.checkCoordinates(_squareX0, _squareY0, _squareX1, _squareY1);



    // пробежимся по всем ячейкам из получившегося прямоугольника и для них проверим, с кем из монстров пересекается пуля
    for (i = _squareX0; i < _squareX1; i++) {
    
        for (j = _squareY0; j < _squareY1; j++) {

            // получаем указатель на ячейку и на вектор монстров, прописанный в ней
            Cell = &( GameCells(i, j) );
            vec  = &( Cell->cellList  );

            // лочим мьютекс, чтобы никто другой не зашел в эту ячейку, пока мы здесь:
            Cell->cellMutex.lock();

                for (mon = 0; mon < vec->size(); mon++) {
    
                    _monsterX = (*vec)[mon]->getPosX();
                    _monsterY = (*vec)[mon]->getPosY();

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
                            
					            _dX = _dY = 0.0f;
					            _X = (float)_monsterX;
					            _Y = (float)_monsterY;
                                _Health = 3;

                                Cell->cellMutex.unlock();
                                return;
                            }

                            break;
                        }

                        // Проверка для ионного взрыва. Всё наоборот - проверяем пересечение короткого отрезка, проходящего сквозь монстра, и окружности взрыва
                        case Player::BulletsType::ION_EXPLOSION:
                        {
                            // Если монстра зацепит взрывом, прописываем ему дамаг (а пока что просто убиваем с одного удара)
                            // Имеем ввиду, что радиус взрыва увеличивается на протяжении нескольких фреймов, поэтому дамаг прописываем небольшой:
                            // те монстры, которые ближе к центру взрыва, пробудут под дамагом больше циклов и получат больший суммарный урон, чем периферийные
                            if( commonSectionCircle(_monsterX-10, _monsterY-10, _monsterX+10, _monsterY+10, _X, _Y, Rad) )
                                (*vec)[mon]->setAlive(false);

                            // _если_ монстр убит, удаляем его из списка ячейки:

                            // копируем последний элемент вектора в текущую позицию, а последний элемент удаляем
                            if( (*vec)[mon] != (*vec).back() )
                                (*vec)[mon] = (*vec).back();
                            (*vec).pop_back();

                            break;
                        }
                    }
                }

            // разлочиваем мьютекс
            Cell->cellMutex.unlock();
        }
    }

    // ----------------------------------------------------------------

    // Продвигаем пулю вперед, если ни в кого не попали
    _X += _dX;
    _Y += _dY;

    if ( _X < -100 || _X > _scrWidth || _Y < -100 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // пуля ушла в молоко
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------
