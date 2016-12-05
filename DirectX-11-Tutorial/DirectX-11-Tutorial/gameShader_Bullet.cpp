#include "stdafx.h"
#include "gameShader_Bullet.h"

bulletShader_Instancing::bulletShader_Instancing()
{
	m_vertexShader = 0;
	m_pixelShader  = 0;
	m_layout       = 0;
	m_matrixBuffer = 0;
	m_sampleState  = 0;
}

bulletShader_Instancing::bulletShader_Instancing(const bulletShader_Instancing &other)
{
}

bulletShader_Instancing::~bulletShader_Instancing()
{
}

bool bulletShader_Instancing::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// The new texture.vs and texture.ps HLSL files are loaded for this shader.
	// Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, L"../DirectX-11-Tutorial/_gameShader_Bullet.vs", L"../DirectX-11-Tutorial/_gameShader_Bullet.ps");

	if (!result)
		return false;

	return true;
}

// The Shutdown function calls the release of the shader variables.
void bulletShader_Instancing::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
	return;
}

// The Render 1 function now takes a new parameter called texture which is the pointer to the texture resource.
// This is then sent into the SetShaderParameters function so that the texture can be set in the shader and then used for rendering.
bool bulletShader_Instancing::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                                            D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                                            ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, -1, -1);
	if (!result)
		return false;

	// Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

// The Render 2 function now takes as input a vertex count and an instance count instead of the old index count
bool bulletShader_Instancing::Render(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount,
											D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                                            ID3D11ShaderResourceView* texture, int X, int Y)
{
	bool result;

	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, X, Y);
	if (!result)
		return false;

	// Now render the prepared buffers with the shader
	RenderShader(deviceContext, vertexCount, instanceCount);

	return true;
}

// 
bool bulletShader_Instancing::Render(ID3D11DeviceContext* deviceContext, int indexCount,
                                            D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                                            ID3D11ShaderResourceView* texture, bool sendTexture)
{
	bool result;

	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, sendTexture);
	if (!result)
		return false;

	// Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

// The Render 3 function now takes as input a vertex count and an instance count instead of the old index count.
// The Render 3 function now takes as input a pointer to the texture array. This will give the shader access to the two textures for blending operations.
bool bulletShader_Instancing::Render(ID3D11DeviceContext* deviceContext, ciRef vertexCount, ciRef instanceCount,
                                            const D3DXMATRIX &worldMatrix, const D3DXMATRIX &viewMatrix, const D3DXMATRIX &projectionMatrix,
                                            ID3D11ShaderResourceView** textureArray, ciRef rotationMode, ciRef rotateToX, ciRef rotateToY)
{
    bool result;
    
    // Set the shader parameters that it will use for rendering
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, rotationMode, rotateToX, rotateToY);
    if (!result)
        return false;
    
    // Now render the prepared buffers with the shader
    RenderShader(deviceContext, vertexCount, instanceCount);

    return true;
}

