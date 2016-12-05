// The TextureShaderClass is just an updated version of the ColorShaderClass from the previous tutorial.
// This class will be used to draw the 3D models using vertex and pixel shaders.

#pragma once
#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_

using namespace std;



class TextureShaderClass {

    // ���������, ������� ������������ ��� �������� ���������� � cbuffer �������. ������ ��������� ������ ���� ������ 16. ����������� ���� ���������� ����������.
    struct MatrixBufferType {
	    D3DXMATRIX world;
	    D3DXMATRIX view;
	    D3DXMATRIX projection;
    };

 public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass &);
   ~TextureShaderClass();

	bool Initialize(ID3D11Device *, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext *, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *);
	// new
	bool Render(ID3D11DeviceContext *, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *, bool);

 private:
	bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

	bool SetShaderParameters(ID3D11DeviceContext *, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *);
	// new
	bool SetShaderParameters(ID3D11DeviceContext *, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *, bool);
	void RenderShader(ID3D11DeviceContext *, int);

 private:
	ID3D11VertexShader	*m_vertexShader;
	ID3D11PixelShader	*m_pixelShader;
	ID3D11InputLayout	*m_layout;
	ID3D11Buffer		*m_matrixBuffer;

	// There is a new private variable for the sampler state pointer. This pointer will be used to interface with the texture shader.
	ID3D11SamplerState	*m_sampleState;
};

#endif
