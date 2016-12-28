#include "stdafx.h"
#include "Bullet.h"
#include "Player.h"
#include "gameCells.h"

// �������������� ����������� ���������� � ���������� �������
int  Bullet :: _scrWidth  = 0;
int  Bullet :: _scrHeight = 0;

extern gameCells GameCells;
// ------------------------------------------------------------------------------------------------------------------------



// ����������� ��� ����
Bullet::Bullet(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed, UINT bulletType)
				: gameObjectBase(x, y, scale, 0.0f, speed, BULLET_DEFAULT_HEALTH + BULLET_BONUS_LIFE),
                    _squareSide(20),
                    _bulletType(bulletType)
{
	// �������� ������� ���� �� ����� ����, ����� ��� ������ ���� ��������� �� ����� �������� � ����� ������������
    // ����� ����� ���������� ���� ���� � ������, ������������ �������� ��� ������ �������� �������
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

    // �������� dX � dY
	// ����������� �������� ���:
	//	double Dist = sqrt((_x-X)*(_x-X) + (_y-Y)*(_y-Y));
	//	dX = Speed * (X-_x)/Dist;
	//	dY = Speed * (Y-_y)/Dist;
    // �� �� ������� �������� ����������:
    float Speed_Divided_By_Dist = _Speed / sqrt(_dX*_dX + _dY*_dY);
    _dX = Speed_Divided_By_Dist * _dX;
    _dY = Speed_Divided_By_Dist * _dY;

    // ������� ����� ������� �� ��������� �����, ����� ��� �� ��� ������� ������
	float initialMult = float( 1.0 + rand()%10 ) * 0.1f;
	_X += _dX * initialMult;
	_Y += _dY * initialMult;

    // ��������� �������� Piercing
    _piercing = bulletType & 1 << Player::BulletsType::PIERCING;

    // ������� ���� ���, ����� ��� ������ �� �� ������ ������, � �� ����� ������ ��� �����
    //static const int gunRadius = 35*2;
    static const int gunRadius = 20;
    _X0 = _X = _X - gunRadius * cos(_Angle);
    _Y0 = _Y = _Y + gunRadius * sin(_Angle);
}
// ------------------------------------------------------------------------------------------------------------------------



