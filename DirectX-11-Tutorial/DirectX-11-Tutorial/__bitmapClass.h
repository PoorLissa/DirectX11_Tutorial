// --------------------------------------------------------------------------------------------------------
// BitmapClass will be used to represent an individual 2D image that needs to be rendered to the screen.
// For every 2D image you have you will need a new BitmapClass for each.
// Note that this class is just the ModelClass re-written to handle 2D images instead of 3D objects.
// --------------------------------------------------------------------------------------------------------

#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include "__textureClass.h"



class BitmapClass {
 private:
	 // Each bitmap image is still a polygon object that gets rendered similar to 3D objects.
	 // For 2D images we just need a position vector and texture coordinates.

	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

 public:
	BitmapClass();
	BitmapClass(const BitmapClass &);
   ~BitmapClass();

	bool Initialize(ID3D11Device *, int, int, WCHAR *, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext *, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

 private:
	bool InitializeBuffers(ID3D11Device *);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *, int, int);
	void RenderBuffers(ID3D11DeviceContext *);

	bool LoadTexture(ID3D11Device *, WCHAR *);
	void ReleaseTexture();

 private:
	ID3D11Buffer	*m_vertexBuffer, *m_indexBuffer;
	int				 m_vertexCount, m_indexCount;
	TextureClass	*m_Texture;

	// The BitmapClass will need to maintain some extra information that a 3D model wouldn't,
	// such as the screen size, the bitmap size, and the last place it was rendered.
	// We have added extra private variables here to track that extra information.

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif
