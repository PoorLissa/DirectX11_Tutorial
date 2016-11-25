#pragma once

#include <iterator>
#include "__bitmapClassInstancing.h"
#include "Helpers.h"

/*
    ����� ����������������� �������.
 ����������� �� ������ BitmapClass_Instancing.
 ��������� ���������������� ���������, ������������� ������ BitmapClass_Instancing, ������� �������� gameObjectBase

    ������ �������������:
1. �������������� ������ InstancedSprite
2. ������ ������/������ ��������, �������������� �� gameObjectBase
3. ��� ����������� ������� onTimer ������������� ���������� � ��������� ���� �������� ������ � �������� ����� initializeInstances:
    if (!sprIns1->initializeInstances(m_d3d->GetDevice(), &monstersVector))
        return false;
4. ��� ����������� �� ����������� ������� onTimer �������� ����� Render...
    if (!sprIns1->Render(m_d3d->GetDeviceContext(), xCenter - bmpSize / 2, yCenter - bmpSize / 2))
        return false;
5. ... � ������������ ��� ��� ������ �������:
    result = m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                sprIns1->GetVertexCount(), sprIns1->GetInstanceCount(),
                    worldMatrixZ * matTrans * matScale,
                        viewMatrix, orthoMatrix, sprIns1->GetTexture(), mouseX - xCenter, yCenter - mouseY);
*/



// ������� ����� ��� �������� �������
class gameObjectBase {
 public:
    gameObjectBase(float x, float y, float angle, float speed) : _X(x), _Y(y), _Angle(angle), _Speed(speed) {}
    virtual ~gameObjectBase() {}

    inline float getX()     const            { return _X;     } 
    inline float getY()     const            { return _Y;     }
    inline float getAngle() const            { return _Angle; }
    inline void  setPosX( const float &x)    {        _X = x; }
    inline void  setPosY( const float &y)    {        _Y = y; }
    inline void  setAngle(const float &a)    {    _Angle = a; }

    // ����� ��� ����������� �������, ���������� � ����� �����
    virtual void Move(const float &, const float &, void* = 0) = 0;
	// ����� ��� ��������� ������� ���� ��������
	virtual int  getAnimPhase() = 0;

 protected:
    bool    _Alive;
    float   _X;
    float   _Y;
    float   _Speed;
    float   _Angle;
};

// ����� �������� ������� - �����
class Player : public gameObjectBase {
public:
    Player(float x, float y, float angle = 0.0f, float speed = 1.0f) : gameObjectBase(x, y, angle, speed) {}
   ~Player() {}

    virtual void Move(const float &x, const float &y, void *Param) {
    }

 private:
    //int color;
};

// ����� �������� ������� - ������
class Monster : public gameObjectBase {
 public:
    Monster(float x, float y, float angle, float speed, int interval, int anim_Qty) : gameObjectBase(x, y, angle, speed),
        animInterval0(interval), animInterval1(interval), animQty(anim_Qty), animPhase(0) {
    }
   ~Monster() {}

    virtual void Move(const float &x, const float &y, void *Param) {

        float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

#if 0
        dX = div_Speed_by_Dist * dX * 0.1f;
        dY = div_Speed_by_Dist * dY * 0.1f;
#else
        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) / 100;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) / 100;
#endif

        _X += dX;
        _Y += dY;

		animInterval1--;

		if( animInterval1 < 0 ) {
			animInterval1 = animInterval0;

			animPhase++;

			if(animPhase > animQty)
				animPhase = 0;
		}
    }

	int getAnimPhase() {
		return animPhase;
	}

 private:
	 int animInterval0, animInterval1;
	 int animQty, animPhase;
};



// ����� ����������������� �������

// 
class InstancedSprite : public BitmapClass_Instancing {
 public:
    InstancedSprite(int width, int height) : scrWidth(width), scrHeight(height) {}
   ~InstancedSprite() {}

    // ������������� ���������: 
    // �������� �� ���� ������ � ������� � �� ����� ������� ��������� ������ ���������������� ������
    bool initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec) {

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
            // ������� ������� �� ������ � ������� ������� �� �������� ����
            // ���������� � �������: (0, 0) - ������� ����� ���� ������, (maxX, maxY) - ������ ������ ����
            instances[i].position = D3DXVECTOR3(
                float( (*iter)->getX() - 0.5f * scrWidth  ),
                float(-(*iter)->getY() + 0.5f * scrHeight ),
                float(-90.0f )
            );

            // ������� ����� �������� (��� ����������� ������ only) � ����� ��������
            instances[i].animationInfo = D3DXVECTOR3(
                m_spriteSliceX,                         // ������ ������ ����� --- ??? ���� ��������� � ������ �����, ������� ������� ���� ��� �� ����
                m_spriteSliceY,                         // ������ ������ ����� --- ??? ���� ��������� � ������ �����, ������� ������� ���� ��� �� ����
                (*iter)->getAnimPhase()                 // ���� �������� - ����� �������� � ������� ��� ���������� ����� ����� � ������
            );
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

        // ������� m_instanceBuffer, ����� ��� ������ ����� ��������� ������ ������
        SAFE_RELEASE(m_instanceBuffer);

        // Create the instance buffer.
        HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
        CHECK_FAILED(result);

        // Release the instance array now that the instance buffer has been created and loaded.
        SAFE_DELETE_ARRAY(instances);

        return true;
    }

 private:
    int scrWidth;
    int scrHeight;
};



/*
    �������� ����� � ����������� ������ ����� �����������.
    ���������� ������ ��������� ����� ����������� �� ���� ������� ������������� ������, �.�. ������� FPS ����� �����������
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

        // ���������� � ������� (0, 0) - ������� ����� ���� ������, (maxX, maxY) - ������ ������ ����
        std::vector<gameObjectBase*>::iterator iter, end;
        int i;

        // ��������� Instance Buffer �� ������� ������ �������
        for (i = 0, iter = vec->begin(), end = vec->end(); iter != end; ++iter, ++i)
            instances[i].position = D3DXVECTOR3(
                float( (*iter)->getX() - Width  / 2 - Size / 2),
                float(-(*iter)->getY() + Height / 2 - Size / 2),
                float( 10 * angle / (i + 1) )
            );

        angle += m_instanceCount / 1000000.0f;


        // ��������� ��������� ������ ��� ������ �������
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

        // ������� m_instanceBuffer, ����� ������ ������
        if (m_instanceBuffer) {
            m_instanceBuffer->Release();
            m_instanceBuffer = 0;
        }

        // Create the instance buffer.
        HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
        CHECK_FAILED(result);

        return true;
    }

 private:
    InstanceType            *instances;
    D3D11_BUFFER_DESC		 instanceBufferDesc;
    D3D11_SUBRESOURCE_DATA   instanceData;
};


