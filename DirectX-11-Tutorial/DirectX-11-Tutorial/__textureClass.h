// The TextureClass encapsulates the loading, unloading, and accessing of a single texture resource.For each texture needed an object of this class must be instantiated.

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <d3dx11tex.h>



class TextureClass {
 public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	
	// The GetTexture function returns a pointer to the texture resource so that it can be used for rendering by shaders.
	ID3D11ShaderResourceView* GetTexture();

 private:
	ID3D11ShaderResourceView* m_texture;
};

#endif