// ������� �������� ���� � ��������� �� � �������� (��������� ������)
// �� ���� �������� ������ ������� � ���������. ������������ ������������ ���� � ���������, � ��� �� ��� �������.
// ���������� ���� ���������� ������ ��� ������������� � ���������� �������� ������ � ����� �� ������������
void Bullet::threadMove_VECT(void *Param)
{
    float Rad = 20.0f;

    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// ������� ������� � ������ �����������, ��������� �� ���� ����� � ������ ��������
	// ��� (2 * 20000 ��������) � ������� 50-100 ����� ���� ������� �������� � 10 ��� (~33 ������ ~23)
    // ??? ����� ��� �������������������� � �������� ��������

    // ??? - ����� ��� ������� �������� �� ������� �����: ���� ������ ����� ������� �����. � ���� ����� ������, �� ��� ������� � ���� �� �������
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

        // �������� ������ � ��������� �� ������� ��������
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

            // ������� ��������, ��������� �� ���� � ������ ����������� � �������, ����� �� ������� ����������� � ����������� ��� ������� ������� �� �����
            if( _squareX0 < _monsterX && _squareX1 > _monsterX && _squareY0 < _monsterY && _squareY1 > _monsterY )
            {
                // ���� ������ �������� ������������, ������������ ������ ������������
                if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                    (*iter)->setAlive(false);			// ������ ����

                    // ���� ������� ����� Piercing, �������� ����� ����� ���� �� �������. ���� ���, �� ���� ������� ����� ������� �� ���������.

                    _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                    // ���� ����� ����� ���� �������, �� ���� ���������:
					if( !_Health ) {

						this->_Alive = false;

						_dX = _dY = 0.0;            // ������������� ����
						_X = (float)_monsterX;      // ��������� ���� � ����� �������
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
        this->_Alive = false;   // ���� ���� � ������
        return;
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ��������� ������� �������� ���� � ��������� ��� ������ �����
void Bullet::threadMove_Cells(cfRef wndPosX, cfRef wndPosY)
{
    float       Rad      = 20.0f;
    int         RadCells = GameCells.getDist_inCells(Rad);
    int        _monsterX, _monsterY, i, j, mon;
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

    // ��������� ������������� ������ ������� �������� ���� �� �������� ������� �������
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

    // ��������, ��� ���������� �������� ������ ���� �� ������� �� ������� ����� �����
    GameCells.checkCoordinates(_squareX0, _squareY0, _squareX1, _squareY1);

    // ���������� �� ���� ������� �� ������������� �������������� � ��� ��� ��������, � ��� �� �������� ������������ ����
    for (i = _squareX0; i < _squareX1; i++) {
    
        for (j = _squareY0; j < _squareY1; j++) {

            // �������� ��������� �� ������ � �� ������ ��������, ����������� � ���
            Cell = &( GameCells(i, j) );
            vec  = &( Cell->cellList  );

            // ����� �������, ����� ����� ������ �� ����� � ��� ������, ���� �� �����:
            // ���� �� ����� �������, �� ����� ������� ������ � ��� �� �������, � ���� �� ������ ��, ���� ������ ����� ����� ��������� ����� ���� �� ������,
            // ��������� ����� ��������
            Cell->cellMutex.lock();

                for (mon = 0; mon < vec->size(); mon++) {

                    _monsterX = (*vec)[mon]->getPosX();
                    _monsterY = (*vec)[mon]->getPosY();

                    if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                        (*vec)[mon]->setAlive(false);  // ������ ����

                        // �������� ��������� ������� ������� � ������� �������, � ��������� ������� �������
                        if( (*vec)[mon] != (*vec).back() )
                            (*vec)[mon] = (*vec).back();
                        (*vec).pop_back();

                        // ���� ������� ����� Piercing, �������� ����� ����� ���� �� �������. ���� ���, �� ���� ������� ����� ������� �� ���������.
                        _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                        // ���� ����� ����� ���� ��� ��� ���� ������� �������, �� ���� ���������:
			            if( !_Health ) {

			                this->_Alive = false;

				            _dX = _dY = 0.0;            // ������������� ����
				            _X = (float)_monsterX;      // ��������� ���� � ����� �������
				            _Y = (float)_monsterY;

                            // ��������� mutex.unlock, �.�. � ��������� ������ �� ������� �� ����� ��������
                            Cell->cellMutex.unlock();
				            return;
			            }
                    }
                }

            // ����������� �������
            Cell->cellMutex.unlock();
        }
    }

    // ----------------------------------------------------------------

    _X += _dX;
    _Y += _dY;

    if ( _X < -100 || _X > _scrWidth || _Y < -100 || _Y > _scrHeight )
        this->_Alive = false;   // ���� ���� � ������

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������� � ����������� ��� ����������� ��������� ���� � �������
// http://www.cyberforum.ru/cpp-beginners/thread853799.html
bool Bullet::commonSectionCircle(float x1, float y1, float x2, float y2, const int &xCirc, const int &yCirc, const float &Rad)
{
    x1 -= xCirc;
    y1 -= yCirc;
    x2 -= xCirc;
    y2 -= yCirc;

    dx = x2 - x1;
    dy = y2 - y1;

    // ���������� ������������ ����������� ��������� �� ����������� ������ � ����������.
    // ���� �� ������� [0..1] ���� ������������� ��������, ������ ������� ���������� ����������
    a = dx*dx + dy*dy;
    b = 2.0f * (x1*dx + y1*dy);
    c = x1*x1 + y1*y1 - Rad*Rad;

    // � ������ ���������, ���� �� �� ������� [0..1] �������
    if (-b < 0)
        return c < 0;

    if (-b < (2.0 * a))
        return (4.0 * a*c - b*b) < 0;

    return (a + b + c) < 0;
}
// ------------------------------------------------------------------------------------------------------------------------
