#include "stdafx.h"
#include "Weapon.h"
#include "Player.h"



// ��� ������-������ ������������ ����� �����, �������, ������� � �������������� � �������
void Weapon::Move(cfRef wndPosX, cfRef wndPosY, void *Param)
{
    Player *player = static_cast<Player*>(Param);

    // �����-������ ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
    // ��� ���� �����-������ �������, � ����� �������� ����� �����
    if( abs(_X - player->getPosX() + wndPosX) < 30 && abs(_Y - player->getPosY() + wndPosY) < 30 ) {

        player->setEffect(_Weapon);
        _Alive = false;
        return;
    }

    // ������������ ������
    _AngleCounter += 0.01f;
    _ScaleCounter += 0.01f;

    //_Angle = 0.5f * sin(_AngleCounter);

    _Angle = 0.5f * sin(_AngleCounter) + 45.0f * 3.14f / 180;

    // ��� ��������� ������ �� ������ ��������� �� ����, � ����� ������ ������ ������� � ����� ��������
    if( _LifeTime > 140 )
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  2.0f + 0.1f * sin(_ScaleCounter);
    else
        _Scale = 2.0f;

    // ����� ����� ����� ������ �������� ��������, ��� ���� ������ ��� � ����� ������ ��������� ������ ����� �� ����
    if( _LifeTime <= 280 ) {

        if( !( _LifeTime > 140 && _LifeTime % 70 ) ) {
            _flashCounter = 5;
        }

        if( _flashCounter ) {
            _flashCounter--;
            _Scale += 0.075f;
        }

        if( _LifeTime < 110 ) {
            _ScaleModifier -= 0.0075f;
        }

        _Scale *= _ScaleModifier;
    }

    if( --_LifeTime <= 0 )
		_Alive = false;

    return;
}

