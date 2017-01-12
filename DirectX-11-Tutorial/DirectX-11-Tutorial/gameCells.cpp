#include "stdafx.h"
#include "gameCells.h"

#define unusedCellNo 2e6

// »нициализируем статические переменные в глобальной области
bool gameCells :: _Single = false;
// ------------------------------------------------------------------------------------------------------------------------



// прописываем монстра в €чейки, которые он собою занимает
// верси€ 3, использует только координаты монстра ƒќ и ѕќ—Ћ≈
// заполн€ет ровно одну €чейку, в которой находитс€ центр монстра
void gameCells::UpdateGameCells(Monster *obj, const int &oldx, const int &oldy, const int &currx, const int &curry)
{
    // переводим экранные координаты в сеточные с учетом того, что видима€ область относительно виртуальной лежит со смещением
    int  oldX     = (oldx  + _extraRange) * _cellSideInverted;
    int  oldY     = (oldy  + _extraRange) * _cellSideInverted;
    int  currX    = (currx + _extraRange) * _cellSideInverted;
    int  currY    = (curry + _extraRange) * _cellSideInverted;
    UINT currCell = obj->getCell();

    // если монстр помен€л свою €чейку, перепропишем его в новую €чейку и удалим из старой
    if( oldX != currX || oldY != currY || currCell == unusedCellNo ) {

        UINT cellId;
        std::vector<gameObjectBase*> *vec;

        // отпишемс€ от старой €чейки
        cellId = getCellId_withCoords(oldX, oldY);

        vec = &( (*VEC)[cellId].cellList );

        (*VEC)[cellId].cellMutex.lock();

            for (UINT i = 0, vecSize = vec->size(); i < vecSize; i++) {
            
                if ((*vec)[i] == obj) {

                    (*vec)[i] = vec->back();
                    vec->pop_back();

                    break;
                }
            }

        (*VEC)[cellId].cellMutex.unlock();



        // пропишемс€ в новую €чейку
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
