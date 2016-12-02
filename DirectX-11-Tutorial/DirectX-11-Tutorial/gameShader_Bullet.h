// Модифицированная версия класса TextureShaderClass_Instancing
// Используется для отрисовки пуль
// Это файл не движка, а присущий конкретному проекту, поэтому он именуется как "gameShader_Bullet.h" без подчеркиваний

#pragma once
#ifndef _GAME_SHADER_BULLET_H_
#define _GAME_SHADER_BULLET_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "Helpers.h"

// для функции D3DCompileFromFile
#pragma comment(lib, "d3dcompiler.lib")
#include "D3Dcompiler.h"

using namespace std;

#define ciRef const int &

class bulletShader_Instancing {

    // Структура, которая используется для передачи параметров в cbuffer шейдера. Размер структуры должен быть кратен 16. Недостающие поля добиваются пустышками.
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		float	   rotationMode;
		float	   rotateToX;
        float	   rotateToY;
        float      dummy;           // <-- пустышка
	};

 public:
	bulletShader_Instancing();
	bulletShader_Instancing(const bulletShader_Instancing &);
   ~bulletShader_Instancing();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, bool);			// new
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, int, int);   // new instancing
    // new for Texture Arrays
    bool Render(ID3D11DeviceContext*, ciRef, ciRef, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX&, ID3D11ShaderResourceView**, ciRef, ciRef, ciRef);

 private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView* , int, int);
    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView* , bool);       // new
    // new for Texture Arrays
    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, ciRef, ciRef, ciRef);

    void RenderShader(ID3D11DeviceContext*, int);
    void RenderShader(ID3D11DeviceContext*, const int &, const int &);      // new for instancing

 private:
	ID3D11VertexShader	*m_vertexShader;
	ID3D11PixelShader	*m_pixelShader;
	ID3D11InputLayout	*m_layout;
	ID3D11Buffer		*m_matrixBuffer;
	ID3D11SamplerState	*m_sampleState;     // Sampler state pointer. This pointer will be used to interface with the texture shader
};

#endif
