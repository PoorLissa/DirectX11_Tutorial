#pragma once
// The TextureArrayClass replaces the TextureClass that was used before.
// Instead of having just a single texture it can now have multiple textures and give calling objects access to those textures.
// For this tutorial it just handles two textures but it can easily be expanded.

#include <d3d11.h>
#include <d3dx11tex.h>
#include "Helpers.h"


class TextureArrayClass {
 public:
    TextureArrayClass();
    TextureArrayClass(const TextureArrayClass &);
   ~TextureArrayClass();

    bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
    bool Initialize(ID3D11Device*, WCHAR**, const int &);
    void Shutdown();

    // The GetTextureArray function returns a pointer to the texture Array resource so that it can be used for rendering by shaders
    ID3D11ShaderResourceView **GetTextureArray();

 private:
    //ID3D11ShaderResourceView *m_textures[2];
    UINT m_texQty;
    ID3D11ShaderResourceView   *m_textures;
};
