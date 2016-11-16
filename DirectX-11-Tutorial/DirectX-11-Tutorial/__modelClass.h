#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>

#include "__textureClass.h"


class ModelClass {
 private:
/*
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};
*/
/*
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
*/
	 struct VertexType {
		 D3DXVECTOR3 position;
		 D3DXVECTOR2 texture;
		 D3DXVECTOR3 normal;
	 };

 public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);				// для простой отрисовки по координатам
	bool Initialize(ID3D11Device*, WCHAR*);		// для текстурирования
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	// The ModelClass now lso has a GetTexture function so it can pass its own texture resource to shaders that will draw this model
	ID3D11ShaderResourceView* GetTexture();

 private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// ModelClass has now a private LoadTexture() and ReleaseTexture() for loading and releasing the texture that will be used to render this model
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

 private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;

	int m_vertexCount;
	int m_indexCount;

	// The m_Texture variable is used for loading, releasing, and accessing the texture resource for this model.
	TextureClass* m_Texture;
};

#endif
