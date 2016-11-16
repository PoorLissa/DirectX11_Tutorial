// Some info in how to optimize the renderer
// https://docs.unity3d.com/Manual/OptimizingGraphicsPerformance.html

#include "__bitmapClassInstancing.h"

BitmapClass_Instancing::BitmapClass_Instancing()
{
	m_vertexBuffer   = 0;
	m_Texture	     = 0;
	m_instanceBuffer = 0;
}

BitmapClass_Instancing::BitmapClass_Instancing(const BitmapClass_Instancing& other)
{
}

BitmapClass_Instancing::~BitmapClass_Instancing()
{
}

bool BitmapClass_Instancing::Initialize(ID3D11Device *device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
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
	m_previousPosX = -1;
	m_previousPosY = -1;



	// The buffers are then created and the texture for this bitmap image is also loaded in.

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
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

// Render puts the buffers of the 2D image on the video card.
// As input it takes the position of where to render the image on the screen.
// The UpdateBuffers function is called with the position parameters.
// If the position has changed since the last frame it will then update the location of the vertices in the dynamic vertex buffer to the new location.
// If not it will skip the UpdateBuffers function.
// After that the RenderBuffers function will prepare the final vertices / indices for rendering.
bool BitmapClass_Instancing::Render(ID3D11DeviceContext *deviceContext, int positionX, int positionY)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
		return false;

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

// These are the two new helper functions which return the vertex and instance counts.
int BitmapClass_Instancing::GetVertexCount()
{
	return m_vertexCount;
}

int BitmapClass_Instancing::GetInstanceCount()
{
	return m_instanceCount;
}

// The GetTexture function returns a pointer to the texture resource for this 2D image.
// The shader will call this function so it has access to the image when drawing the buffers.
ID3D11ShaderResourceView* BitmapClass_Instancing::GetTexture()
{
	return m_Texture->GetTexture();
}

// InitializeBuffers is the function that is used to build the vertex and index buffer that will be used to draw the 2D image.
bool BitmapClass_Instancing::InitializeBuffers(ID3D11Device *device)
{
	VertexType				*vertices;
	unsigned long			*indices;
	D3D11_BUFFER_DESC		 vertexBufferDesc;
	D3D11_BUFFER_DESC		 indexBufferDesc;
	D3D11_SUBRESOURCE_DATA	 vertexData;
	D3D11_SUBRESOURCE_DATA	 indexData;
	HRESULT					 result;

	InstanceType			*instances;
	D3D11_BUFFER_DESC		 instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA   instanceData;

	// We set the vertices to six since we are making a square out of two triangles, so six points are needed. The indices will be the same.

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType)* m_vertexCount));

	// Here is the big change in comparison to the ModelClass.
	// We are now creating a dynamic vertex buffer so we can modify the data inside the vertex buffer each frame if we need to.
	// To make it dynamic we set Usage to D3D11_USAGE_DYNAMIC and CPUAccessFlags to D3D11_CPU_ACCESS_WRITE in the description.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth		= sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags		= 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem			= vertices;
	vertexData.SysMemPitch		= 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	// new part instancing
	if(false)
	{
		// We will now setup the new instance buffer.
		// We start by first setting the number of instances of the triangle that will need to be rendered.
		// For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

		// Set the number of instances in the array.
		m_instanceCount = 10000;
		
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

		// Load the instance array with data.
		for (int i = 0; i < m_instanceCount; i++)
			instances[i].position = D3DXVECTOR3(-50.0f + 333*cos(100.0*i)*sin(float(.2*i)), -50.0f + 333*cos(100.0*i)*cos(float(.2*i)), 0.0f);


		// The instance buffer description is setup exactly the same as a vertex buffer description.

		// Set up the description of the instance buffer.
		// ??? not really?
		instanceBufferDesc.Usage			= D3D11_USAGE_DEFAULT;
		instanceBufferDesc.ByteWidth		= sizeof(InstanceType) * m_instanceCount;
		instanceBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags	= 0;
		instanceBufferDesc.MiscFlags		= 0;
		instanceBufferDesc.StructureByteStride = 0;

		// Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
		// Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

		// Give the subresource structure a pointer to the instance data.
		instanceData.pSysMem		  = instances;
		instanceData.SysMemPitch	  = 0;
		instanceData.SysMemSlicePitch = 0;

		// Create the instance buffer.
		result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
		if (FAILED(result))
			return false;

		// Release the instance array now that the instance buffer has been created and loaded.
		delete[] instances;
		instances = 0;
	}

	return true;
}

