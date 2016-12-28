#include "stdafx.h"
#include "Monster.h"
#include "gameCells.h"

bool Monster :: _freezeEffect = false;

extern gameCells GameCells;
// ------------------------------------------------------------------------------------------------------------------------



// ����������� �������
// ���������� � ������
void Monster::Move(cfRef x, cfRef y, void *Param)
{
    if( !_freezeEffect ) {

	    float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        // ����������� �� �������������, ���� dX ��� dY ��������� � ����
        if( std::isinf(div_Speed_by_Dist) )
            return;

        dX = div_Speed_by_Dist * dX * 0.1f * (float(rand() % 200)+1) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * (float(rand() % 200)+1) * 0.01f;

        int oldX = _X;
        int oldY = _Y;

        _X += dX;
        _Y += dY;

	    animInterval1--;

	    if( animInterval1 < 0 ) {
		    animInterval1 = animInterval0;

		    animPhase++;

		    if(animPhase > animQty)
			    animPhase = 0;
	    }

        // ������� ������ ����� ����� ����������� �������
        GameCells.UpdateGameCells(this, oldX, oldY, _X, _Y, _X);
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------
