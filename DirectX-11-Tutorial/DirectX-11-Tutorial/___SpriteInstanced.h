#pragma once
#ifndef _SPRITE_INSTANCED_H_
#define _SPRITE_INSTANCED_H_

#include "__bitmapClassInstancing.h"
#include "gameClasses.h"

/* --------------------------------------------------------------------------------------------------------
    Класс инстанцированного спрайта.
 Унаследован от класса BitmapClass_Instancing.
 Позволяет инициализировать инстанции, принадлежащие классу BitmapClass_Instancing, списком объектов gameObjectBase

    Логика использования:
1. Инициализируем объект InstancedSprite
2. Задаем вектор/список объектов, унаследованных от gameObjectBase
3. При наступлении события onTimer пересчитываем координаты и параметры всех объектов списка и вызываем метод initializeInstances:
    if (!sprIns1->initializeInstances(m_d3d->GetDevice(), &monstersVector))
        return false;
4. Вне зависимости от наступления события onTimer вызываем метод Render...
    if (!sprIns1->Render(m_d3d->GetDeviceContext(), xCenter - bmpSize / 2, yCenter - bmpSize / 2))
        return false;
5. ... и отрисовываем все при помощи шейдера:
    result = m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                sprIns1->GetVertexCount(), sprIns1->GetInstanceCount(),
                    worldMatrixZ * matTrans * matScale,
                        viewMatrix, orthoMatrix, sprIns1->GetTexture(), mouseX - xCenter, yCenter - mouseY);
// ----------------------------------------------------------------------------------------------------- */



// Класс инстанцированного спрайта
class InstancedSprite : public BitmapClass_Instancing {

 public:
    InstancedSprite(int width, int height) : scrWidth(width), scrHeight(height) {}
   ~InstancedSprite() {}

    // --- Инициализация инстанций ---

    // Получаем на вход вектор с данными и из этого вектора заполняем массив инстанцированных данных
    bool initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec);

    // Получаем на вход список с данными и из этого списка заполняем массив инстанцированных данных
    bool initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize);

	// Получаем на вход список с данными о пулях и заполняем заполняем массив инстанцированных данных (пули заполняются иначе, не как все остальные данные)
    bool initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize, bool bullet);

    // Получаем на вход всего один объект, инициализируемся из него (для текстурного атласа)
    bool initializeInstances(ID3D11Device *device, gameObjectBase* obj);

 private:
    int scrWidth;
    int scrHeight;
};

#endif
