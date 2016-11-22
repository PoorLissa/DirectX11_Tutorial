#pragma once

#include <iterator>
#include "__bitmapClassInstancing.h"
#include "Helpers.h"

/*
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
*/



// Базовый класс для игрового объекта
class gameObjectBase {
 public:
    gameObjectBase(int x, int y, float speed) : _X(float(x)), _Y(float(y)), _Speed(speed) {}
    virtual ~gameObjectBase() {}

    inline int  getX() const       { return (int)_X; } 
    inline int  getY() const       { return (int)_Y; }
    inline void setX(const int &x) {  _X = (float)x; }
    inline void setY(const int &y) {  _Y = (float)y; }

    // метод для перемещения объекта, вызывается в общем цикле
    virtual void Move(const int &, const int &, void* = 0) = 0;

 protected:
    bool    _Alive;
    float   _X;
    float   _Y;
    float   _Speed;
};

// Класс игрового объекта - Игрок
class Player : public gameObjectBase {
public:
    Player(int x, int y, float speed = 1.0f) : gameObjectBase(x, y, speed) {}
   ~Player() {}

    virtual void Move(const int &x, const int &y, void *Param) {
    }

 private:
    //int color;
};

// Класс игрового объекта - Монстр
class Monster : public gameObjectBase {
 public:
    Monster(int x, int y, float speed) : gameObjectBase(x, y, speed) {}
   ~Monster() {}

    virtual void Move(const int &x, const int &y, void *Param) {

        float dX = float(x) - _X;
        float dY = float(y) - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        dX = /*float(rand()%20) * */div_Speed_by_Dist * dX * 0.1f;
        dY = /*float(rand()%20) * */div_Speed_by_Dist * dY * 0.1f;

        _X += dX;
        _Y += dY;
    }

 private:

};



// Класс инстанцированного спрайта
class InstancedSprite : public BitmapClass_Instancing {
 public:
    InstancedSprite(int width, int height) : scrWidth(width), scrHeight(height) {}
   ~InstancedSprite() {}

    bool initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec) {

        InstanceType			*instances;
        D3D11_BUFFER_DESC		 instanceBufferDesc;
        D3D11_SUBRESOURCE_DATA   instanceData;

        // We will now setup the new instance buffer.
        // We start by first setting the number of instances of the triangle that will need to be rendered.
        // For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

        // Set the number of instances in the array.
        m_instanceCount = vec->size();

        // Next we create a temporary instance array using the instance count.
        // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

        // Create the instance array.
        instances = new InstanceType[m_instanceCount];
        if (!instances)
            return false;

        // Now here is where we setup the different positions for each instance of the triangle.
        // I have set four different x, y, z positions for each triangle.
        // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
        // An instance can be modified in any way you want it to be.
        // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

        static float angle   = 0.0f;
        static int   Size    = 0;
        static int   counter = 0;
        static int   zzz     = 0;

        // Координаты в формате (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
        std::vector<gameObjectBase*>::iterator iter, end;
        int i;
        for (i = 0, iter = vec->begin(), end = vec->end(); iter != end; ++iter, ++i) {
            instances[i].position = D3DXVECTOR3(
                float( (*iter)->getX() - 0.5f * scrWidth  - 0.5f * Size ),
                float(-(*iter)->getY() + 0.5f * scrHeight - 0.5f * Size ),
                float( 10 * angle / (i + 1) )
            );

            instances[i].material = zzz;
        }

        angle += m_instanceCount * 1e-6f;

        counter++;
        if (counter > 20) {
            zzz++;
            if (zzz > 3)
                zzz = 0;

            counter = 0;
        }


        // The instance buffer description is setup exactly the same as a vertex buffer description.
        // Set up the description of the instance buffer.
        instanceBufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        instanceBufferDesc.ByteWidth        = sizeof(InstanceType)* m_instanceCount;
        instanceBufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
        instanceBufferDesc.CPUAccessFlags   = 0;
        instanceBufferDesc.MiscFlags        = 0;
        instanceBufferDesc.StructureByteStride = 0;

        // Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
        // Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

        // Give the subresource structure a pointer to the instance data.
        instanceData.pSysMem          = instances;
        instanceData.SysMemPitch      = 0;
        instanceData.SysMemSlicePitch = 0;

        // очищаем m_instanceBuffer, иначе при каждой новой отрисовке теряем память
        SAFE_RELEASE(m_instanceBuffer);

        // Create the instance buffer.
        HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
        if (FAILED(result))
            return false;

        // Release the instance array now that the instance buffer has been created and loaded.
        SAFE_DELETE_ARRAY(instances);

        return true;
    }

 private:
    int scrWidth;
    int scrHeight;
};



/*
    Тестовый класс с сохранением буфера между отрисовками.
    Сохранение буфера инстанций между отрисовками не дает особого практического смысла, т.к. прирост FPS чисто номинальный
*/
class InstancedSprite_PersistBuf : public BitmapClass_Instancing {
 public:
    InstancedSprite_PersistBuf() {
        m_instanceCount = 0;
        instances = 0;
        m_instanceBuffer = 0;
    }

   ~InstancedSprite_PersistBuf() {
        // Release the instance array only once
        delete[] instances;
        instances = 0;
    }

    bool initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec) {

        // We will now setup the new instance buffer.
        // We start by first setting the number of instances of the triangle that will need to be rendered.
        // For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

        // Set the number of instances in the array.
        if (m_instanceCount == 0)
            m_instanceCount = vec->size();

        // Next we create a temporary instance array using the instance count.
        // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

        // Create the instance array.
        if (!instances) {
            instances = new InstanceType[m_instanceCount];
            if (!instances)
                return false;
        }

        // Now here is where we setup the different positions for each instance of the triangle.
        // I have set four different x, y, z positions for each triangle.
        // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
        // An instance can be modified in any way you want it to be.
        // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

        static float angle = 0.0f;
        int Width = 800;
        int Height = 600;
        int Size = 24;

        // Координаты в формате (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
        std::vector<gameObjectBase*>::iterator iter, end;
        int i;

        // Заполняем Instance Buffer из вектора нашими данными
        for (i = 0, iter = vec->begin(), end = vec->end(); iter != end; ++iter, ++i)
            instances[i].position = D3DXVECTOR3(
                float( (*iter)->getX() - Width  / 2 - Size / 2),
                float(-(*iter)->getY() + Height / 2 - Size / 2),
                float( 10 * angle / (i + 1) )
            );

        angle += m_instanceCount / 1000000.0f;


        // заполняем структуры только при первом проходе
        if (!m_instanceBuffer) {

            // The instance buffer description is setup exactly the same as a vertex buffer description.
            // Set up the description of the instance buffer.
            instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            instanceBufferDesc.ByteWidth = sizeof(InstanceType)* m_instanceCount;
            instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            instanceBufferDesc.CPUAccessFlags = 0;
            instanceBufferDesc.MiscFlags = 0;
            instanceBufferDesc.StructureByteStride = 0;

            // Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
            // Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

            // Give the subresource structure a pointer to the instance data.
            instanceData.pSysMem = instances;
            instanceData.SysMemPitch = 0;
            instanceData.SysMemSlicePitch = 0;
        }

        // очищаем m_instanceBuffer, иначе теряем память
        if (m_instanceBuffer) {
            m_instanceBuffer->Release();
            m_instanceBuffer = 0;
        }

        // Create the instance buffer.
        HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
        if (FAILED(result))
            return false;

        return true;
    }

 private:
    InstanceType            *instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;
};


