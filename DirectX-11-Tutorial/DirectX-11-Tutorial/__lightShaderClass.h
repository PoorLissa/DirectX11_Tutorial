
// The new LightShaderClass is just the TextureShaderClass from the previous tutorials re-written slightly to incorporate lighting.
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;



class LightShaderClass {
 private:
	struct MatrixBufferType {
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	// The new LightBufferType structure will be used for holding lighting information.
	// This typedef is the same as the new typedef in the pixel shader.
	// Do note that I add a extra float for size padding to ensure the structure is a multiple of 16.
	// Since the structure without an extra float is only 28 bytes CreateBuffer would have failed if we used a sizeof(LightBufferType), because
	// it requires sizes that are a multiple of 16 to succeed.
#if 0
	struct LightBufferType {
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float		padding;		// Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};
#endif

	// The LightBufferType has been modified to hold a specular color and specular power to match the light constant buffer in the pixel shader.
	// Pay attention to the fact that I placed the specular power by the light direction to form a 4 float slot
	// instead of using padding so that the structure could be kept in multiples of 16 bytes.
	// Also had specular power been placed last in the structure and no padding used beneath light direction then the shader would not have functioned correctly.
	// This is because even though the structure was a multiple of 16 the individual slots themselves were not aligned logically to 16 bytes each.

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float		specularPower;
		D3DXVECTOR4 specularColor;
	};

	// We add a new camera buffer structure to match the new camera constant buffer in the vertex shader.
	// Note we add a padding to make the structure size a multiple of 16 to prevent CreateBuffer failing when using sizeof with this structure.

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

 public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass &);
	~LightShaderClass();

	bool Initialize(ID3D11Device *, HWND);
	void Shutdown();
	//bool Render(ID3D11DeviceContext *, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *, D3DXVECTOR3, D3DXVECTOR4);
	//bool Render(ID3D11DeviceContext *, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	bool Render(ID3D11DeviceContext *, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *,
					D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

 private:
	bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

	bool SetShaderParameters(ID3D11DeviceContext *, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView *, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext *, int);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4,
		D3DXVECTOR3, D3DXVECTOR4, float);

 private:
	ID3D11VertexShader	*m_vertexShader;
	ID3D11PixelShader	*m_pixelShader;
	ID3D11InputLayout	*m_layout;
	ID3D11SamplerState	*m_sampleState;
	ID3D11Buffer		*m_matrixBuffer;

	// There is a new private constant buffer for the light information(color and direction).
	// The light buffer will be used by this class to set the global light variables inside the HLSL pixel shader.
	ID3D11Buffer		*m_lightBuffer;

	// We add a new camera constant buffer here which will be used for setting the camera position in the vertex shader
	ID3D11Buffer		*m_cameraBuffer;
};

#endif
