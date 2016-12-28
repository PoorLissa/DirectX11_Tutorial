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



// ������ ���� ������������ �����, ������ ��� � ��� ���� �) ������, �) ����� ��� ���������
void BulletIon::threadMove()
{
    float       Rad      = 20.0f;
    int         RadCells = GameCells.getDist_inCells(Rad);
    int        _monsterX, _monsterY, i, j, mon;
    int        _squareX0, _squareY0, _squareX1, _squareY1;      // ���������� ��������, ���������� ������ ������� �������� ���� � ����� ��������
    OlegCell   *Cell;
    std::vector<gameObjectBase*> *vec;

    // ����� ���������� ���� � ������ � ����� ���� ��������
    int bulletX0 = _X,
        bulletY0 = _Y,
        bulletX1 = _X + _dX,
        bulletY1 = _Y + _dY;

    // ��������� ���������� ���� �� ������������ � �������� ������� ���������
    GameCells.getCellCoordinates(bulletX0, bulletY0);
    GameCells.getCellCoordinates(bulletX1, bulletY1);

    // ���������� ������������� ������ ������� �������� ����
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



    // ���������� ������ ������� ������ (��� ������������ � �������� ���� ������������ �� �����)
    if( _bulletType == Player::BulletsType::ION_EXPLOSION ) {

        _Health++;

        // 15 ����� - �������������� ������ ��� ���������� ������, ������ �������� �� ���� ������ (����, � �������� ������, � �������� ������)
        // ??? - change 15 to method call
        if( _Health <= 15 ) {   
            _Scale   = _Health;         // ���������� ������ ������ ��� �������
            Rad      = _Health * 5;     // bullet sprite texture size / 2 (as in the _gameShader_Bullet.vs file)
            RadCells = GameCells.getDist_inCells(Rad);

            // ��� ������� ������ ������������ ���� ����������� ������ �������
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

    // ��������, ��� ���������� �������� �� ������� �� ������� ����� �����
    GameCells.checkCoordinates(_squareX0, _squareY0, _squareX1, _squareY1);



    // ���������� �� ���� ������� �� ������������� �������������� � ��� ��� ��������, � ��� �� �������� ������������ ����
    for (i = _squareX0; i < _squareX1; i++) {
    
        for (j = _squareY0; j < _squareY1; j++) {

            // �������� ��������� �� ������ � �� ������ ��������, ����������� � ���
            Cell = &( GameCells(i, j) );
            vec  = &( Cell->cellList  );

            // ����� �������, ����� ����� ������ �� ����� � ��� ������, ���� �� �����:
            Cell->cellMutex.lock();

                for (mon = 0; mon < vec->size(); mon++) {
    
                    _monsterX = (*vec)[mon]->getPosX();
                    _monsterY = (*vec)[mon]->getPosY();

                    switch( _bulletType )
                    {
                        // ��� ������ ���� (� ������� ���� ������): �������, ��� ���� �������� � �������, ���� �� ����������� ����� ���������� ����������,
                        // ������ ������� ����� ����� �������� ������� � ����
                        case Player::BulletsType::ION:
                        {
                            if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad + 25) )
                            {
                                // ��� ���������: ������������� ����, ��������� �� � ����� ������� � �������� �� ��� �� ION_EXPLOSION.
                                // ����� � �������� ����� ����� (� � ��� � ������ ������), ���� �� ��������� ��������� ��������� ��������, ����� ������� ������ ������
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

                        // �������� ��� ������� ������. �� �������� - ��������� ����������� ��������� �������, ����������� ������ �������, � ���������� ������
                        case Player::BulletsType::ION_EXPLOSION:
                        {
                            // ���� ������� ������� �������, ����������� ��� ����� (� ���� ��� ������ ������� � ������ �����)
                            // ����� �����, ��� ������ ������ ������������� �� ���������� ���������� �������, ������� ����� ����������� ���������:
                            // �� �������, ������� ����� � ������ ������, �������� ��� ������� ������ ������ � ������� ������� ��������� ����, ��� ������������
                            if( commonSectionCircle(_monsterX-10, _monsterY-10, _monsterX+10, _monsterY+10, _X, _Y, Rad) )
                                (*vec)[mon]->setAlive(false);

                            // _����_ ������ ����, ������� ��� �� ������ ������:

                            // �������� ��������� ������� ������� � ������� �������, � ��������� ������� �������
                            if( (*vec)[mon] != (*vec).back() )
                                (*vec)[mon] = (*vec).back();
                            (*vec).pop_back();

                            break;
                        }
                    }
                }

            // ����������� �������
            Cell->cellMutex.unlock();
        }
    }

    // ----------------------------------------------------------------

    // ���������� ���� ������, ���� �� � ���� �� ������
    _X += _dX;
    _Y += _dY;

    if ( _X < -100 || _X > _scrWidth || _Y < -100 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // ���� ���� � ������
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------
