#pragma once
#ifndef _GAME_CELLS_H_
#define _GAME_CELLS_H_

#include "Monster.h"
#include "gameObjectBase.h"

// ------------------------------------------------------------------------------------------------------------------------



struct OlegCell {
    std::vector<gameObjectBase *> cellList;     // ������, � ������� ����� ���������� �������, �������� � ������
    std::mutex                    cellMutex;    // ������� ��� ������������ ������� cellList �� ������/��������
    unsigned int                  cellId;       // ���������� id ��� �����
};

// ------------------------------------------------------------------------------------------------------------------------



class gameCells {

 public:
    gameCells() {

        if( _Single )
            exit(EXIT_FAILURE);
        else
            _Single = true;
    }

   ~gameCells() {
        delete VEC;
    }

    void Init(int bgrWidth, int bgrHeight, int cellSide = 10) {

        // ��������� ���������
        _lowX = 0;
        _lowY = 0;
        _maxX = bgrWidth;
        _maxY = bgrHeight;

        _widthPixels  = abs(_maxX - _lowX);
        _heightPixels = abs(_maxY - _lowY);

        // ����� ����� ��������� ���������� ������
        _cellSide = cellSide;
        _cellSideInverted = 1.0f / _cellSide;

        _widthCells  = _widthPixels  * _cellSideInverted;
        _heightCells = _heightPixels * _cellSideInverted;

        // �������� ������ � �������� � �������� ���� ������� ���������� id
        VEC = new std::vector<OlegCell>(_widthCells * _heightCells);

        for (unsigned int i = 0; i < _widthCells * _heightCells; i++)
            (*VEC)[i].cellId = i;   

        return;
    }

    // ������������� �������� ���������� �� �������������� ������ � ���������� ��������������� ������
    inline OlegCell& operator() (const int &posx, const int &posy) {
        return (*VEC)[_widthCells * posy + posx];
    }

    // �������� �� ������ 2 ���������� � ���������� � ��� ���������� ������ � �����
    inline void getCellCoordinates(int &x, int &y) {
        x *= _cellSideInverted;
        y *= _cellSideInverted;
    }

    inline int getDist_inCells(const int &dist) {
        return dist * _cellSideInverted;
    }

    inline int getCellId_withCoords(const int &x, const int &y) {
        return _widthCells * y + x;
    }

    // ���������, ����� ������������� ������ ���� �� ������� �� ������� ����� �����
    // ���� �� ����� ������� (���� ��������� �������� � ���� �����) - ���������� ���������� ���, ����� �� �� �������
    inline void checkCoordinates(int &x0, int &y0, int &x1, int &y1) {

        static int _lowX_Cells = getDist_inCells(_lowX);
        static int _lowY_Cells = getDist_inCells(_lowY);
        static int _maxX_Cells = getDist_inCells(_maxX);
        static int _maxY_Cells = getDist_inCells(_maxY);

        if( x0 < _lowX_Cells ) x0 = _lowX_Cells;
        if( y0 < _lowY_Cells ) y0 = _lowY_Cells;
        if( x1 > _maxX_Cells ) x1 = _maxX_Cells - 1;
        if( y1 > _maxY_Cells ) y1 = _maxY_Cells - 1;
    }

    // ����������� ������� � ������, ������� �� ����� ��������
    void UpdateGameCells(Monster *, const int &, const int &, const int &, const int &, float test);

 private:
    // ��������� ����������� � ������������
    gameCells(const gameCells &);
    gameCells& operator=(gameCells);

 private:
    short _lowX, _lowY, _maxX, _maxY;               // ����������, ��� ������� �������� �����
    unsigned short _cellSide;                       // ������ ������ �����
    float          _cellSideInverted;               // 1/_cellSide;
    unsigned short _widthPixels, _heightPixels;     // ������� ���� � ��������
    unsigned short _widthCells,  _heightCells;      // ������� ���� � �������
    static   bool  _Single;                         // ��� ����� ����� 1 ��������� ������, ��� ��� ������ �� ���� ������� ��������

    std::vector<OlegCell> *VEC;
};

#endif