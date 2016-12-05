#pragma once
#ifndef _SPRITE_INSTANCED_H_
#define _SPRITE_INSTANCED_H_

#include "__bitmapClassInstancing.h"
#include "gameClasses.h"

/* --------------------------------------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------------------------------- */



// ����� ����������������� �������
class InstancedSprite : public BitmapClass_Instancing {

 public:
    InstancedSprite(int width, int height) : scrWidth(width), scrHeight(height) {}
   ~InstancedSprite() {}

    // --- ������������� ��������� ---

    // �������� �� ���� ������ � ������� � �� ����� ������� ��������� ������ ���������������� ������
    bool initializeInstances(ID3D11Device *device, std::vector<gameObjectBase*> *vec);

    // �������� �� ���� ������ � ������� � �� ����� ������ ��������� ������ ���������������� ������
    bool initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize);

	// �������� �� ���� ������ � ������� � ����� � ��������� ��������� ������ ���������������� ������ (���� ����������� �����, �� ��� ��� ��������� ������)
    bool initializeInstances(ID3D11Device *device, std::list<gameObjectBase*> *list, const unsigned int *listSize, bool bullet);

    // �������� �� ���� ����� ���� ������, ���������������� �� ���� (��� ����������� ������)
    bool initializeInstances(ID3D11Device *device, gameObjectBase* obj);

 private:
    int scrWidth;
    int scrHeight;
};

#endif
