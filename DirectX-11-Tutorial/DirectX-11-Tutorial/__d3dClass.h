#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#if defined _DEBUG
	#pragma comment(lib, "d3dx11d.lib")
	#pragma comment(lib, "d3dx10d.lib")
#else
	#pragma comment(lib, "d3dx11.lib")
	#pragma comment(lib, "d3dx10.lib")
#endif

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>



class d3dClass {
 public:
	d3dClass();
	d3dClass(const d3dClass &);
   ~d3dClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device*		 GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX &);
	void GetWorldMatrix(D3DXMATRIX &);
	void GetOrthoMatrix(D3DXMATRIX &);

	void GetVideoCardInfo(char *, int &);

	// We now have two new function in the D3DClass for turning the Z buffer on and off when rendering 2D images.
	void TurnZBufferOn();
	void TurnZBufferOff();

 private:
	bool m_vsync_enabled;
	int	 m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain			*m_swapChain;
	ID3D11Device			*m_device;
	ID3D11DeviceContext		*m_deviceContext;
	ID3D11RenderTargetView	*m_renderTargetView;
	ID3D11Texture2D			*m_depthStencilBuffer;
	ID3D11DepthStencilState	*m_depthStencilState;
	ID3D11DepthStencilView	*m_depthStencilView;
	ID3D11RasterizerState	*m_rasterState;

	D3DXMATRIX				m_projectionMatrix;
	D3DXMATRIX				m_worldMatrix;
	D3DXMATRIX				m_orthoMatrix;

	// There is also a new depth stencil state for 2D drawing.
	ID3D11DepthStencilState	*m_depthDisabledStencilState;
};

#endif