// InitializeShader sets up the texture shader.
bool bulletShader_Instancing::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
 	HRESULT                   result;
 	ID3D10Blob               *errorMessage;
 	ID3D10Blob               *vertexShaderBuffer;
 	ID3D10Blob               *pixelShaderBuffer;

    // polygonLayout[...] содержит описание полей, которые передаются в шейдер для каждого полигона.
    // Должен совпадать со структурой VertexInputType в шейдере и со структурами VertexType/InstanceType в классе модели/спрайта
    D3D11_INPUT_ELEMENT_DESC  polygonLayout[4];     
 	unsigned int              numElements;
 	D3D11_BUFFER_DESC         matrixBufferDesc;
    D3D11_SAMPLER_DESC        samplerDesc;          // We have a new variable to hold the description of the texture sampler that will be setup in this function.
  


	// Initialize the pointers this function will use to null
	errorMessage       = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer  = 0;

	// Load in the new texture vertex and pixel shaders.
	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result)) {

		if (errorMessage)
            // If the shader failed to compile it should have writen something to the error message
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			// If there was nothing in the error message then it simply could not find the shader file itself
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result)) {

		if (errorMessage)
            // If the shader failed to compile it should have writen something to the error message
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			// If there was  nothing in the error message then it simply could not find the file itself
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    CHECK_FAILED(result);

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    CHECK_FAILED(result);



	// The input layout has changed as we now have a texture element instead of color.
	// The first position element stays unchanged but the SemanticName and Format of the second element have been changed to TEXCOORD and DXGI_FORMAT_R32G32_FLOAT.
	// These two changes will now align this layout with our new VertexType in both the ModelClass definition and the typedefs in the shader files

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName         = "POSITION";
 	polygonLayout[0].SemanticIndex        = 0;
 	polygonLayout[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
 	polygonLayout[0].InputSlot            = 0;
 	polygonLayout[0].AlignedByteOffset    = 0;
 	polygonLayout[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
  	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName         = "TEXCOORD";
	polygonLayout[1].SemanticIndex        = 0;
	polygonLayout[1].Format               = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot            = 0;
	polygonLayout[1].AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
  	polygonLayout[1].InstanceDataStepRate = 0;

	// new for instancing
	{
		// For instancing we are going to add a third element to the layout.
		// As we plan to position the four instanced triangles in different positions in 3D space we need to send the shader an extra position vector
		// composed of three floats representing x, y, and z.
		// Therefore the format is set to DXGI_FORMAT_R32G32B32_FLOAT.
		// The semantic name is set to TEXCOORD as we are just using a generic semantic.
		// Note that we have to set the semantic index to 1 in the layout since there is already a TEXCOORD for the texture coordinates using slot 0.

		// Now for the instancing specific stuff we set the InputSlotClass to D3D11_INPUT_PER_INSTANCE_DATA which indicates that this is instanced data.
		// Secondly the InstanceDataStepRate is now used and we set the step rate to 1 so that it will draw one instance before stepping forward a unit
		// in the instance data.
		// Note also that this is the first unit in the instance buffer so we set the AlignedByteOffset to 0 again since we are not aligning to the vertex buffer
		// with the instance data.

		polygonLayout[2].SemanticName		  = "TEXCOORD";
		polygonLayout[2].SemanticIndex		  = 1;
		polygonLayout[2].Format				  = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot			  = 1;
		polygonLayout[2].AlignedByteOffset    = 0;
		polygonLayout[2].InputSlotClass		  = D3D11_INPUT_PER_INSTANCE_DATA;
  		polygonLayout[2].InstanceDataStepRate = 1;

        polygonLayout[3].SemanticName         = "TEXCOORD";
        polygonLayout[3].SemanticIndex        = 2;
        polygonLayout[3].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[3].InputSlot            = 1;
        polygonLayout[3].AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[3].InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
        polygonLayout[3].InstanceDataStepRate = 1;
	}

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    CHECK_FAILED(result);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage				 = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth			 = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags			 = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    CHECK_FAILED(result);



	// The sampler state description is setup here and then can be passed to the pixel shader after.
	// The most important element of the texture sampler description is Filter. Filter will determine how it decides which pixels will be used or combined
	// to create the final look of the texture on the polygon face.
	// In the example here I use D3D11_FILTER_MIN_MAG_MIP_LINEAR which is more expensive in terms of processing but gives the best visual result.
	// It tells the sampler to use linear interpolation for minification, magnification, and mip - level sampling.

	// AddressU and AddressV are set to Wrap which ensures that the coordinates stay between 0.0f and 1.0f.
	// Anything outside of that wraps around and is placed between 0.0f and 1.0f.
	// All other settings for the sampler state description are defaults.

	// Create a texture sampler state description.
    // ??? !!! optimize - try using another filter in order to get more speed !!! ???
	samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
 	samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
 	samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
 	samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
 	samplerDesc.MipLODBias     = 0.0f;
    samplerDesc.MaxAnisotropy  = 1;
 	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  	samplerDesc.BorderColor[0] = 0;
  	samplerDesc.BorderColor[1] = 0;
  	samplerDesc.BorderColor[2] = 0;
  	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD         = 0;
	samplerDesc.MaxLOD         = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    CHECK_FAILED(result);

	return true;
}

// The ShutdownShader function releases all the variables used in the bulletShader_Instancing.
void bulletShader_Instancing::ShutdownShader()
{
	// The ShutdownShader function now releases the new sampler state that was created during initialization.

	// Release the sampler state
    SAFE_RELEASE(m_sampleState);

	// Release the matrix constant buffer
    SAFE_RELEASE(m_matrixBuffer);

	// Release the layout
    SAFE_RELEASE(m_layout);

	// Release the pixel shader
    SAFE_RELEASE(m_pixelShader);

	// Release the vertex shader
    SAFE_RELEASE(m_vertexShader);

	return;
}

// OutputShaderErrorMessage writes out errors to a text file if the HLSL shader could not be loaded.
void bulletShader_Instancing::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("____shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
		fout << compileErrors[i];

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader. Check '__shader-error.txt' for message.", shaderFilename, MB_OK);

	return;
}

// SetShaderParameters function now takes in a pointer to a texture resource and then assigns it to the shader using the new texture resource pointer.
// Note that the texture has to be set before rendering of the buffer occurs.
// Теперь метод принимает в качестве параметра массив текстур
bool bulletShader_Instancing::SetShaderParameters(ID3D11DeviceContext* deviceContext,
										D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
                                        ID3D11ShaderResourceView **textureArray,
                                        ciRef rotationMode, ciRef rotateToX, ciRef rotateToY)
{
	HRESULT					 result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int			 bufferNumber;
	MatrixBufferType		*dataPtr;


	// Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix,	   &worldMatrix     );
	D3DXMatrixTranspose(&viewMatrix,	   &viewMatrix      );
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CHECK_FAILED(result);

	// Get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer
	dataPtr->world		  = worldMatrix;
	dataPtr->view		  = viewMatrix;
	dataPtr->projection   = projectionMatrix;
	dataPtr->rotationMode = rotationMode;
    dataPtr->rotateToX    = rotateToX;
    dataPtr->rotateToY    = rotateToY;
	dataPtr->dummy        = 0.0f;               // <-- dummy

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// The SetShaderParameters function has been modified from the previous tutorial to include setting the texture in the pixel shader now.
	// Set shader texture resource in the pixel shader.
    // +++
    // Here is where the texture array is set before rendering.
    // The PSSetShaderResources function is used to set the texture array.
    // The first parameter is where to start in the array.
    // The second parameter is how many textures are in the array that is being passed in.
    // And the third parameter is a pointer to the texture array.
	deviceContext->PSSetShaderResources(0, 1, textureArray);

	return true;
}

// SetShaderParameters function now takes in a pointer to a texture resource and then assigns it to the shader using the new texture resource pointer.
// Note that the texture has to be set before rendering of the buffer occurs.
bool bulletShader_Instancing::SetShaderParameters(ID3D11DeviceContext* deviceContext,
										D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, int X, int Y)
{
	HRESULT					 result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int			 bufferNumber;
	MatrixBufferType		*dataPtr;


	// Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix,	   &worldMatrix     );
	D3DXMatrixTranspose(&viewMatrix,	   &viewMatrix      );
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CHECK_FAILED(result);

	// Get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer
	dataPtr->world		  = worldMatrix;
	dataPtr->view		  = viewMatrix;
	dataPtr->projection   = projectionMatrix;
	dataPtr->rotationMode = 1.0f;
	dataPtr->rotateToX	  = X;
	dataPtr->rotateToY    = Y;
	dataPtr->dummy        = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// The SetShaderParameters function has been modified from the previous tutorial to include setting the texture in the pixel shader now.
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool bulletShader_Instancing::SetShaderParameters(ID3D11DeviceContext* deviceContext,
										D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, bool sendTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CHECK_FAILED(result);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world		  = worldMatrix;
	dataPtr->view		  = viewMatrix;
	dataPtr->projection   = projectionMatrix;
	dataPtr->rotationMode = 0.0f;
	dataPtr->rotateToX	  = 0.0f;
	dataPtr->rotateToY    = 0.0f;
	dataPtr->dummy        = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// The SetShaderParameters function has been modified from the previous tutorial to include setting the texture in the pixel shader now.
	// Set shader texture resource in the pixel shader.
	if (sendTexture)
		deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

// RenderShader calls the shader technique to render the polygons.
void bulletShader_Instancing::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader,  NULL, 0);

	// The RenderShader function has been changed to include setting the sample state in the pixel shader before rendering.
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

// The RenderShader function is different in two ways.
// First is that it takes as input the vertex and instance count instead of how it used to take in just an index count.
// Secondly it uses the DrawInstanced function to draw the triangles instead of using the DrawIndexed function.
void bulletShader_Instancing::RenderShader(ID3D11DeviceContext* deviceContext, const int &vertexCount, const int &instanceCount)
{
	// Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader,  NULL, 0);

	// Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle
	deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);

	return;
}
