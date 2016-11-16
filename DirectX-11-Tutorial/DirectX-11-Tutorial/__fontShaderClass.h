// The FontShaderClass is just the TextureShaderClass from the previous tutorial renamed with a couple code changes for rendering fonts.

#ifndef _FONTSHADERCLASS_H_
#define _FONTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

class FontShaderClass {
 private:

	struct ConstantBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	// We have a new structure type to match the PixleBuffer in the pixel shader.
	// It contains just the pixel color of the text that will be rendered.

	struct PixelBufferType
	{
		D3DXVECTOR4 pixelColor;
	};

 public:
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

 private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

 private:
	ID3D11VertexShader	*m_vertexShader;
	ID3D11PixelShader	*m_pixelShader;
	ID3D11InputLayout	*m_layout;
	ID3D11Buffer		*m_constantBuffer;
	ID3D11SamplerState	*m_sampleState;

	// The FontShaderClass has a constant buffer for the pixel color that will be used to render the text fonts with color.
	ID3D11Buffer		*m_pixelBuffer;
};

#endif