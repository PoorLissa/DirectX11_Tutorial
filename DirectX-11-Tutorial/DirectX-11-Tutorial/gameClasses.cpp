#include "gameClasses.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������
int Player::Move(const float &x, const float &y, void *Param)
{
	_Step = _Speed;

    // � ������ ������� ���� ������ ��������� ��� � sqrt(2) ���, ����� �������������� �������� ���� �������� ��������
    if( ( _Up && _Left ) || ( _Up && _Right ) || ( _Down && _Left ) || ( _Down && _Right ) )
        _Step *= 0.7071068f;

    if (  _Down ) _Y += _Step;
    if (  _Left ) _X -= _Step;
    if (    _Up ) _Y -= _Step;
    if ( _Right ) _X += _Step;

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// ����������� �������
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



// ����������� ��� ����
Bullet::Bullet(const int &x, const int &y, const int &x_to, const int &y_to, const float &speed)
				: gameObjectBase(x, y, 0.0f, speed), _X0(x), _Y0(y)
{
	// �������� ������� ���� �� ����� ����, ����� ��� ������ ���� ��������� �� ����� �������� � ����� ������������
    // ����� ����� ���������� ���� ���� � ������, ������������ �������� ��� ������ �������� �������
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

    // �������� dX � dY
	// ����������� �������� ���:
	//	double Dist = sqrt((_x-X)*(_x-X) + (_y-Y)*(_y-Y));
	//	dX = Speed * (X-_x)/Dist;
	//	dY = Speed * (Y-_y)/Dist;
    // �� �� ������� �������� ����������:
    float Speed_Divided_By_Dist = _Speed / sqrt(dX*dX + dY*dY);
    dX = Speed_Divided_By_Dist * dX;
    dY = Speed_Divided_By_Dist * dY;

	float initialMult = float( 1.0 + rand()%10 ) * 0.1f;

	_X += dX * initialMult;
	_Y += dY * initialMult;
}
// ------------------------------------------------------------------------------------------------------------------------



// �� ���� �������� ���������� ���� � ������ ������� � ���������. ������������ ������������ ���� � ���������, � ��� �� ��� �������
int Bullet::Move(const float &x, const float &y, void *Param)
{
    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// ������� ������� � ������ �����������, ��������� �� ���� ����� � ������ ��������
	// ??? - ����� ���������, ���������� �� ���, � ��������� ������ ��������
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

        // �������� ������ � ��������� �� ������� ��������
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            monsterX = (*iter)->getPosX();
            monsterY = (*iter)->getPosY();

            // ������� ��������, ��������� �� ���� � ������ ����������� � �������, ����� �� ������� ����������� � ����������� ��� ������� ������� �� �����
            if( squareX0 > monsterX && squareX1 < monsterX && squareY0 > monsterY && squareY1 < monsterY ) {

                if( commonSectionCircle(_X, _Y, _X + dX, _Y + dY, monsterX, monsterY, 20) ) {

                    (*iter)->setAlive(false);			// ������ ����

					if( rand() % 5 == 1 ) {
#if 1
						this->_Alive = false;           // ���� ���������

						dX = dY = 0.0;                  // ������������� ����
						_X = monsterX;                  // ��������� ���� � 
						_Y = monsterY;                  // ����� �������

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
        this->_Alive = false;   // ���� ���� � ������
        return 1;
    }

    return 0;
}
// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������� � ����������� ( http://www.cyberforum.ru/cpp-beginners/thread853799.html )
bool Bullet::commonSectionCircle(double x1, double y1, double x2, double y2, const double &xCirc, const double &yCirc, const double &Rad)
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
    b = 2.0 * (x1*dx + y1*dy);
    c = x1*x1 + y1*y1 - Rad*Rad;

    // � ������ ���������, ���� �� �� ������� [0..1] �������
    if (-b < 0)
        return c < 0;

    if (-b < (2.0 * a))
        return (4.0 * a*c - b*b) < 0;

    return (a + b + c) < 0;
}
// ------------------------------------------------------------------------------------------------------------------------
