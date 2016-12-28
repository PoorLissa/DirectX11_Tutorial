#include "stdafx.h"
#include "gameCells.h"

#define unusedCellNo 2e6

// �������������� ����������� ���������� � ���������� �������
bool gameCells :: _Single = false;
// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������� � ������, ������� �� ����� ��������
// ������ 3, ���������� ������ ���������� ������� �� � �����
// ��������� ����� ���� ������, � ������� ��������� ����� �������
void gameCells::UpdateGameCells(Monster *obj, const int &oldx, const int &oldy, const int &currx, const int &curry, float test)
{
    int oldX      = oldx  * _cellSideInverted;
    int oldY      = oldy  * _cellSideInverted;
    int currX     = currx * _cellSideInverted;
    int currY     = curry * _cellSideInverted;
    UINT currCell = obj->getCell();

    // ���� ������ ������� ���� ������, ������������ ��� � ����� ������ � ������ �� ������
    if( oldX != currX || oldY != currY || currCell == unusedCellNo ) {

        UINT cellId;
        std::vector<gameObjectBase*> *vec;

        // ��������� �� ������ ������
        cellId = getCellId_withCoords(oldX, oldY);

        vec = &( (*VEC)[cellId].cellList );

        (*VEC)[cellId].cellMutex.lock();

            for (int i = 0; i < vec->size(); i++)
            
                if ((*vec)[i] == obj) {

                    (*vec)[i] = vec->back();
                    vec->pop_back();

                    break;
                }

        (*VEC)[cellId].cellMutex.unlock();



        // ���������� � ����� ������
        cellId = getCellId_withCoords(currX, currY);

        (*VEC)[cellId].cellMutex.lock();

            (*VEC)[cellId].cellList.push_back(obj);
            obj->setCell(cellId);

        (*VEC)[cellId].cellMutex.unlock();
    }

    #undef unusedCellNo
    return;
}
// ------------------------------------------------------------------------------------------------------------------------
