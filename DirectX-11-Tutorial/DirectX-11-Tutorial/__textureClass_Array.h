// The TextureArrayClass replaces the TextureClass that was used before.
// Instead of having just a single texture it can now have multiple textures and give calling objects access to those textures.
// For this tutorial it just handles two textures but it can easily be expanded.
#pragma once
#ifndef _TEXTURECLASS_ARRAY_H_
#define _TEXTURECLASS_ARRAY_H_



class TextureArrayClass {
 public:
    TextureArrayClass();
    TextureArrayClass(const TextureArrayClass &);
   ~TextureArrayClass();

    bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
    bool Initialize(ID3D11Device*, WCHAR**, const int &);
    void Shutdown();

    // GetTexture is the function that is called by other objects that need access to the texture shader resource so that they can use the texture for rendering.
    // GetTextureArray returns a pointer to the texture array so calling objects can have access to the textures in the texture array.
    inline ID3D11ShaderResourceView **GetTextureArray() { return &m_textures; }

 private:
    //ID3D11ShaderResourceView *m_textures[2];
    UINT m_texQty;
    ID3D11ShaderResourceView   *m_textures;
};

#endif
