#include "__textureArrayClass.h"

TextureArrayClass::TextureArrayClass()
{
    m_textures;
    m_textures;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

// Initialize takes in the Direct3D device and file name of the texture and then loads the texture file into the
// shader resource variable called m_texture.The texture can now be used to render with.
// ++ Initialize takes in the two texture file names and creates two texture resources in the texture array from those files.
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
    HRESULT result;

    //arr = new ID3D11ShaderResourceView*[qty];

    // Load the textures in (we can use DDS, BMP, PNG and JPG).
    result = D3DX11CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_textures[0], NULL);
    if (FAILED(result))
        return false;

    result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_textures[1], NULL);
    if (FAILED(result))
        return false;

    return true;
}

// The Shutdown function releases the texture resource if it has been loaded and then sets the pointer to null.
void TextureArrayClass::Shutdown()
{
    // Release the texture resource.
    if (m_textures[0]) {
        m_textures[0]->Release();
        m_textures[0] = 0;
    }

    if (m_textures[1]) {
        m_textures[1]->Release();
        m_textures[1] = 0;
    }

    return;
}

// GetTexture is the function that is called by other objects that need access to the texture shader resource so that they can use the texture for rendering.
// GetTextureArray returns a pointer to the texture array so calling objects can have access to the textures in the texture array.
ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
    return m_textures;
}
