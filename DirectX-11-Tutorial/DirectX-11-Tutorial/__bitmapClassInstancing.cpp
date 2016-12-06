// Some info in how to optimize the renderer
// https://docs.unity3d.com/Manual/OptimizingGraphicsPerformance.html

#include "stdafx.h"
#include "__bitmapClassInstancing.h"



BitmapClass_Instancing::BitmapClass_Instancing()
{
	m_vertexBuffer   = 0;
	m_Texture	     = 0;
    m_TextureArray   = 0;
	m_instanceBuffer = 0;
}

BitmapClass_Instancing::BitmapClass_Instancing(const BitmapClass_Instancing& other)
{
}

BitmapClass_Instancing::~BitmapClass_Instancing()
{
}

// »нициализаци€ одной текстурой из файла
bool BitmapClass_Instancing::Initialize(ID3D11Device *device, int screenWidth, int screenHeight,
                                            WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// In the Initialize function both the screen size and image size are stored.
	// These will be required for generating exact vertex locations during rendering.
	// Note that the pixels of the image do not need to be exactly the same as the texture that is used,
	// you can set this to any size and use any size texture you want also.

	// Store the screen size.
	m_screenWidth =  screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth  = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// The previous rendering location is first initialized to negative one.
	// This will be an important variable that will locate where it last drew this image.
	// If the image location hasn't changed since last frame then it won't modify the dynamic vertex buffer which will save us some cycles.

	// Initialize the previous rendering position to negative one.
/*
	m_previousPosX = -1;
	m_previousPosY = -1;
*/


	// The buffers are then created and the texture for this bitmap image is also loaded in.

	// Initialize the vertex and index buffers.
    result = InitializeBuffers(device, (screenWidth - bitmapWidth)*0.5f, (screenHeight - bitmapHeight)*0.5f);
	if (!result)
		return false;

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
		return false;

	return true;
}

// »нициализаци€ массивом текстур из списка файлов
bool BitmapClass_Instancing::Initialize(ID3D11Device *device, ciRef screenWidth, ciRef screenHeight,
                                            WCHAR** textureFilenames, ciRef filesQty,
                                            ciRef bitmapWidth,  ciRef bitmapHeight,
                                            cfRef spriteSliceX, cfRef spriteSliceY)
{
    bool result;

    // In the Initialize function both the screen size and image size are stored.
    // These will be required for generating exact vertex locations during rendering.
    // Note that the pixels of the image do not need to be exactly the same as the texture that is used,
    // you can set this to any size and use any size texture you want also.

    // Store the screen size
    m_screenWidth  = screenWidth;
    m_screenHeight = screenHeight;

    // Store the size in pixels that this bitmap should be rendered at
    m_bitmapWidth  = bitmapWidth;
    m_bitmapHeight = bitmapHeight;

    // «апомним размеры одного кадра из текстурного атласа
    m_spriteSliceX = spriteSliceX;
    m_spriteSliceY = spriteSliceY;

    // The buffers are created and the texture for this bitmap image is also loaded in

    // Initialize the vertex and index buffers.
    result = InitializeBuffers(device, (screenWidth - bitmapWidth)*0.5f, (screenHeight - bitmapHeight)*0.5f);
    if (!result)
        return false;

    // Load the texture for this model.
    // We call the LoadTextures function which takes in multiple file names for textures
    // that will be loaded into the texture array.
    result = LoadTexture(device, textureFilenames, filesQty);
    if (!result)
        return false;

    return true;
}

// The Shutdown function will release the vertex and index buffers as well as the texture that was used for the bitmap image.
void BitmapClass_Instancing::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

// Render puts the buffers of the 2D image on the video card
bool BitmapClass_Instancing::Render(ID3D11DeviceContext *deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);

	return true;
}

// InitializeBuffers is the function that is used to build the vertex and index buffer that will be used to draw the 2D image
bool BitmapClass_Instancing::InitializeBuffers(ID3D11Device *device, const int &positionX, const int &positionY)
{
    float		             left, right, top, bottom;
    VertexType				*vertices;
    D3D11_BUFFER_DESC		 vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA	 vertexData;
    HRESULT					 result;

    // Set the number of vertices in the vertex array:
    // We set the vertices to 6 since we are making a square out of 2 triangles, so 6 points are needed
    m_vertexCount = 6;

    // Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
        return false;

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, sizeof(VertexType) * m_vertexCount);

    // -----------------------------------------------------------------------------------------------------------------------------------
    // “еперь мы совсем отказываемс€ от использовани€ динамического буфера дл€ вершин и инициализируем буфер сразу при создании,
    // т.к. хотим скорости и потому что вершины квада не мен€ютс€ со временем.
    // Ќа вход мы получаем координаты центра экрана и переносим спрайт так, что он своим центром лежит в левом верхнем углу

    // The four sides of the image need to be calculated.
    // See the diagram at the top of the tutorial for a complete explaination:
    // In DirectX's 2d scene the point (0, 0) lies at the center of the screen.

    left   = (float)positionX - (float)((m_screenWidth / 2));       // Calculate the screen coordinates of the left side of the bitmap
    right  = left + (float)m_bitmapWidth;                           // Calculate the screen coordinates of the right side of the bitmap
    top    = (float)(m_screenHeight / 2) - (float)positionY;        // Calculate the screen coordinates of the top of the bitmap
    bottom = top - (float)m_bitmapHeight;                           // Calculate the screen coordinates of the bottom of the bitmap

    // Load the vertex array with data:
    // First triangle
    vertices[0].position = D3DXVECTOR3(left, top, 0.0f);		// Top left
    vertices[0].texture  = D3DXVECTOR2(0.0f, 0.0f);
    vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);	// Bottom right
    vertices[1].texture  = D3DXVECTOR2(1.0f, 1.0f);
    vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);		// Bottom left
    vertices[2].texture  = D3DXVECTOR2(0.0f, 1.0f);

    // Second triangle
    vertices[3].position = D3DXVECTOR3(left, top, 0.0f);		// Top left
    vertices[3].texture  = D3DXVECTOR2(0.0f, 0.0f);
    vertices[4].position = D3DXVECTOR3(right, top, 0.0f);		// Top right
    vertices[4].texture  = D3DXVECTOR2(1.0f, 0.0f);
    vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);	// Bottom right
    vertices[5].texture  = D3DXVECTOR2(1.0f, 1.0f);

    // -----------------------------------------------------------------------------------------------------------------------------------

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth      = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags      = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
        return false;

    // Release the array now that the vertex buffer have been created and loaded
    SAFE_DELETE(vertices);

    // instancing - ¬ынесено в отдельный метод initializeInstances

    return true;
}

