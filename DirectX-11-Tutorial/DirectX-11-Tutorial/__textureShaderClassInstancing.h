// The TextureShaderClass is just an updated version of the ColorShaderClass from the previous tutorial.
// This class will be used to draw the 3D models using vertex and pixel shaders.

#ifndef _TEXTURESHADERCLASSINSTANCING_H_
#define _TEXTURESHADERCLASSINSTANCING_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

class TextureShaderClass_Instancing {
 private:
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		float	   testX;
		float	   testY;
		float	   z1, z2;
	};

 public:
	TextureShaderClass_Instancing();
	TextureShaderClass_Instancing(const TextureShaderClass_Instancing &);
   ~TextureShaderClass_Instancing();

	bool Initialize(ID3D11Device*, HWND, bool = false);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
    bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, bool);                // new
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, int, int);       // new instancing
    bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, int, int);      // new new for Texture Arrays

 private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, bool = false);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, int, int);
    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, bool);    	// new
    bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, int, int);   // new new for Texture Arrays

    void RenderShader(ID3D11DeviceContext*, int);
    void RenderShader(ID3D11DeviceContext*, int, int);                                                                          // new instancing

 private:
	ID3D11VertexShader	*m_vertexShader;
	ID3D11PixelShader	*m_pixelShader;
	ID3D11InputLayout	*m_layout;
	ID3D11Buffer		*m_matrixBuffer;

	// There is a new private variable for the sampler state pointer. This pointer will be used to interface with the texture shader.
	ID3D11SamplerState	*m_sampleState;
};

#endif
