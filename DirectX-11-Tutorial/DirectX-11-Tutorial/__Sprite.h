#pragma once
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "__bitmapClass.h"

// �����-������� ��� BitmapClass, ��������� ��� ����, ����� ����� ����������� �������� ������ ��������,
// � ������� ���� �� ���� � �� �� ��������, ������� �� ����� ���� ��� �� ���� ������ ������������ �� � GPU

// ������ �������������:
// ����� ������ ��������� ���� ��� �������� m_spriteVec[0]->Render(...),
// � ����� � ����� ���� ��� �������� m_TextureShader->Render(..., true) � ���������� ��������, � ����� �������� �� �� � ���������� false - ��� ��������� ��������

// ��. __graphicsClass.cpp -> "test-fast-render"

// ����� �������, ������ ���� ������� ������������ ����� InstancedSprite



class Sprite {

 public:

	Sprite(int x, int y) : posX(x), posY(y) {}
   ~Sprite() {}

	bool Render(ID3D11DeviceContext *deviceContext, int positionX, int positionY) {
		Bitmap->Render(deviceContext, positionX, positionY);
		return true;
	}

	static void setBitmap(BitmapClass *bitmap) {
		if( !Bitmap)
			Bitmap = bitmap;
	}

	void getCoords(int &x, int &y) {
		x = posX;
		y = posY;
	}

	int getIndexCount() {
		return Bitmap->GetIndexCount();
	}

	ID3D11ShaderResourceView* getTexture() {
		return Bitmap->GetTexture();
	}

	void setX(int x) {
		posX = x;
	}

	void setY(int y) {
		posY = y;
	}

 private:
	static bool setBitmap(ID3D11Device *device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight) {

		Bitmap = new BitmapClass;
		if (!Bitmap)
			return false;

		// Initialize the bitmap object.
		// ??? why 256?
		return Bitmap->Initialize(device, screenWidth, screenHeight, textureFilename, 256, 256);
	}

 private:
	 int posX, posY;
	 static BitmapClass *Bitmap;
};

#endif
