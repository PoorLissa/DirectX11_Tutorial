#pragma once
#ifndef _GAME_CELLS_H_
#define _GAME_CELLS_H_

#include "Monster.h"
#include "gameObjectBase.h"

// ------------------------------------------------------------------------------------------------------------------------



struct OlegCell {
    std::vector<gameObjectBase *> cellList;     // вектор, в котором будут отмечаться монстры, зашедшие в ячейку
    std::mutex                    cellMutex;    // мьютекс для блокирования вектора cellList на запись/удаление
    unsigned int                  cellId;       // уникальный id для ячеек
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

        // временные константы
        _lowX = 0;
        _lowY = 0;
        _maxX = bgrWidth;
        _maxY = bgrHeight;

        _widthPixels  = abs(_maxX - _lowX);
        _heightPixels = abs(_maxY - _lowY);

        // потом нужно подобрать подходящий размер
        _cellSide = cellSide;
        _cellSideInverted = 1.0f / _cellSide;

        _widthCells  = _widthPixels  * _cellSideInverted;
        _heightCells = _heightPixels * _cellSideInverted;

        // создадим вектор с ячейками и раздадим всем ячейкам уникальные id
        VEC = new std::vector<OlegCell>(_widthCells * _heightCells);

        for (unsigned int i = 0; i < _widthCells * _heightCells; i++)
            (*VEC)[i].cellId = i;   

        return;
    }

    // пересчитываем экранные координаты во внутрисеточный индекс и возвращаем соответствующий вектор
    inline OlegCell& operator() (const int &posx, const int &posy) {
        return (*VEC)[_widthCells * posy + posx];
    }

    // передаем по ссылке 2 координаты и записываем в них координаты ячейки в сетке
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

    // проверяем, чтобы прямоугольник вокруг пули не выходил за пределы сетки ячеек
    // если он вдруг выходит (пуля находится вплотную к краю сетки) - исправляем координаты так, чтобы он не выходил
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

    // прописываем монстра в ячейки, которые он собою занимает
    void UpdateGameCells(Monster *, const int &, const int &, const int &, const int &, float test);

 private:
    // запрещаем копирование и присваивание
    gameCells(const gameCells &);
    gameCells& operator=(gameCells);

 private:
    short _lowX, _lowY, _maxX, _maxY;               // координаты, для которых строится сетка
    unsigned short _cellSide;                       // размер ячейки сетки
    float          _cellSideInverted;               // 1/_cellSide;
    unsigned short _widthPixels, _heightPixels;     // размеры поля в пикселах
    unsigned short _widthCells,  _heightCells;      // размеры поля в ячейках
    static   bool  _Single;                         // нам нужен ровно 1 экземпляр класса, так что делаем из него простой синглтон

    std::vector<OlegCell> *VEC;
};

#endif