bool BitmapClass_Instancing::initializeInstances(ID3D11Device *device) {

	InstanceType			*instances;
	D3D11_BUFFER_DESC		 instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA   instanceData;

	// We will now setup the new instance buffer.
	// We start by first setting the number of instances of the triangle that will need to be rendered.
	// For this tutorial I have manually set it to 4 so that we will have four triangles rendered on the screen.

	// Set the number of instances in the array.
	m_instanceCount = 30000;

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

	// ¬ качестве координат передаем смещение от центра
	for (int i = 0; i < m_instanceCount; i++) {
		//instances[i].position = D3DXVECTOR3(-50.0f + 333 * cos(100.0*i)*sin(float(.2*i)), -50.0f + 333 * cos(100.0*i)*cos(float(.2*i)), i);
		//instances[i].position = D3DXVECTOR3(400.0f - 15.0*i, -300.0f - 15.0*i, 10*angle/i);

		int X = 400 + 12 + ( 300 * sin(float(i))) * cos(float(100*i))*sin(float(0.2*i));
		int Y = -300 + 12 + (- 300 * cos(float(i))) * cos(float(100*i))*sin(float(0.2*i));;

		int Width  = 800;
		int Height = 600;

		int Size = 24;

		instances[i].position = D3DXVECTOR3(float(X - Width/2 - Size/2), float(Y + Height/2 - Size/2), 10*angle/i);
	}

	angle += m_instanceCount / 1000;


	// The instance buffer description is setup exactly the same as a vertex buffer description.
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType)* m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Just like the vertex buffer we get the pointer to the instance array and then create the instance buffer.
	// Once the instance buffer is created we can release the temporary instance array since the data from the array has been copied into the instance buffer.

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem		  = instances;
	instanceData.SysMemPitch	  = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	HRESULT result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
		return false;

	// Release the instance array now that the instance buffer has been created and loaded.
	delete[] instances;
	instances = 0;

	return true;
}

// ShutdownBuffers releases the vertex and index buffers.
void BitmapClass_Instancing::ShutdownBuffers()
{
	// Release the instance buffer.
	if (m_instanceBuffer) {
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// The UpdateBuffers function is called each frame to update the contents of the dynamic vertex buffer
// to re-position the 2D bitmap image on the screen if need be.
bool BitmapClass_Instancing::UpdateBuffers(ID3D11DeviceContext *deviceContext, int positionX, int positionY)
{
	// We check if the position to render this image has changed.
	// If it hasn't changed then we just exit since the vertex buffer doesn't need any changes for this frame.
	// This check can save us a lot of processing.
	if (positionX == m_previousPosX && positionY == m_previousPosY)
		return true;

	// If the position to render this image has changed then we record the new location for the next time we come through this function.
	m_previousPosX = positionX;
	m_previousPosY = positionY;


	float		 left, right, top, bottom;
	VertexType	*vertices;
	VertexType	*verticesPtr;
	HRESULT		 result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// The four sides of the image need to be calculated.
	// See the diagram at the top of the tutorial for a complete explaination:
	// In DirectX's 2d scene the point (0, 0) lies at the center of the screen.

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)positionX - (float)((m_screenWidth / 2));
	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;
	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;
	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;


	// Now that the coordinates are calculated create a temporary vertex array and fill it with the new six vertex points.

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);		// Top left
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);	// Bottom right
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);		// Bottom left
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);		// Top left
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);		// Top right
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);	// Bottom right
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);


	// Now copy the contents of the vertex array into the vertex buffer using the Map and memcpy functions.

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType)* m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
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

	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

//This ReleaseTexture function releases the texture that was loaded.
void BitmapClass_Instancing::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
