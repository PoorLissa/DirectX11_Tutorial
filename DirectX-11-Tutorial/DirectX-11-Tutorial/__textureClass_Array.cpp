#include "stdafx.h"
#include "__textureClass_Array.h"

TextureArrayClass::TextureArrayClass()
{
    m_textures = nullptr;
    m_texQty   = 0;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

// »нициализируем массив текстур файлами из списка
// http://hghltd.yandex.net/yandbtm?fmode=inject&url=http%3A%2F%2Fwww.gamedev.net%2Ftopic%2F636900-how-to-create-a-texture2darray-from-files-in-dx11%2F&tld=ru&lang=en&la=1478087296&tm=1479712063&text=Texture2DArray%20ArraySize%20D3D11_TEXTURE2D_DESC&l10n=ru&mime=html&sign=a197c7315dc18e38bd2c486f93eccbc9&keyno=0
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR **fileNames, const int &qty)
{
    if( qty <= 0 || !fileNames )
        return false;

    m_texQty = qty;

    HRESULT                result;
    D3DX11_IMAGE_LOAD_INFO loadInfo;
    ID3D11Texture2D      **pTexture2Ds = new ID3D11Texture2D*[m_texQty];
    if (!pTexture2Ds)
        return false;

    ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
    loadInfo.Width          = D3DX11_FROM_FILE;
    loadInfo.Height         = D3DX11_FROM_FILE;
    loadInfo.Depth          = D3DX11_FROM_FILE;
    loadInfo.FirstMipLevel  = 0;
    loadInfo.MipLevels      = 0;
    loadInfo.Usage          = D3D11_USAGE_STAGING;
    loadInfo.BindFlags      = 0;
    loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    loadInfo.MiscFlags      = 0;
    loadInfo.Format         = DXGI_FORMAT_R8G8B8A8_UNORM;
    loadInfo.Filter         = D3DX11_FILTER_NONE;
    loadInfo.MipFilter      = D3DX11_FILTER_LINEAR;         // ‘ильтр, который будет использоватьс€ при ресемплинге.  роме прочего, если фильтра нет, то все ломаетс€.
    loadInfo.pSrcInfo       = 0;                            // ¬озможно, хотим передавать фильтр сюда в качестве параметра

    // —оздаем ресурс с текстурами
    for (int i = 0; i < m_texQty; i++) {
        result = D3DX11CreateTextureFromFile(device, fileNames[i], &loadInfo, 0, (ID3D11Resource**)&pTexture2Ds[i], 0);
        CHECK_FAILED(result);
    }

    //---------------------------------------------------------------------------------------
    //	Create the texture array.  Each element in the texture array has the same format/dimensions
    //---------------------------------------------------------------------------------------
    D3D11_TEXTURE2D_DESC    texElementDesc;
    pTexture2Ds[0]->GetDesc(&texElementDesc);

    // ??? - !!! - ƒл€ текстуры 256x256 этот параметр равен 9, и при этом она не уменьшаетс€ до размеров квада, в который вписываетс€.
    // ƒела€ его равным единице получаем ожидаемое поведение, при котором текстура вписываетс€ в размер спрайта
    // Ќужно разобратьс€, что здесь не так, и как сделать правильно
    // ??? - ”меньшенна€ текстура рендеритс€ некрасиво, что-то здесь тоже не так
    // ¬се заработало, когда выставил loadInfo.MipFilter
    //texElementDesc.MipLevels = 1;

    D3D11_TEXTURE2D_DESC  texArrayDesc;
    texArrayDesc.Width              = texElementDesc.Width;
    texArrayDesc.Height             = texElementDesc.Height;
    texArrayDesc.MipLevels          = texElementDesc.MipLevels;
    texArrayDesc.ArraySize          = m_texQty;
    texArrayDesc.Format             = texElementDesc.Format;
    texArrayDesc.SampleDesc.Count   = 1;
    texArrayDesc.SampleDesc.Quality = 0;
    texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
    texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texArrayDesc.CPUAccessFlags     = 0;
    texArrayDesc.MiscFlags          = 0;

    ID3D11Texture2D* texArray = nullptr;
    result = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
    //---------------------------------------------------------------------------------------
    //	Copy individual texture elements into texture array.
    //---------------------------------------------------------------------------------------
    // for each texture element...
    ID3D11DeviceContext* pd3dImmediateContext;
    device->GetImmediateContext(&pd3dImmediateContext);
    D3D11_MAPPED_SUBRESOURCE MappedResource;

    for (UINT texElement = 0; texElement < m_texQty; ++texElement) {
        // for each mipmap level
        for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel) {

            result = pd3dImmediateContext->Map(pTexture2Ds[texElement], mipLevel, D3D11_MAP_READ, 0, &MappedResource);

            pd3dImmediateContext->UpdateSubresource(texArray,
                D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels), 0, MappedResource.pData, MappedResource.RowPitch, 0);

            pd3dImmediateContext->Unmap(pTexture2Ds[texElement], mipLevel);
        }
    }



    //---------------------------------------------------------------------------------------
    //					Create a resource view to the texture array.
    //---------------------------------------------------------------------------------------

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format                         = texArrayDesc.Format;
    viewDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    viewDesc.Texture2DArray.MostDetailedMip = 0;
    viewDesc.Texture2DArray.MipLevels       = texArrayDesc.MipLevels;
    viewDesc.Texture2DArray.FirstArraySlice = 0;
    viewDesc.Texture2DArray.ArraySize       = m_texQty;

    m_textures = nullptr;
    result = device->CreateShaderResourceView(texArray, &viewDesc, &m_textures);
    //---------------------------------------------------------------------------------------

    SAFE_RELEASE(texArray);

    for (int i = 0; i < m_texQty; i++)
        SAFE_RELEASE(pTexture2Ds[i]);

    SAFE_DELETE_ARRAY(pTexture2Ds);

    return true;
}

// Initialize takes in the Direct3D device and file name of the texture and then loads the texture file into the
// shader resource variable called m_texture. The texture can now be used to render with.
// ++ Initialize takes in the two texture file names and creates two texture resources in the texture array from those files.
bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
    HRESULT result;

    return false;

    // Ѕольше не используетс€, используем инициализацию списком файлов
#if 0
    ID3D11ShaderResourceView **m_textures = new ID3D11ShaderResourceView*[2];
    if( !m_textures )
        return false;

    // Load the textures in (we can use DDS, BMP, PNG and JPG).
    result = D3DX11CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_textures[0], NULL);
    CHECK_FAILED(result);

    result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_textures[1], NULL);
    CHECK_FAILED(result);

    return true;
#endif
}

// The Shutdown function releases the texture resource if it has been loaded and then sets the pointer to null.
void TextureArrayClass::Shutdown()
{
    // Release the texture array resource
    if( m_textures ) {
        m_textures[0].Release();
        m_textures->Release();
        m_textures = 0;
    }

    return;
}
