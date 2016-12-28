#include "stdafx.h"
#include "Bonus.h"



// ��� ������ ������������ ���������� ����� �����, �������, ������� � �������������� � �������
void Bonus::Move(cfRef wndPosX, cfRef wndPosY, void *Param)
{
    BonusParams *bpr = static_cast<BonusParams*>(Param);

    float playerX = bpr->player->getPosX();
    float playerY = bpr->player->getPosY();

    // ������������ �������� ������� Telekinetic: �������� � ���������������
    if( true ) {
    
        // ��������� ��� ������ ����
        {
            if( abs(_X - *bpr->mouseX + wndPosX) < 30 && abs(_Y - *bpr->mouseY + wndPosY) < 30 ) {

                _mouseHover++;

                if( _mouseHover == 100 ) {

                    bpr->player->setEffect(_Effect);
                    _Alive = false;
                    return;
                }
            }
            else {
                _mouseHover = 0;
            }
        }

        // �������������� ���������
        {
            static int Const = 1e4;                 // ��������� �������� ������� �� ����

            float dx = (_X - playerX + wndPosX);
            float dy = (_Y - playerY + wndPosY);

            float dist2 = (dx*dx + dy*dy);
            float dist1 = sqrt( dist2 );

            dist1 = Const / (dist2 * dist1);

            _X -= dist1 * dx;
            _Y -= dist1 * dy;
        }
    }

    // ����� ���� (??? ���������� ���� ��� ������� ������� �������� ���� � ��������� �� �� ����������, � �� ��������)
    // ��� ���� ����� �������, � ����� �������� ����� ������
    if( abs(_X - playerX + wndPosX) < 30 && abs(_Y - playerY + wndPosY) < 30 ) {

        bpr->player->setEffect(_Effect);
        _Alive = false;
        return;
    }

    // ������������ ������
    _AngleCounter += 0.01f;
    _ScaleCounter += 0.01f;
    _Angle = 0.5f * sin(_AngleCounter);

    // ��� ��������� ������ �� ������ ��������� �� ����, � ����� ������ ������ ������� � ����� ��������
    if( _LifeTime > 140 )
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
    else
        _Scale = 1.0f;

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
