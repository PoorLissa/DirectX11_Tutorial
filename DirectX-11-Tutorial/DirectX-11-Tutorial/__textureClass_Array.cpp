#include "__textureClass_Array.h"

TextureArrayClass::TextureArrayClass()
{
    m_textures = 0;
    texCount   = 0;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

// Initialize takes in the Direct3D device and file name of the texture and then loads the texture file into the
// shader resource variable called m_texture. The texture can now be used to render with.
// ++ Initialize takes in the two texture file names and creates two texture resources in the texture array from those files.
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
#if 0

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

#else

    // http://hghltd.yandex.net/yandbtm?fmode=inject&url=http%3A%2F%2Fwww.gamedev.net%2Ftopic%2F636900-how-to-create-a-texture2darray-from-files-in-dx11%2F&tld=ru&lang=en&la=1478087296&tm=1479712063&text=Texture2DArray%20ArraySize%20D3D11_TEXTURE2D_DESC&l10n=ru&mime=html&sign=a197c7315dc18e38bd2c486f93eccbc9&keyno=0
    HRESULT result;

    //UINT iNumOfMaterials = g_sFileNames.size();
    UINT iNumOfMaterials = 2;

    //std::vector<ID3D11Texture2D*> pTexture2Ds;
    //pTexture2Ds.resize(iNumOfMaterials);
    ID3D11Texture2D* pTexture2Ds[2];

    D3DX11_IMAGE_LOAD_INFO loadInfo;
    ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));

    loadInfo.Width          = 256;
    loadInfo.Height         = 256;
    loadInfo.Depth          = 0;
    loadInfo.FirstMipLevel  = 0;
    loadInfo.MipLevels      = 0;
    loadInfo.Usage          = D3D11_USAGE_STAGING;
    loadInfo.BindFlags      = 0;
    loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    loadInfo.MiscFlags      = 0;
    loadInfo.Format         = DXGI_FORMAT_R8G8B8A8_UNORM;
    loadInfo.Filter         = D3DX11_FILTER_NONE;
    loadInfo.MipFilter      = D3DX11_FILTER_NONE;
    loadInfo.pSrcInfo       = 0;

    result = D3DX11CreateTextureFromFile(device, filename1, &loadInfo, 0, (ID3D11Resource**)&pTexture2Ds[0], 0);
    result = D3DX11CreateTextureFromFile(device, filename2, &loadInfo, 0, (ID3D11Resource**)&pTexture2Ds[1], 0);

    //---------------------------------------------------------------------------------------
    //	Create the texture array.  Each element in the texture array has the same format/dimensions
    //---------------------------------------------------------------------------------------
    D3D11_TEXTURE2D_DESC    texElementDesc;
    pTexture2Ds[0]->GetDesc(&texElementDesc);

    D3D11_TEXTURE2D_DESC  texArrayDesc;
    texArrayDesc.Width              = texElementDesc.Width;
    texArrayDesc.Height             = texElementDesc.Height;
    texArrayDesc.MipLevels          = texElementDesc.MipLevels;
    texArrayDesc.ArraySize          = iNumOfMaterials;
    texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    texArrayDesc.SampleDesc.Count   = 1;
    texArrayDesc.SampleDesc.Quality = 0;
    texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
    texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texArrayDesc.CPUAccessFlags     = 0;
    texArrayDesc.MiscFlags          = 0;

    ID3D11Texture2D* texArray = 0;
    result = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
    //---------------------------------------------------------------------------------------
    //	Copy individual texture elements into texture array.
    //---------------------------------------------------------------------------------------
    // for each texture element...
    ID3D11DeviceContext* pd3dImmediateContext;
    device->GetImmediateContext(&pd3dImmediateContext);
    //for (UINT i = 0; i < g_sFileNames.size(); ++i)
    for (UINT i = 0; i < iNumOfMaterials; ++i) {
        // for each mipmap level...
        for (UINT j = 0; j < texElementDesc.MipLevels; ++j) {

            D3D11_MAPPED_SUBRESOURCE MappedResource;
            result = pd3dImmediateContext->Map(pTexture2Ds[i], j, D3D11_MAP_READ, 0, &MappedResource);

            pd3dImmediateContext->UpdateSubresource(texArray, D3D11CalcSubresource(j, i, texElementDesc.MipLevels), 0, MappedResource.pData, MappedResource.RowPitch, 0);

            pd3dImmediateContext->Unmap(pTexture2Ds[i], j);
        }
    }
    //---------------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------------
    //					Create a resource view to the texture array.
    //---------------------------------------------------------------------------------------

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format                         = texArrayDesc.Format;
    viewDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    viewDesc.Texture2DArray.MostDetailedMip = 0;
    viewDesc.Texture2DArray.MipLevels       = texArrayDesc.MipLevels;
    viewDesc.Texture2DArray.FirstArraySlice = 0;
    viewDesc.Texture2DArray.ArraySize       = iNumOfMaterials;

    m_textures = 0;
    result = device->CreateShaderResourceView(texArray, &viewDesc, &m_textures);
    //---------------------------------------------------------------------------------------

    SAFE_RELEASE(texArray);
    for (int i = 0; i < iNumOfMaterials; i++)
        SAFE_RELEASE(pTexture2Ds[i]);

    return true;

#endif
}

// The Shutdown function releases the texture resource if it has been loaded and then sets the pointer to null.
void TextureArrayClass::Shutdown()
{
    // Release the texture array resource
    if (m_textures) {
        m_textures[0].Release();
        m_textures->Release();
        m_textures = 0;
    }

/*
    // Release the texture resource
    SAFE_RELEASE(m_textures[0]);
    SAFE_RELEASE(m_textures[1]);
*/
    return;
}

// GetTexture is the function that is called by other objects that need access to the texture shader resource so that they can use the texture for rendering.
// GetTextureArray returns a pointer to the texture array so calling objects can have access to the textures in the texture array.
ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
    return &m_textures;
}
