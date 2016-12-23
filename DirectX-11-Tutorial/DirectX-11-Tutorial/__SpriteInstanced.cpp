#include "stdafx.h"
#include "__SpriteInstanced.h"

// ------------------------------------------------------------------------------------------------------------------------



// Получаем на вход вектор с данными и из этого вектора заполняем массив инстанцированных данных
bool InstancedSprite::initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec)
{
    // Set the number of instances in the array
    if (vec->size() <= 0)
        return false;
    m_instanceCount = vec->size();


    InstanceType			*instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;

    // We will now setup the new instance buffer.
    // We start by first setting the number of instances of the triangle that will need to be rendered.
    // For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

    // Next we create a temporary instance array using the instance count.
    // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

    // Create the instance array
    SAFE_CREATE_ARRAY(instances, InstanceType, m_instanceCount);

    // Now here is where we setup the different positions for each instance of the triangle.
    // I have set four different x, y, z positions for each triangle.
    // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
    // An instance can be modified in any way you want it to be.
    // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

    std::vector<gameObjectBase*>::iterator iter, end;
    int i = 0;
    for (iter = vec->begin(), end = vec->end(); iter != end; ++iter, ++i) {
        // Позиция одной инстанции спрайта на экране, поворот этого спрайта на заданный угол и коэффициент масштабирования
        // Координаты в формате: (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
        instances[i].position = D3DXVECTOR4(
            float( (*iter)->getPosX() - 0.5f * scrWidth  ),
            float(-(*iter)->getPosY() + 0.5f * scrHeight ),
            float( (*iter)->getAngle() ),
			float( 1.0 )
        );

        // Размеры кадра анимации (для текстурного атласа only) и номер анимации
        instances[i].animationInfo = D3DXVECTOR3(
            m_spriteSliceX,                         // ширина одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
            m_spriteSliceY,                         // высота одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
            (float)(*iter)->getAnimPhase()          // фаза анимации - номер текстуры в массиве или порядковый номер кадра в атласе
        );
    }



    // The instance buffer description is setup exactly the same as a vertex buffer description.
    // Set up the description of the instance buffer.
    instanceBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.ByteWidth           = sizeof(InstanceType)* m_instanceCount;
    instanceBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags      = 0;
    instanceBufferDesc.MiscFlags           = 0;
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
    CHECK_FAILED(result);

    // Release the instance array now that the instance buffer has been created and loaded.
    SAFE_DELETE_ARRAY(instances);

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



// Получаем на вход список с данными и из этого списка заполняем массив инстанцированных данных
bool InstancedSprite::initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize)
{
    // Set the number of instances in the array. If the list is empty just return true
    if (*listSize <= 0)
        return true;
    m_instanceCount = *listSize;

/*
    if( *listSize != 10 && *listSize != 5 && *listSize != 4 && *listSize != 3 ) {
        int asd = 123;
    }
*/
    InstanceType			*instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;

    // We will now setup the new instance buffer.
    // We start by first setting the number of instances of the triangle that will need to be rendered.

    // Next we create a temporary instance array using the instance count.
    // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

    // Create the instance array
    SAFE_CREATE_ARRAY(instances, InstanceType, m_instanceCount);

    // Now here is where we setup the different positions for each instance of the triangle.
    // I have set four different x, y, z positions for each triangle.
    // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
    // An instance can be modified in any way you want it to be.
    // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

    int i = 0;
    std::list<gameObjectBase*>::iterator iter, end;

	static float halfWidth  = 0.5f * scrWidth;
    static float halfHeight = 0.5f * scrHeight;

    for (iter = list->begin(), end = list->end(); iter != end; ++iter, ++i) {

        // Позиция одной инстанции спрайта на экране, поворот этого спрайта на заданный угол и коэффициент масштабирования
        // Координаты в формате: (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
        instances[i].position = D3DXVECTOR4(
            float( (*iter)->getPosX() - halfWidth),
            float(-(*iter)->getPosY() + halfHeight),
            float( (*iter)->getAngle() ),
			float( (*iter)->getScale() )
        );

        // Размеры кадра анимации (для текстурного атласа only) и номер анимации
        instances[i].animationInfo = D3DXVECTOR3(
            m_spriteSliceX,                         // ширина одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
            m_spriteSliceY,                         // высота одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
            (float)(*iter)->getAnimPhase()          // фаза анимации - номер текстуры в массиве или порядковый номер кадра в атласе
        );
    }



    // The instance buffer description is setup exactly the same as a vertex buffer description.
    // Set up the description of the instance buffer.
    instanceBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.ByteWidth      = sizeof(InstanceType)* m_instanceCount;
    instanceBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;
    instanceBufferDesc.MiscFlags      = 0;
    instanceBufferDesc.StructureByteStride = 0;

    // Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
    // Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

    // Give the subresource structure a pointer to the instance data.
    instanceData.pSysMem          = instances;
    instanceData.SysMemPitch      = 0;
    instanceData.SysMemSlicePitch = 0;

    // очищаем m_instanceBuffer, иначе при каждой новой отрисовке теряем память
    SAFE_RELEASE(m_instanceBuffer);

    // Create the instance buffer
    HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
    CHECK_FAILED(result);

    // Release the instance array now that the instance buffer has been created and loaded
    SAFE_DELETE_ARRAY(instances);

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



// Получаем на вход список с данными о пулях и заполняем заполняем массив инстанцированных данных (пули заполняются иначе, не как все остальные данные)
bool InstancedSprite::initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize, bool bullet)
{
    // Set the number of instances in the array. If the list is empty just return true
    if (*listSize <= 0)
        return true;
    m_instanceCount = (*listSize)*2;


    InstanceType			*instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;

    // We will now setup the new instance buffer.
    // We start by first setting the number of instances of the triangle that will need to be rendered.
    // For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

    // Next we create a temporary instance array using the instance count.
    // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

    // Create the instance array
    SAFE_CREATE_ARRAY(instances, InstanceType, m_instanceCount);

    // Now here is where we setup the different positions for each instance of the triangle.
    // I have set four different x, y, z positions for each triangle.
    // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
    // An instance can be modified in any way you want it to be.
    // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

    unsigned int i = 0;
    std::list<gameObjectBase*>::iterator iter, end;

	float halfWidth  = 0.5f * scrWidth;
    float halfHeight = 0.5f * scrHeight;

    for (iter = list->begin(), end = list->end(); iter != end; ++iter, i+=2) {

        Bullet *obj = (Bullet*)(*iter);

        float X =  (*iter)->getPosX() - halfWidth;
        float Y = -(*iter)->getPosY() + halfHeight;
        float A =  (*iter)->getAngle();
		float S =  (*iter)->getScale();

        float X0 =  obj->getX0() - halfWidth;
        float Y0 = -obj->getY0() + halfHeight;

        UINT bulletType = obj->getBulletType();

        // Для каждой пули посылаем не одну, а две инстанции: для самой пули и для ее шлейфа

        // Позиция одной инстанции спрайта на экране, поворот этого спрайта на заданный угол и коэффициент масштабирования
        // Координаты в формате: (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
        instances[ i ].position      = D3DXVECTOR4(X, Y, A, S);
        instances[ i ].animationInfo = D3DXVECTOR3(0, 0, bulletType);

        // для каждой второй инстанции передаем координаты, откуда летит пуля, чтобы отрисовать шлейф
        instances[i+1].position      = D3DXVECTOR4(X, Y, A, S);
        instances[i+1].animationInfo = D3DXVECTOR3(X0, Y0, bulletType + 100);
    }



    // The instance buffer description is setup exactly the same as a vertex buffer description.
    // Set up the description of the instance buffer.
    instanceBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.ByteWidth      = sizeof(InstanceType)* m_instanceCount;
    instanceBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;
    instanceBufferDesc.MiscFlags      = 0;
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
    CHECK_FAILED(result);

    // Release the instance array now that the instance buffer has been created and loaded.
    SAFE_DELETE_ARRAY(instances);

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



// Получаем на вход всего один объект, инициализируемся из него
bool InstancedSprite::initializeInstances(ID3D11Device *device, gameObjectBase* obj)
{
    // Set the number of instances in the array
    if ( !obj )
        return false;
    m_instanceCount = 1;


    InstanceType			*instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;

    // We will now setup the new instance buffer.
    // We start by first setting the number of instances of the triangle that will need to be rendered.
    // For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

    // Next we create a temporary instance array using the instance count.
    // Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

    // Create the instance array
    SAFE_CREATE_ARRAY(instances, InstanceType, m_instanceCount);

    // Now here is where we setup the different positions for each instance of the triangle.
    // I have set four different x, y, z positions for each triangle.
    // Note that this is where you could set color, scaling, different texture coordinates, and so forth.
    // An instance can be modified in any way you want it to be.
    // For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

    // Позиция одной инстанции спрайта на экране, поворот этого спрайта на заданный угол и коэффициент масштабирования
    // Координаты в формате: (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол

    instances[0].position = D3DXVECTOR4(
        float( obj->getPosX() - 0.5f * scrWidth),
        float(-obj->getPosY() + 0.5f * scrHeight),
        float( obj->getAngle() ),
		float( obj->getScale() )
    );

    // Размеры кадра анимации (для текстурного атласа only) и номер анимации
    instances[0].animationInfo = D3DXVECTOR3(
        m_spriteSliceX,                         // ширина одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
        m_spriteSliceY,                         // высота одного кадра --- ??? надо перенести в другой буфер, который берется один раз на кадр
        (float)obj->getAnimPhase()              // фаза анимации - номер текстуры в массиве или порядковый номер кадра в атласе
    );



    // The instance buffer description is setup exactly the same as a vertex buffer description.
    // Set up the description of the instance buffer.
    instanceBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.ByteWidth      = sizeof(InstanceType)* m_instanceCount;
    instanceBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;
    instanceBufferDesc.MiscFlags      = 0;
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
    CHECK_FAILED(result);

    // Release the instance array now that the instance buffer has been created and loaded.
    SAFE_DELETE_ARRAY(instances);

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------