// это был вариант дл€ отладки, сейчас не используетс€, т.к. дл€ этого существует класс-наследник (InstancedSprite),
// который умеет инициализировать данные из вход€щего массива
bool BitmapClass_Instancing::initializeInstances(ID3D11Device *device) {

	InstanceType			*instances;
	D3D11_BUFFER_DESC		 instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA   instanceData;

	// We will now setup the new instance buffer.
	// We start by first setting the number of instances of the triangle that will need to be rendered.
	// For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

	// Set the number of instances in the array.
	m_instanceCount = 30;

	// Next we create a temporary instance array using the instance count.
	// Note we use the InstanceType structure for the array type which is defined in the ModelClass header file.

	// Create the instance array.
	instances = new InstanceType[m_instanceCount];
	if (!instances)
		return false;

	// Now here is where we setup the different positions for each instance of the triangle.
	// I have set four different x, y, z positions for each triangle.
	// Note that this is where you could set color, scaling, different texture coordinates, and so forth.
	// An instance can be modified in any way you want it to be.
	// For this tutorial I used position as it is easy to see visually which helps understand how instancing works.

	static float angle = 0.0f;
	int Width  = 800;
	int Height = 600;
	int Size   = 24;

	//  оординаты в формате (0, 0) - верхний левый угол экрана, (maxX, maxY) - нижний правый угол
	for (int i = 0; i < m_instanceCount; i++) {
		int X = 400 + 12 + ( 300 * sin(float(i))) * cos(float(100*i))*sin(float(0.2*i));
		int Y = 300 - 12 + ( 300 * cos(float(i))) * cos(float(100*i))*sin(float(0.2*i));

		instances[i].position = D3DXVECTOR4(float(X - Width / 2 - Size / 2), float(-Y + Height / 2 - Size / 2), 10 * angle / (i + 1), 1.0f);
	}

	angle += m_instanceCount / 1000000.0;



	// The instance buffer description is setup exactly the same as a vertex buffer description.
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage		  = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth	  = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags	  = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags	  = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
	// Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem		  = instances;
	instanceData.SysMemPitch	  = 0;
	instanceData.SysMemSlicePitch = 0;

	SAFE_RELEASE(m_instanceBuffer);

	// Create the instance buffer.
	HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
		return false;

	// Release the instance array now that the instance buffer has been created and loaded.
	SAFE_DELETE_ARRAY(instances);

	return true;
}

// ShutdownBuffers releases the vertex and index buffers.
void BitmapClass_Instancing::ShutdownBuffers()
{
	// Release the instance buffer
	SAFE_RELEASE(m_instanceBuffer);

	// Release the vertex buffer
	SAFE_RELEASE(m_vertexBuffer);

	return;
}

// The RenderBuffers function sets up the vertex and index buffers on the GPU to be drawn by the shader
void BitmapClass_Instancing::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	unsigned int  strides[2];
	unsigned int  offsets[2];
	ID3D11Buffer *bufferPointers[2];

	// The instance buffer is just a second vertex buffer containing different information so it is set on the device at the same time using the same
	// call as the vertex buffer. So instead of how we previously sent in a single stride, offset, and buffer we now send an array of strides, offsets,
	// and buffers to the IASetVertexBuffers call.

	// First we set the two strides to the size of the VertexType and InstanceType.
	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// We then set the offsets for both the vertex and instance buffer.
	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Next we create an array that holds the pointers to the vertex buffer and the instance buffer.

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Finally we set both the vertex buffer and the instance buffer on the device context in the same call.

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// The following function loads the texture that will be used for drawing the 2D image.
bool BitmapClass_Instancing::LoadTexture(ID3D11Device *device, WCHAR *filename)
{
	bool result;

	// Create the texture object
	SAFE_CREATE(m_Texture, TextureClass);

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

// The following function loads the texture that will be used for drawing the 2D image
// ¬ерси€ дл€ загрузки массива текстур
bool BitmapClass_Instancing::LoadTexture(ID3D11Device *device, WCHAR **fileNames, const int &fileQty)
{
    bool result;

    // Create the texture object
	SAFE_CREATE(m_TextureArray, TextureArrayClass);

    result = m_TextureArray->Initialize(device, fileNames, fileQty);
    if (!result)
        return false;

    return true;
}

//This ReleaseTexture function releases the texture that was loaded.
void BitmapClass_Instancing::ReleaseTexture()
{
	// Release the texture object
	SAFE_SHUTDOWN(m_Texture);

    // Release the texture array
	SAFE_SHUTDOWN(m_TextureArray);

	return;
}
