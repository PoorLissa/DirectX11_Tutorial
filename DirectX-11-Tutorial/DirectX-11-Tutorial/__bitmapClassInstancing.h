// --------------------------------------------------------------------------------------------------------
// BitmapClass will be used to represent an individual 2D image that needs to be rendered to the screen.
// For every 2D image you have you will need a new BitmapClass for each.
// Note that this class is just the ModelClass re-written to handle 2D images instead of 3D objects.
// --------------------------------------------------------------------------------------------------------

#ifndef _BITMAPCLASSINSTANCING_H_
#define _BITMAPCLASSINSTANCING_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include "__textureClass.h"



class BitmapClass_Instancing {
private:
	// Each bitmap image is still a polygon object that gets rendered similar to 3D objects.
	// For 2D images we just need a position vector and texture coordinates.
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	// We add a new structure that will hold the instance information.
	// In this tutorial we are modifying the position of each instance of the triangle so we use a position vector.
	// But note that it could be anything else you want to modify for each instance such as color, size, rotation, and so forth.
	// You can modify multiple things at once for each instance also.

	struct InstanceType {
		D3DXVECTOR3 position;	// position содержит 2 координаты, по которым будет размешен спрайт, и угол поворота, на который этот спрайт нужно развернуть
		//float		angle;
	};

public:
	BitmapClass_Instancing();
	BitmapClass_Instancing(const BitmapClass_Instancing &);
   ~BitmapClass_Instancing();

	bool Initialize(ID3D11Device *, int, int, WCHAR *, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext *, int, int);

	ID3D11ShaderResourceView* GetTexture();

	// We have two new functions for getting the vertex and instance counts.
	// We also removed the helper function which previously returned the index count as the instance count has replaced that.
	int GetVertexCount();
	int GetInstanceCount();

	bool initializeInstances(ID3D11Device *);

private:
	bool InitializeBuffers(ID3D11Device *);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *, int, int);
	void RenderBuffers(ID3D11DeviceContext *);

	bool LoadTexture(ID3D11Device *, WCHAR *);
	void ReleaseTexture();

private:
	ID3D11Buffer	*m_vertexBuffer;
	int				 m_vertexCount;
	TextureClass	*m_Texture;

	// The BitmapClass will need to maintain some extra information that a 3D model wouldn't,
	// such as the screen size, the bitmap size, and the last place it was rendered.
	// We have added extra private variables here to track that extra information.
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

	// The BitmapClass now has an instance buffer instead of an index buffer.
	// All buffers in DirectX 11 are generic so it uses the ID3D11Buffer type.
	ID3D11Buffer	*m_instanceBuffer;
	// The index count has been replaced with the instance count.
	int				 m_instanceCount;
};

#endif
